/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/

#include "ClangAstVisitor.h"
#include "ExpressionVisitor.h"
#include "../CppImportUtilities.h"
#include "TemplateArgumentVisitor.h"

#include <clang/AST/Comment.h>

namespace CppImport {

ClangAstVisitor::ClangAstVisitor(OOModel::Project* project, const QString& projectPath, CppImportLogger* logger)
 : clang_{project, projectPath}, macroImporter_{project, clang_}, log_{logger}
{
	exprVisitor_ = new ExpressionVisitor{this, clang_, log_};
	trMngr_ = new TranslateManager{clang_, project, exprVisitor_};
	utils_ = new CppImportUtilities{log_, exprVisitor_, clang_};
	exprVisitor_->setUtilities(utils_);
	trMngr_->setUtils(utils_);
	templArgVisitor_ = new TemplateArgumentVisitor{clang_, exprVisitor_, utils_, log_};
	ooStack_.push(project);

	commentParser_ = new CommentParser{};
}

ClangAstVisitor::~ClangAstVisitor()
{
	SAFE_DELETE(utils_);
	SAFE_DELETE(exprVisitor_);
	SAFE_DELETE(trMngr_);

	SAFE_DELETE(commentParser_);
}

void ClangAstVisitor::setSourceManagerAndPreprocessor(const clang::SourceManager* sourceManager,
																		const clang::Preprocessor* preprocessor)
{
	Q_ASSERT(sourceManager);
	Q_ASSERT(preprocessor);
	clang_.setSourceManager(sourceManager);
	clang_.setPreprocessor(preprocessor);
	macroImporter_.startTranslationUnit();
}

Model::Node* ClangAstVisitor::ooStackTop()
{
	return ooStack_.top();
}

void ClangAstVisitor::pushOOStack(Model::Node* node)
{
	Q_ASSERT(node);
	ooStack_.push(node);
}

Model::Node* ClangAstVisitor::popOOStack()
{
	return ooStack_.pop();
}

bool ClangAstVisitor::TraverseDecl(clang::Decl* decl)
{
	if (auto project = DCast<OOModel::Project>(ooStack_.top()))
	{
		auto parentProject = clang_.projectForLocation(decl->getLocation());

		if (project != parentProject)
		{
			 ooStack_.pop();
			 ooStack_.push(parentProject);
		}
	}

	return Base::TraverseDecl(decl);
}

bool ClangAstVisitor::VisitDecl(clang::Decl* decl)
{
	if (!shouldImport(decl->getLocation()))
		return true;

	if (decl && !llvm::isa<clang::AccessSpecDecl>(decl) && !llvm::isa<clang::ParmVarDecl>(decl))
	{
		log_->writeError(className_, decl, CppImportLogger::Reason::NOT_SUPPORTED);
		return true;
	}
	return Base::VisitDecl(decl);
}

bool ClangAstVisitor::TraverseNamespaceDecl(clang::NamespaceDecl* namespaceDecl)
{
	if (!shouldImport(namespaceDecl->getLocation()))
		return true;
	OOModel::Module* ooModule = trMngr_->insertNamespace(namespaceDecl);
	if (!ooModule)
	{
		log_->writeError(className_, namespaceDecl, CppImportLogger::Reason::INSERT_PROBLEM);
		// this is a severe error which should not happen therefore stop visiting
		return false;
	}
	// visit the body of the namespace
	ooStack_.push(ooModule);
	clang::DeclContext::decl_iterator it = namespaceDecl->decls_begin();
	for (;it!=namespaceDecl->decls_end();++it)
	{
		TraverseDecl(*it);
	}
	ooStack_.pop();
	return true;
}

bool ClangAstVisitor::TraverseClassTemplateDecl(clang::ClassTemplateDecl* classTemplate)
{
	if (!shouldImport(classTemplate->getLocation()) || !classTemplate->isThisDeclarationADefinition())
		return true;

	OOModel::Class* ooClass = nullptr;
	if (trMngr_->insertClassTemplate(classTemplate, ooClass))
	{
		TraverseClass(classTemplate->getTemplatedDecl(), ooClass);
		// visit type arguments if any
		auto templateParamList = classTemplate->getTemplateParameters();
		for (auto templateParameter : *templateParamList)
			ooClass->typeArguments()->append(templArgVisitor_->translateTemplateArg(templateParameter));
	}
	return true;
}

bool ClangAstVisitor::TraverseClassTemplateSpecializationDecl
(clang::ClassTemplateSpecializationDecl* specializationDecl)
{
	if (!shouldImport(specializationDecl->getLocation()) ||
		 specializationDecl->getExternLoc().getPtrEncoding())
		return true;

	// explicit instantiation declaration
	if (!specializationDecl->isExplicitSpecialization() && specializationDecl->isExplicitInstantiationOrSpecialization())
	{
		auto ooExplicitTemplateInst = trMngr_->insertExplicitTemplateInstantiation(specializationDecl);
		if (!ooExplicitTemplateInst)
			// already inserted
			return true;

		auto typeLoc = specializationDecl->getTypeAsWritten()->getTypeLoc()
							.castAs<clang::TemplateSpecializationTypeLoc>();

		auto ooRef = clang_.createReference(typeLoc.getTemplateNameLoc());
		for (unsigned i = 0; i < typeLoc.getNumArgs(); i++)
			ooRef->typeArguments()->append(utils_->translateTemplateArgument(typeLoc.getArgLoc(i)));

		utils_->setReferencePrefix(ooRef, specializationDecl->getQualifierLoc());

		ooExplicitTemplateInst->setInstantiatedClass(ooRef);
		// add to tree
		if (QString::fromStdString(specializationDecl->getNameAsString()) == "TypedList")
		{
			/*
			 * TypedList import specialization:
			 * whenever we encounter an explicit template instantiation for a class called "TypedList" we look at its only
			 * argument e.g. "OOModel::Boolean" in Model::TypedList<OOModel::Boolean> and then insert the explicit template
			 * instantiation inside the class referred to by the argument.
			 */
			Q_ASSERT(specializationDecl->getTemplateArgs().size() == 1);
			auto templateArgument = specializationDecl->getTemplateArgs().get(0);
			Q_ASSERT(templateArgument.getKind() == clang::TemplateArgument::ArgKind::Type);
			auto typedListOfType = templateArgument.getAsType().getTypePtr();
			Q_ASSERT(typedListOfType);
			auto ooClassParentForTemplateInstantiation = trMngr_->lookupClass(typedListOfType->getAsCXXRecordDecl());
			ooClassParentForTemplateInstantiation->subDeclarations()->append(ooExplicitTemplateInst);
		}
		else if (auto decl = DCast<OOModel::Declaration>(ooStack_.top()))
			decl->subDeclarations()->append(ooExplicitTemplateInst);
		else if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(clang_.createNode<OOModel::DeclarationStatement>(specializationDecl->getSourceRange(),
																									ooExplicitTemplateInst));
		else
			log_->writeError(className_, specializationDecl, CppImportLogger::Reason::INSERT_PROBLEM);
		return true;
	}

	OOModel::Class* ooClass = nullptr;
	if (trMngr_->insertClassTemplateSpec(specializationDecl, ooClass))
	{
		TraverseClass(specializationDecl, ooClass);

		auto typeLoc = specializationDecl->getTypeAsWritten()->getTypeLoc()
							.castAs<clang::TemplateSpecializationTypeLoc>();
		// visit type arguments if any
		for (unsigned i = 0; i < typeLoc.getNumArgs(); i++)
		{
			auto argLoc = typeLoc.getArgLoc(i);
			auto typeArg = clang_.createNode<OOModel::FormalTypeArgument>(argLoc.getSourceRange(),
																							  clang_.spelling(argLoc.getLocation()));
			typeArg->setSpecializationExpression(utils_->translateTemplateArgument(argLoc));
			ooClass->typeArguments()->append(typeArg);
		}
	}
	return true;
}

bool ClangAstVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl* recordDecl)
{
	if (!shouldImport(recordDecl->getLocation()) || !recordDecl->isThisDeclarationADefinition())
		return true;

	OOModel::Class* ooClass = nullptr;
	if (trMngr_->insertClass(recordDecl, ooClass))
	{
		TraverseClass(recordDecl, ooClass);
		// visit type arguments if any
		if (auto describedTemplate = recordDecl->getDescribedClassTemplate())
		{
			auto templateParamList = describedTemplate->getTemplateParameters();
			for (auto templateParameter : *templateParamList)
				ooClass->typeArguments()->append(templArgVisitor_->translateTemplateArg(templateParameter));
		}
	}

	return true;
}

bool ClangAstVisitor::TraverseFunctionDecl(clang::FunctionDecl* functionDecl)
{
	if (!shouldImport(functionDecl->getLocation()) || llvm::isa<clang::CXXMethodDecl>(functionDecl))
		return true;

	if (auto ooFunction = trMngr_->insertFunctionDecl(functionDecl))
	{
		addFunctionModifiers(functionDecl, ooFunction);

		if (!ooFunction->parent())
		{
			// insert in tree
			if (auto curProject = DCast<OOModel::Project>(ooStack_.top()))
				clang_.insertDeclarationInFolder(ooFunction, functionDecl->getLocation(), curProject);
			else if (auto curModule = DCast<OOModel::Module>(ooStack_.top()))
				clang_.insertDeclarationInFolder(ooFunction, functionDecl->getLocation(), curModule);
			else if (auto curClass = DCast<OOModel::Class>(ooStack_.top()))
			{
				// happens in the case of friend functions
				log_->writeError(className_, functionDecl, CppImportLogger::Reason::NOT_SUPPORTED);
				curClass->methods()->append(ooFunction);
			}
			else if (auto statements = DCast<OOModel::StatementItemList>(ooStack_.top()))
				// This happens in statement lists
				statements->append(clang_.createNode<OOModel::DeclarationStatement>(functionDecl->getSourceRange(),
																										  ooFunction));
			else
				log_->writeError(className_, functionDecl, CppImportLogger::Reason::INSERT_PROBLEM);
		}
		if (!ooFunction->items()->size())
			// only visit the body if we have not yet visited it
			// handle body, typeargs and storage specifier
			TraverseFunction(functionDecl, ooFunction);
	}
	else
		log_->writeError(className_, functionDecl, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::TraverseFunctionTemplateDecl(clang::FunctionTemplateDecl* functionDecl)
{
	// this node does not provide any special information
	// therefore it is sufficient to just visit the templated function
	return TraverseDecl(functionDecl->getTemplatedDecl());
}

bool ClangAstVisitor::TraverseVarDecl(clang::VarDecl* varDecl)
{
	if (!shouldImport(varDecl->getLocation()))
		return true;

	if (llvm::isa<clang::ParmVarDecl>(varDecl))
		return true;

	// check if this variable is only from a explicit/implicit template instantiation - if so we do not translate it.
	if (clang::TSK_ExplicitInstantiationDeclaration == varDecl->getTemplateSpecializationKind() ||
			clang::TSK_ExplicitInstantiationDefinition == varDecl->getTemplateSpecializationKind() ||
			clang::TSK_ImplicitInstantiation == varDecl->getTemplateSpecializationKind())
		return true;

	OOModel::VariableDeclaration* ooVarDecl = nullptr;
	OOModel::VariableDeclarationExpression* ooVarDeclExpr = nullptr;
	bool wasDeclared = false;
	if (varDecl->isStaticDataMember())
	{
		if (!(ooVarDecl = trMngr_->insertStaticField(varDecl, wasDeclared)))
		{
			log_->writeError(className_, varDecl, CppImportLogger::Reason::NO_PARENT);
			return true;
		}
	}
	else if (llvm::dyn_cast<clang::NamespaceDecl>(varDecl->getDeclContext()))
	{
		if (!(ooVarDecl = trMngr_->insertNamespaceField(varDecl, wasDeclared)))
		{
			log_->writeError(className_, varDecl, CppImportLogger::Reason::NO_PARENT);
			return true;
		}
	}
	else
	{
		if (!inBody_)
		{
			ooVarDecl = clang_.createNamedNode<OOModel::VariableDeclaration>(varDecl);
			ooVarDeclExpr = clang_.createNode<OOModel::VariableDeclarationExpression>(varDecl->getSourceRange(),
																											  ooVarDecl);
			ooExprStack_.push(ooVarDeclExpr);
		}
		else if (auto project = DCast<OOModel::Project>(ooStack_.top()))
		{
			ooVarDecl = clang_.createNamedNode<OOModel::Field>(varDecl);
			clang_.insertDeclarationInFolder(static_cast<OOModel::Field*>(ooVarDecl), varDecl->getLocation(), project);
		}
		else if (auto module = DCast<OOModel::Module>(ooStack_.top()))
		{
			ooVarDecl = clang_.createNamedNode<OOModel::Field>(varDecl);
			clang_.insertDeclarationInFolder(static_cast<OOModel::Field*>(ooVarDecl), varDecl->getLocation(), module);
		}
		else if (auto ooClass = DCast<OOModel::Class>(ooStack_.top()))
		{
			ooVarDecl = clang_.createNamedNode<OOModel::Field>(varDecl);
			ooClass->fields()->append(ooVarDecl);
		}
		else if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
		{
			ooVarDecl = clang_.createNamedNode<OOModel::VariableDeclaration>(varDecl);
			ooVarDeclExpr = clang_.createNode<OOModel::VariableDeclarationExpression>(varDecl->getSourceRange(),
																											  ooVarDecl);
			itemList->append(clang_.createNode<OOModel::ExpressionStatement>(varDecl->getSourceRange(), ooVarDeclExpr));
		}
		else
		{
			log_->writeWarning(className_, varDecl, CppImportLogger::Reason::INSERT_PROBLEM);
			return true;
		}
	}

	if (varDecl->hasInit())
	{
		if (varDecl->getInitStyle() == clang::VarDecl::InitializationStyle::CInit)
			ooVarDecl->setInitialValue(exprVisitor_->translateExpression(varDecl->getInit()));
		else
		{
			auto constructExpr = llvm::dyn_cast<clang::CXXConstructExpr>(varDecl->getInit()->IgnoreImplicit());

			bool hasInit = true;
			if (varDecl->getInitStyle() == clang::VarDecl::InitializationStyle::ListInit)
				ooVarDecl->setInitializationKind(OOModel::VariableDeclaration::InitializationKind::StandardInitialization);
			else
			{
				ooVarDecl->setInitializationKind(OOModel::VariableDeclaration::InitializationKind::CallInitialization);
				if (constructExpr) hasInit = constructExpr->getParenOrBraceRange().getBegin().getPtrEncoding();
			}

			if (hasInit)
			{
				if (constructExpr)
				{
					auto arguments = exprVisitor_->translateArguments(constructExpr->getArgs(), constructExpr->getNumArgs());
					if (arguments.size() == 1 && DCast<OOModel::ArrayInitializer>(arguments.first()))
						ooVarDecl->setInitialValue(arguments.first());
					else
					{
						auto arrayInitializer = clang_.createNode<OOModel::ArrayInitializer>(varDecl->getInit()
																													->IgnoreImplicit()
																													->getSourceRange());
						for (auto argument : arguments)
							arrayInitializer->values()->append(argument);
						ooVarDecl->setInitialValue(arrayInitializer);
					}
				}
				else
					ooVarDecl->setInitialValue(exprVisitor_->translateExpression(varDecl->getInit()));
			}
		}
	}

	if (wasDeclared)
		// we know the rest of the information already
		return true;

	// set the type
	ooVarDecl->setTypeExpression(utils_->translateQualifiedType(varDecl->getTypeSourceInfo()->getTypeLoc()));
	// modifiers

	if (varDecl->isConstexpr()) ooVarDecl->modifiers()->set(OOModel::Modifier::ConstExpr);
	ooVarDecl->modifiers()->set(utils_->translateStorageSpecifier(varDecl->getStorageClass()));
	ooVarDecl->modifiers()->set(utils_->translateAccessSpecifier(varDecl->getAccess()));
	return true;
}

bool ClangAstVisitor::TraverseFieldDecl(clang::FieldDecl* fieldDecl)
{
	if (!shouldImport(fieldDecl->getLocation()))
		return true;
	OOModel::Field* field = trMngr_->insertField(fieldDecl);
	if (!field)
	{
		log_->writeError(className_, fieldDecl, CppImportLogger::Reason::NO_PARENT);
		return true;
	}
	if (fieldDecl->hasInClassInitializer())
	{
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(fieldDecl->getInClassInitializer());
		if (!ooExprStack_.empty())
			field->setInitialValue(ooExprStack_.pop());
		else
			log_->writeError(className_, fieldDecl->getInClassInitializer(), CppImportLogger::Reason::NOT_SUPPORTED);
		inBody_ = inBody;
	}

	field->setTypeExpression(utils_->translateQualifiedType(fieldDecl->getTypeSourceInfo()->getTypeLoc()));
	// modifiers
	field->modifiers()->set(utils_->translateAccessSpecifier(fieldDecl->getAccess()));
	return true;
}

bool ClangAstVisitor::TraverseEnumDecl(clang::EnumDecl* enumDecl)
{
	if (!shouldImport(enumDecl->getLocation()))
		return true;

	OOModel::Class* ooEnumClass = nullptr;
	if (trMngr_->insertEnum(enumDecl, ooEnumClass))
	{
		ooEnumClass->modifiers()->set(utils_->translateAccessSpecifier(enumDecl->getAccess()));

		// insert in tree
		if (auto curProject = DCast<OOModel::Project>(ooStack_.top()))
			clang_.insertDeclarationInFolder(ooEnumClass, enumDecl->getLocation(), curProject);
		else if (auto curModule = DCast<OOModel::Module>(ooStack_.top()))
			clang_.insertDeclarationInFolder(ooEnumClass, enumDecl->getLocation(), curModule);
		else if (auto curClass = DCast<OOModel::Class>(ooStack_.top()))
			curClass->classes()->append(ooEnumClass);
		else if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(clang_.createNode<OOModel::DeclarationStatement>(enumDecl->getSourceRange(), ooEnumClass));
		else
		{
			log_->writeWarning(className_, enumDecl, CppImportLogger::Reason::INSERT_PROBLEM);
			// no need to further process this enum
			return true;
		}

		bool inBody = inBody_;
		inBody_ = false;
		for (auto it = enumDecl->enumerator_begin(); it != enumDecl->enumerator_end(); ++it)
		{
			auto enumerator = clang_.createNamedNode<OOModel::Enumerator>(*it);
			// check if there is an initializing expression if so visit it first and then add it to the enum
			if (auto e = it->getInitExpr())
			{
				TraverseStmt(e);
				enumerator->setValue(ooExprStack_.pop());
			}
			ooEnumClass->enumerators()->append(enumerator);
		}
		inBody_ = inBody;
	}
	return true;
}

bool ClangAstVisitor::WalkUpFromTypedefNameDecl(clang::TypedefNameDecl* typedefDecl)
{
	// This method is a walkup such that it covers both subtypes (typedef and typealias)
	if (!shouldImport(typedefDecl->getLocation()))
		return true;
	if (auto ooTypeAlias = trMngr_->insertTypeAlias(typedefDecl))
	{
		ooTypeAlias->setTypeExpression(utils_->translateQualifiedType(typedefDecl->getTypeSourceInfo()->getTypeLoc()));
		ooTypeAlias->setName(QString::fromStdString(typedefDecl->getNameAsString()));
		ooTypeAlias->modifiers()->set(utils_->translateAccessSpecifier(typedefDecl->getAccess()));
		if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(clang_.createNode<OOModel::DeclarationStatement>(typedefDecl->getSourceRange(), ooTypeAlias));
		else if (auto declaration = DCast<OOModel::Declaration>(ooStack_.top()))
			declaration->subDeclarations()->append(ooTypeAlias);
		else
			log_->writeError(className_, typedefDecl, CppImportLogger::Reason::INSERT_PROBLEM);
	}
	return true;
}

bool ClangAstVisitor::TraverseTypeAliasTemplateDecl(clang::TypeAliasTemplateDecl* typeAliasTemplate)
{
	if (!shouldImport(typeAliasTemplate->getLocation()))
		return true;
	if (auto ooTypeAlias = trMngr_->insertTypeAliasTemplate(typeAliasTemplate))
	{
		auto typeAlias = typeAliasTemplate->getTemplatedDecl();
		ooTypeAlias->setTypeExpression(utils_->translateQualifiedType(typeAlias->getTypeSourceInfo()->getTypeLoc()));
		ooTypeAlias->setName(QString::fromStdString(typeAliasTemplate->getNameAsString()));
		// type arguments
		auto templateParamList = typeAliasTemplate->getTemplateParameters();
		for (auto templateParameter : *templateParamList)
			ooTypeAlias->typeArguments()->append(templArgVisitor_->translateTemplateArg(templateParameter));
		// insert in tree
		if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(clang_.createNode<OOModel::DeclarationStatement>(typeAliasTemplate->getSourceRange(),
																									ooTypeAlias));
		else if (auto declaration = DCast<OOModel::Declaration>(ooStack_.top()))
			declaration->subDeclarations()->append(ooTypeAlias);
		else
			log_->writeError(className_, typeAliasTemplate, CppImportLogger::Reason::INSERT_PROBLEM);
	}
	return true;
}

bool ClangAstVisitor::TraverseNamespaceAliasDecl(clang::NamespaceAliasDecl* namespaceAlias)
{
	if (!shouldImport(namespaceAlias->getLocation()))
		return true;
	if (auto ooTypeAlias = trMngr_->insertNamespaceAlias(namespaceAlias))
	{
		ooTypeAlias->setName(QString::fromStdString(namespaceAlias->getNameAsString()));

		auto nameRef = utils_->setReferencePrefix(clang_.createReference(namespaceAlias->getAliasLoc()),
																namespaceAlias->getQualifierLoc());
		ooTypeAlias->setTypeExpression(nameRef);
		if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(clang_.createNode<OOModel::DeclarationStatement>(namespaceAlias->getSourceRange(),
																									ooTypeAlias));
		else if (auto declaration = DCast<OOModel::Declaration>(ooStack_.top()))
			declaration->subDeclarations()->append(ooTypeAlias);
		else
		{
			clang_.deleteNode(ooTypeAlias);
			log_->writeError(className_, namespaceAlias, CppImportLogger::Reason::INSERT_PROBLEM);
		}
	}
	return true;
}

bool ClangAstVisitor::TraverseUsingDecl(clang::UsingDecl* usingDecl)
{
	if (!shouldImport(usingDecl->getLocation()))
		return true;
	if (auto ooNameImport = trMngr_->insertUsingDecl(usingDecl))
	{
		ooNameImport->modifiers()->set(utils_->translateAccessSpecifier(usingDecl->getAccess()));

		auto nameRef = utils_->setReferencePrefix(clang_.createReference(usingDecl->getNameInfo().getSourceRange()),
																usingDecl->getQualifierLoc());
		ooNameImport->setImportedName(nameRef);
		if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(clang_.createNode<OOModel::DeclarationStatement>(usingDecl->getSourceRange(), ooNameImport));
		else if (auto declaration = DCast<OOModel::Declaration>(ooStack_.top()))
			declaration->subDeclarations()->append(ooNameImport);
		else
		{
			clang_.deleteNode(ooNameImport);
			log_->writeError(className_, usingDecl, CppImportLogger::Reason::INSERT_PROBLEM);
		}
	}
	return true;
}

bool ClangAstVisitor::TraverseUsingDirectiveDecl(clang::UsingDirectiveDecl* usingDirectiveDecl)
{
	if (!shouldImport(usingDirectiveDecl->getLocation()))
		return true;

	usingDirectiveDeclarations_.append(usingDirectiveDecl);
	return true;
}

bool ClangAstVisitor::TraverseUnresolvedUsingValueDecl(clang::UnresolvedUsingValueDecl* unresolvedUsing)
{
	if (!shouldImport(unresolvedUsing->getLocation()))
		return true;
	if (auto ooNameImport = trMngr_->insertUnresolvedUsing(unresolvedUsing))
	{
		ooNameImport->modifiers()->set(utils_->translateAccessSpecifier(unresolvedUsing->getAccess()));

		auto nameRef = utils_->setReferencePrefix(clang_.createReference(unresolvedUsing->getNameInfo().getSourceRange()),
																unresolvedUsing->getQualifierLoc());
		ooNameImport->setImportedName(nameRef);
		if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(clang_.createNode<OOModel::DeclarationStatement>(unresolvedUsing->getSourceRange(),
																									ooNameImport));
		else if (auto declaration = DCast<OOModel::Declaration>(ooStack_.top()))
			declaration->subDeclarations()->append(ooNameImport);
		else
		{
			clang_.deleteNode(ooNameImport);
			log_->writeError(className_, unresolvedUsing, CppImportLogger::Reason::INSERT_PROBLEM);
		}
	}
	return true;
}

bool ClangAstVisitor::TraverseStaticAssertDecl(clang::StaticAssertDecl* staticAssertDecl)
{
	if (!shouldImport(staticAssertDecl->getLocation()))
		return true;

	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		auto ooAssert = clang_.createNode<OOModel::AssertStatement>(staticAssertDecl->getSourceRange());
		auto ooAssertExpression = exprVisitor_->translateExpression(staticAssertDecl->getAssertExpr());
		if (staticAssertDecl->getMessage())
		{
			auto commaExpression = clang_.createNode<OOModel::CommaExpression>(staticAssertDecl->getSourceRange());
			commaExpression->setLeft(ooAssertExpression);
			commaExpression->setRight(exprVisitor_->translateExpression(staticAssertDecl->getMessage()));
			ooAssert->setExpression(commaExpression);
		}
		else
			ooAssert->setExpression(ooAssertExpression);
		ooAssert->setAssertKind(OOModel::AssertStatement::AssertKind::Static);
		itemList->append(ooAssert);
	}
	else
	{
		// TODO: it is necessary to add an AssertDeclaration to Envision (right now we have only a statement)
		// in order to support the remaining cases.
		log_->writeError(className_, staticAssertDecl, CppImportLogger::Reason::INSERT_PROBLEM);
	}
	return true;
}

bool ClangAstVisitor::TraverseStmt(clang::Stmt* S)
{
	if (S && llvm::isa<clang::Expr>(S))
	{
		// always ignore implicit stuff
		auto expr = exprVisitor_->translateExpression(S->IgnoreImplicit());
		if (!inBody_)
			ooExprStack_.push(expr);
		else if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(clang_.createNode<OOModel::ExpressionStatement>(S->getSourceRange(), expr));
		return true;
	}

	return Base::TraverseStmt(S);
}

bool ClangAstVisitor::VisitStmt(clang::Stmt* S)
{
	if (S && !llvm::isa<clang::CompoundStmt>(S) && !llvm::isa<clang::NullStmt>(S))
	{
		log_->writeError(className_, S, CppImportLogger::Reason::NOT_SUPPORTED);
		return true;
	}
	return Base::VisitStmt(S);
}

bool ClangAstVisitor::TraverseIfStmt(clang::IfStmt* ifStmt)
{
	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		auto ooIfStmt = clang_.createNode<OOModel::IfStatement>(ifStmt->getSourceRange());
		// append the if stmt to current stmt list
		itemList->append(ooIfStmt);
		// condition
		bool inBody = inBody_;
		inBody_ = false;
		if (auto varDecl = ifStmt->getConditionVariable())
			TraverseDecl(varDecl);
		else
			TraverseStmt(ifStmt->getCond());
		ooIfStmt->setCondition(ooExprStack_.pop());
		inBody_ = true;
		// then branch
		ooStack_.push(ooIfStmt->thenBranch());
		TraverseStmt(ifStmt->getThen());
		ooStack_.pop();
		// else branch
		ooStack_.push(ooIfStmt->elseBranch());
		TraverseStmt(ifStmt->getElse());
		inBody_ = inBody;
		ooStack_.pop();
	}
	else
		log_->writeError(className_, ifStmt, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::TraverseWhileStmt(clang::WhileStmt* whileStmt)
{
	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		auto ooLoop = clang_.createNode<OOModel::LoopStatement>(whileStmt->getSourceRange());
		// append the loop to current stmt list
		itemList->append(ooLoop);
		// condition
		bool inBody = inBody_;
		inBody_ = false;
		if (auto varDecl = whileStmt->getConditionVariable())
			TraverseDecl(varDecl);
		else
			TraverseStmt(whileStmt->getCond());
		inBody_ = true;
		ooLoop->setCondition(ooExprStack_.pop());
		// body
		ooStack_.push(ooLoop->body());
		TraverseStmt(whileStmt->getBody());
		inBody_ = inBody;
		ooStack_.pop();
	}
	else
		log_->writeError(className_, whileStmt, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::TraverseDoStmt(clang::DoStmt* doStmt)
{
	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		auto ooLoop = clang_.createNode<OOModel::LoopStatement>(doStmt->getSourceRange(),
																				  OOModel::LoopStatement::LoopKind::PostCheck);
		// append the loop to current stmt list
		itemList->append(ooLoop);
		// condition
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(doStmt->getCond());
		ooLoop->setCondition(ooExprStack_.pop());
		// body
		inBody_ = true;
		ooStack_.push(ooLoop->body());
		TraverseStmt(doStmt->getBody());
		inBody_ = inBody;
		ooStack_.pop();
	}
	else
		log_->writeError(className_, doStmt, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::TraverseForStmt(clang::ForStmt* forStmt)
{
	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		auto ooLoop = clang_.createNode<OOModel::LoopStatement>(forStmt->getSourceRange());
		itemList->append(ooLoop);
		bool inBody = inBody_;
		inBody_ = false;
		// init
		TraverseStmt(forStmt->getInit());
		if (!ooExprStack_.empty())
		ooLoop->setInitStep(ooExprStack_.pop());
		// condition
		TraverseStmt(forStmt->getCond());
		if (!ooExprStack_.empty())
		ooLoop->setCondition(ooExprStack_.pop());
		// update
		if (forStmt->getInc())
			ooLoop->setUpdateStep(exprVisitor_->translateExpression(forStmt->getInc()));
		inBody_ = true;
		// body
		ooStack_.push(ooLoop->body());
		TraverseStmt(forStmt->getBody());
		inBody_ = inBody;
		ooStack_.pop();
	}
	else
		log_->writeError(className_, forStmt, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::TraverseCXXForRangeStmt(clang::CXXForRangeStmt* forRangeStmt)
{
	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		auto ooLoop = clang_.createNode<OOModel::ForEachStatement>(forRangeStmt->getSourceRange());
		auto loopVar = forRangeStmt->getLoopVariable();
		itemList->append(ooLoop);
		ooLoop->setVarName(QString::fromStdString(loopVar->getNameAsString()));
		ooLoop->setVarType(utils_->translateQualifiedType(loopVar->getTypeSourceInfo()->getTypeLoc()));
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(forRangeStmt->getRangeInit());
		if (!ooExprStack_.empty())
			ooLoop->setCollection(ooExprStack_.pop());
		inBody_ = true;
		//body
		ooStack_.push(ooLoop->body());
		TraverseStmt(forRangeStmt->getBody());
		ooStack_.pop();
		inBody_ = inBody;
	}
	else
		log_->writeError(className_, forRangeStmt, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::TraverseReturnStmt(clang::ReturnStmt* returnStmt)
{
	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		auto ooReturn = clang_.createNode<OOModel::ReturnStatement>(returnStmt->getSourceRange());
		itemList->append(ooReturn);
		// return expression
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(returnStmt->getRetValue());
		if (!ooExprStack_.empty())
			ooReturn->values()->append(ooExprStack_.pop());
		else
			log_->writeError(className_, returnStmt->getRetValue(), CppImportLogger::Reason::NOT_SUPPORTED);
		inBody_ = inBody;
	}
	else
		log_->writeError(className_, returnStmt, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::TraverseDeclStmt(clang::DeclStmt* declStmt)
{
	if (inBody_)
	{
		for (auto declIt = declStmt->decl_begin(); declIt != declStmt->decl_end(); ++declIt)
			TraverseDecl(*declIt);
		return true;
	}
	if (!declStmt->isSingleDecl())
	{
		auto ooComma = clang_.createNode<OOModel::CommaExpression>(declStmt->getSourceRange());
		bool inBody = inBody_;
		inBody_ = false;
		QList<OOModel::Expression*> exprList;
		for (auto declIt = declStmt->decl_begin(); declIt != declStmt->decl_end(); declIt++)
		{
			TraverseDecl(*declIt);
			if (!ooExprStack_.empty())
				exprList.append(ooExprStack_.pop());
			else
				log_->writeError(className_, *declIt, CppImportLogger::Reason::NOT_SUPPORTED);
		}
		int size = exprList.size();
		auto currentCommaExpr = ooComma;
		for (int i = 0; i < size; i++)
		{
			if ((i+2) < size)
			{
				currentCommaExpr->setLeft(exprList.at(i));
				auto next = clang_.createNode<OOModel::CommaExpression>(declStmt->getSourceRange());
				currentCommaExpr->setRight(next);
				currentCommaExpr = next;
			}
			else if ((i+1) < size)
				currentCommaExpr->setLeft(exprList.at(i));
			else
				currentCommaExpr->setRight(exprList.at(i));
		}

		if (!(inBody_ = inBody))
			ooExprStack_.push(ooComma);
		else
			log_->writeError(className_, declStmt, CppImportLogger::Reason::INSERT_PROBLEM);
		return true;
	}
	return TraverseDecl(declStmt->getSingleDecl());
}

bool ClangAstVisitor::TraverseCXXTryStmt(clang::CXXTryStmt* tryStmt)
{
	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		auto ooTry = clang_.createNode<OOModel::TryCatchFinallyStatement>(tryStmt->getSourceRange());
		itemList->append(ooTry);
		bool inBody = inBody_;
		inBody_ = true;
		// visit the body
		ooStack_.push(ooTry->tryBody());
		TraverseStmt(tryStmt->getTryBlock());
		ooStack_.pop();
		// visit catch blocks
		unsigned end = tryStmt->getNumHandlers();
		for (unsigned i = 0; i < end; i++)
		{
			TraverseStmt(tryStmt->getHandler(i));
			ooTry->catchClauses()->append(ooStack_.pop());
		}
		inBody_ = inBody;
	}
	else
		log_->writeError(className_, tryStmt, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::TraverseCXXCatchStmt(clang::CXXCatchStmt* catchStmt)
{
	auto ooCatch = clang_.createNode<OOModel::CatchClause>(catchStmt->getSourceRange());
	// save inBody var
	bool inBody = inBody_;
	inBody_ = false;
	// visit exception to catch
	if (catchStmt->getExceptionDecl())
	{
		TraverseDecl(catchStmt->getExceptionDecl());
		if (!ooExprStack_.empty())
			ooCatch->setExceptionToCatch(ooExprStack_.pop());
	}
	// visit catch body
	inBody_ = true;
	ooStack_.push(ooCatch->body());
	TraverseStmt(catchStmt->getHandlerBlock());
	ooStack_.pop();
	// finish up
	inBody_ = inBody;
	ooStack_.push(ooCatch);
	return true;
}

bool ClangAstVisitor::TraverseSwitchStmt(clang::SwitchStmt* switchStmt)
{
	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		auto ooSwitchStmt = clang_.createNode<OOModel::SwitchStatement>(switchStmt->getSourceRange());
		itemList->append(ooSwitchStmt);
		// save inbody var
		bool inBody = inBody_;
		inBody_ = false;
		// Traverse condition
		if (auto varDecl = switchStmt->getConditionVariable())
			TraverseDecl(varDecl);
		else
			TraverseStmt(switchStmt->getCond());
		if (!ooExprStack_.empty())
			ooSwitchStmt->setSwitchExpression(ooExprStack_.pop());
		// body
		inBody_ = true;
		if (auto body = llvm::dyn_cast<clang::CompoundStmt>(switchStmt->getBody()))
		{
			ooStack_.push(ooSwitchStmt->body());
			for (auto it = body->body_begin(); it != body->body_end(); it++)
				TraverseStmt(*it);
			ooStack_.pop();
		}
		else
			log_->writeError(className_, switchStmt, CppImportLogger::Reason::NOT_SUPPORTED);
		// restore inbody var
		inBody_ = inBody;
	}
	else
		log_->writeError(className_, switchStmt, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::TraverseCaseStmt(clang::CaseStmt* caseStmt)
{
	auto ooSwitchCase = clang_.createNode<OOModel::CaseStatement>(caseStmt->getSourceRange());

	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
		itemList->append(ooSwitchCase);
	else
	{
		log_->writeError(className_, caseStmt, CppImportLogger::Reason::INSERT_PROBLEM);
		return true;
	}

	ooSwitchCase->setCaseExpression(exprVisitor_->translateExpression(caseStmt->getLHS()));

	if (!llvm::dyn_cast<clang::CaseStmt>(caseStmt->getSubStmt()))
	{
		ooStack_.push(ooSwitchCase->body());
		TraverseStmt(caseStmt->getSubStmt());
		ooStack_.pop();
	}
	else
		TraverseStmt(caseStmt->getSubStmt());
	return true;
}

bool ClangAstVisitor::TraverseCompoundStmt(clang::CompoundStmt* compoundStmt)
{
	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		/*
		 * in case this compound statement gets translated to a statement item list we are manually
		 * visiting children in order to be able to insert empty lines and comments.
		 *
		 * comments processing 1 of 3.
		 * process comments which are on separate lines.
		 * we might process some comments which are later reassociated with declarations.
		 */

		// calculate the presumed locations for the beginning and end of this compound statement.
		auto presumedLocationStart = clang_.sourceManager()->getPresumedLoc(compoundStmt->getLocStart());
		auto presumedLocationEnd = clang_.sourceManager()->getPresumedLoc(compoundStmt->getLocEnd());

		// assert clang behaves as expected.
		Q_ASSERT(presumedLocationStart.getFilename() == presumedLocationEnd.getFilename());
		Q_ASSERT(presumedLocationStart.getLine() <= presumedLocationEnd.getLine());

		/*
		 * to increase performance we precompute a subset of the comments of this translation unit containing
		 * only the comments which are in range of this compound statement.
		 */
		QList<Comment*> listComments;
		for (auto comment : clang_.comments())
				if (presumedLocationStart.getFilename() == comment->fileName() &&
					 presumedLocationStart.getLine() <= comment->lineStart() &&
					 comment->lineEnd() <= presumedLocationEnd.getLine())
					listComments.append(comment);
		/*
		 * keep track of the line the last child has ended on.
		 * initially this location is the beginning of the compound statement itself.
		 */
		auto lastChildEndLine = clang_.sourceManager()->getPresumedLineNumber(compoundStmt->getLocStart());
		bool firstLine = true;

		// Traverse children
		for (auto child : compoundStmt->children())
		{
			// calculate the line on which the current child starts
			auto currentChildStartLine = clang_.sourceManager()->getPresumedLineNumber(child->getSourceRange().getBegin());

			// check that we are in a valid case where the end of the last child comes before the start of the current one.
			if (lastChildEndLine < currentChildStartLine)
				// "visit" each line between the two children
				for (auto currentLine = lastChildEndLine; currentLine < currentChildStartLine; currentLine++)
				{
					// try to find a comment that was not yet attached to any node located on the current line
					Comment* commentOnLine = nullptr;
					bool emptyLine = true;
					for (auto comment : listComments)
						if (!comment->node() && comment->lineStart() == currentLine)
						{
							commentOnLine = comment;
							emptyLine = false;
							break;
						}
						else if (comment->lineStart() < currentLine && currentLine <= comment->lineEnd())
							emptyLine = false;

					if (emptyLine && !firstLine)
						// if no comment was found that means that the line is empty
						itemList->append(new OOModel::ExpressionStatement{});
					else if (commentOnLine)
					{
						// insert the found comment at the current line
						commentOnLine->insertIntoItemList(itemList);

						// in case the comment takes up multiple lines we have to skip over the additional lines as well
						currentLine = commentOnLine->lineEnd();
					}

					firstLine = false;
				}

			firstLine = false;
			if (llvm::dyn_cast<clang::CompoundStmt>(child))
			{
				auto block = clang_.createNode<OOModel::Block>(child->getSourceRange());
				itemList->append(block);
				ooStack_.push(block->items());
				TraverseStmt(child);
				ooStack_.pop();
			}
			else
				TraverseStmt(child);

			// update the location on which the last child ended
			lastChildEndLine = clang_.sourceManager()->getPresumedLineNumber(child->getLocEnd()) + 1;
		}

		QList<Comment*> remainingComments;
		for (auto comment : listComments)
			if (!comment->node()) remainingComments.append(comment);
		std::sort(remainingComments.begin(), remainingComments.end(),
				[](Comment* c1, Comment* c2)
		{
			if (c1->lineStart() < c2->lineStart()) return true;
			Q_ASSERT(c1->lineStart() != c2->lineEnd());
			Q_ASSERT(c1->lineEnd() != c2->lineStart());
			return false;
		});
		for (auto comment : remainingComments)
		{
			if (!firstLine)
				for (auto currentLine = lastChildEndLine; currentLine < comment->lineStart(); currentLine++)
					itemList->append(new OOModel::ExpressionStatement{});
			comment->insertIntoItemList(itemList);
			lastChildEndLine = comment->lineEnd() + 1;
			firstLine = false;
		}

		return true;
	}

	return Base::TraverseCompoundStmt(compoundStmt);
}

bool ClangAstVisitor::TraverseDefaultStmt(clang::DefaultStmt* defaultStmt)
{
	auto ooDefaultCase = clang_.createNode<OOModel::CaseStatement>(defaultStmt->getSourceRange());
	// insert in tree
	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
		itemList->append(ooDefaultCase);
	else
	{
		log_->writeError(className_, defaultStmt, CppImportLogger::Reason::INSERT_PROBLEM);
		return true;
	}
	// Traverse statements/body
	ooStack_.push(ooDefaultCase->body());
	TraverseStmt(defaultStmt->getSubStmt());
	ooStack_.pop();
	return true;
}

bool ClangAstVisitor::TraverseBreakStmt(clang::BreakStmt* breakStmt)
{
	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		auto ooBreakStmt = clang_.createNode<OOModel::BreakStatement>(breakStmt->getSourceRange());
		itemList->append(ooBreakStmt);
	}
	else
		log_->writeError(className_, breakStmt, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::TraverseContinueStmt(clang::ContinueStmt* continueStmt)
{
	if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		auto ooContinueStmt = clang_.createNode<OOModel::ContinueStatement>(continueStmt->getSourceRange());
		itemList->append(ooContinueStmt);
	}
	else
		log_->writeError(className_, continueStmt, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::shouldUseDataRecursionfor (clang::Stmt*)
{
	return false;
}

void ClangAstVisitor::addFunctionModifiers(clang::FunctionDecl* functionDecl, OOModel::Method* method)
{
	if (functionDecl->isInlined())
		method->modifiers()->set(OOModel::Modifier::Inline);
	if (functionDecl->isConstexpr())
		method->modifiers()->set(OOModel::Modifier::ConstExpr);
	if (functionDecl->isVirtualAsWritten())
		method->modifiers()->set(OOModel::Modifier::Virtual);
	if (functionDecl->hasAttr<clang::OverrideAttr>())
		method->modifiers()->set(OOModel::Modifier::Override);
	if (functionDecl->hasAttr<clang::FinalAttr>())
		method->modifiers()->set(OOModel::Modifier::Final);
	if (functionDecl->getStorageClass() == clang::SC_Static)
		method->modifiers()->set(OOModel::Modifier::Static);
	if (functionDecl->isDefaulted())
		method->modifiers()->set(OOModel::Modifier::Default);
	if (functionDecl->isDeleted())
		method->modifiers()->set(OOModel::Modifier::Deleted);
	if (functionDecl->isPure())
		method->modifiers()->set(OOModel::Modifier::Abstract);
}

bool ClangAstVisitor::TraverseMethodDecl(clang::CXXMethodDecl* methodDecl, OOModel::Method::MethodKind kind)
{
	auto ooMethod = trMngr_->insertMethodDecl(methodDecl, kind);
	if (!ooMethod)
	{
		// TODO: at the moment we only consider a method where the parent has been visited.
		if (trMngr_->containsClass(methodDecl->getParent()))
			log_->writeError(className_, methodDecl, CppImportLogger::Reason::NO_PARENT);
		return true;
	}

	if (methodDecl->isConst())
		ooMethod->modifiers()->set(OOModel::Modifier::Const);
	addFunctionModifiers(methodDecl, ooMethod);

	if (!ooMethod->items()->size())
	{
		// we only translate the following if the method is not yet defined (therefore the body is empty)
		// note that the following code might get executed twice once for the declaration and once for the definition.

		// translate modifiers
		ooMethod->modifiers()->set(utils_->translateAccessSpecifier(methodDecl->getAccess()));

		// handle body, typeargs and storage specifier
		TraverseFunction(methodDecl, ooMethod);

		// member initializers
		if (auto constructor = llvm::dyn_cast<clang::CXXConstructorDecl>(methodDecl))
		{
			if (constructor->getNumCtorInitializers() && !ooMethod->memberInitializers()->size())
			{
				if (constructor->isExplicit())
					ooMethod->modifiers()->set(OOModel::Modifier::Explicit);

				// if the method already has member initializer we do not have to consider them anymore
				for (auto initIt = constructor->init_begin(); initIt != constructor->init_end(); ++initIt)
				{
					if (!(*initIt)->isWritten())
						continue;
					if (auto ooMemberInit = utils_->translateMemberInit((*initIt)))
						ooMethod->memberInitializers()->append(ooMemberInit);
				}
			}
		}
	}
	return true;
}

void ClangAstVisitor::TraverseClass(clang::CXXRecordDecl* recordDecl, OOModel::Class* ooClass)
{
	Q_ASSERT(ooClass);
	// insert in tree
	if (auto curProject = DCast<OOModel::Project>(ooStack_.top()))
		clang_.insertDeclarationInFolder(ooClass, recordDecl->getLocation(), curProject);
	else if (auto curModule = DCast<OOModel::Module>(ooStack_.top()))
		clang_.insertDeclarationInFolder(ooClass, recordDecl->getLocation(), curModule);
	else if (auto curClass = DCast<OOModel::Class>(ooStack_.top()))
		curClass->classes()->append(ooClass);
	else if (auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
		itemList->append(clang_.createNode<OOModel::DeclarationStatement>(recordDecl->getSourceRange(), ooClass));
	else
		log_->writeError(className_, recordDecl, CppImportLogger::Reason::INSERT_PROBLEM);

	// visit child decls
	if (recordDecl->isThisDeclarationADefinition())
	{
		ooStack_.push(ooClass);
		for (auto declIt = recordDecl->decls_begin(); declIt!=recordDecl->decls_end(); ++declIt)
		{
			if (auto fDecl = llvm::dyn_cast<clang::FriendDecl>(*declIt))
			{
				// Class type
				if (fDecl->getFriendType())
					insertFriendClass(fDecl, ooClass);
				// Functions
				if (fDecl->getFriendDecl())
					insertFriendFunction(fDecl, ooClass);
			}
			else
				TraverseDecl(*declIt);
		}
		ooStack_.pop();

		// visit base classes
		for (auto base_itr = recordDecl->bases_begin(); base_itr!=recordDecl->bases_end(); ++base_itr)
			ooClass->baseClasses()->append(utils_->translateQualifiedType(base_itr->getTypeSourceInfo()->getTypeLoc()));
	}

	// set modifiers
	ooClass->modifiers()->set(utils_->translateAccessSpecifier(recordDecl->getAccess()));
}

void ClangAstVisitor::TraverseFunction(clang::FunctionDecl* functionDecl, OOModel::Method* ooFunction)
{
	Q_ASSERT(ooFunction);
	// only visit the body if we are at the definition
	if (functionDecl->isThisDeclarationADefinition())
	{
		if (ooFunction->items()->size()) return;

		ooStack_.push(ooFunction->items());
		bool inBody = inBody_;
		inBody_ = true;
		if (auto body = functionDecl->getBody())
			TraverseStmt(body);
		inBody_ = inBody;
		ooStack_.pop();
	}
	// visit type arguments if any & if not yet visited
	if (!ooFunction->typeArguments()->size())
	{
		if (auto functionTemplate = functionDecl->getDescribedFunctionTemplate())
		{
			auto templateParamList = functionTemplate->getTemplateParameters();
			for (auto templateParameter : *templateParamList)
				ooFunction->typeArguments()->append(templArgVisitor_->translateTemplateArg(templateParameter));
		}
		if (auto specArgs = functionDecl->getTemplateSpecializationArgsAsWritten())
		{
			for (unsigned i = 0; i < specArgs->NumTemplateArgs; i++)
			{
				auto templateArg = specArgs->getTemplateArgs()[i];
				auto typeArg = clang_.createNode<OOModel::FormalTypeArgument>(templateArg.getSourceRange(),
																								  clang_.spelling(templateArg.getLocation()));
				typeArg->setSpecializationExpression(utils_->translateTemplateArgument(templateArg));
				ooFunction->typeArguments()->append(typeArg);
			}
		}
	}
}

void ClangAstVisitor::insertFriendClass(clang::FriendDecl* friendDecl, OOModel::Class* ooClass)
{
	auto friendClass = friendDecl->getFriendType()->getType().getTypePtr()->getAsCXXRecordDecl();
	Q_ASSERT(friendClass);

	auto friendClassNameSourceRange = friendDecl->getFriendType()->getTypeLoc().getNextTypeLoc().getSourceRange();
	ooClass->friends()->append(clang_.createNode<OOModel::Class>(friendClassNameSourceRange,
																					 clang_.spelling(friendClassNameSourceRange)));
}

void ClangAstVisitor::insertFriendFunction(clang::FriendDecl* friendDecl, OOModel::Class* ooClass)
{
	if (auto friendFunction = llvm::dyn_cast<clang::FunctionDecl>(friendDecl->getFriendDecl()))
	{
		auto ooMethod = clang_.createNode<OOModel::Method>(friendDecl->getSourceRange(),
																			clang_.spelling(friendFunction->getLocation()));
		auto functionTypeLoc = friendFunction->getTypeSourceInfo()->getTypeLoc().castAs<clang::FunctionTypeLoc>();
		ooMethod->results()->append(
					clang_.createNode<OOModel::FormalResult>(functionTypeLoc.getReturnLoc().getSourceRange(), QString{},
																		 utils_->translateQualifiedType(functionTypeLoc.getReturnLoc())));
		// process arguments
		for (auto it = friendFunction->param_begin(); it != friendFunction->param_end(); ++it)
		{
			auto formalArgument = clang_.createNamedNode<OOModel::FormalArgument>(*it,
																utils_->translateQualifiedType((*it)->getTypeSourceInfo()->getTypeLoc()));
			if ((*it)->hasDefaultArg())
				formalArgument->setInitialValue(exprVisitor_->translateExpression((*it)->getInit()));
			ooMethod->arguments()->append(formalArgument);
		}
		ooClass->friends()->append(ooMethod);
	}
}

bool ClangAstVisitor::shouldImport(const clang::SourceLocation& location)
{
	QString fileName;
	if (auto file = clang_.sourceManager()->getPresumedLoc(location).getFilename())
		fileName = QString{file};
	if (clang_.sourceManager()->isInSystemHeader(location) || fileName.isEmpty() || fileName.toLower().contains("qt"))
		return importSysHeader_;
	return true;
}

void ClangAstVisitor::beforeTranslationUnit(clang::ASTContext& astContext)
{
	auto comments = astContext.getRawCommentList().getComments();
	for (auto it = comments.begin(); it != comments.end(); it++)
		clang_.comments().append(new Comment{*it, *clang_.sourceManager()});
}

void ClangAstVisitor::endTranslationUnit()
{
	macroImporter_.endTranslationUnit();

	/*
	 * comments processing 3 of 3.
	 * process comments which are on the same line as statements.
	 */
	for (auto it = clang_.envisionToClangMap().begin(); it != clang_.envisionToClangMap().end(); it++)
	{
		auto nodePresumedLocation = clang_.sourceManager()->getPresumedLoc(it.value().getBegin());

		for (Comment* comment : clang_.comments())
		{
			if (comment->node() ||
				 nodePresumedLocation.getFilename() != comment->fileName() ||
				 nodePresumedLocation.getLine() != comment->lineStart()) continue;

			// calculate the parent of the current node which is a direct child of a statement item list.
			auto lastNodeBeforeList = it.key();
			auto parent = lastNodeBeforeList->parent();
			while (parent)
			{
				if (auto itemList = DCast<OOModel::StatementItemList>(parent))
				{
					// add the comment to the parent of the current node which is a direct child of a statement item list.
					comment->insertIntoItemList(itemList, itemList->indexOf(lastNodeBeforeList));
					break;
				}

				lastNodeBeforeList = parent;
				parent = lastNodeBeforeList->parent();
			}
		}
	}

	insertUsingDirectiveDeclarations();

	trMngr_->removeEmptyNamespaces();

	clang_.envisionToClangMap().clear();
	clang_.comments().clear();
	usingDirectiveDeclarations_.clear();
}

void ClangAstVisitor::insertUsingDirectiveDeclarations()
{
	for (auto usingDirective : usingDirectiveDeclarations_)
	{
		for (auto classs : trMngr_->classesInFile(clang_.presumedFilenameWithoutExtension(usingDirective->getLocation())))
		{
			auto ooNameImport = clang_.createNode<OOModel::NameImport>(usingDirective->getSourceRange());
			auto nameRef = utils_->setReferencePrefix(clang_.createReference(usingDirective->getIdentLocation()),
																	usingDirective->getQualifierLoc());
			ooNameImport->setImportedName(nameRef);
			ooNameImport->setImportAll(true);

			classs->subDeclarations()->append(ooNameImport);
		}
	}
}

void ClangAstVisitor::endEntireImport()
{
	macroImporter_.endEntireImport();

	QList<Model::Node*> workStack{clang_.rootProject()};
	while (!workStack.empty())
	{
		auto current = workStack.takeLast();

		if (auto reference = DCast<OOModel::ReferenceExpression>(current))
		{
			if (auto prefix = DCast<OOModel::ReferenceExpression>(reference->prefix()))
			{
				if (DCast<OOModel::Module>(prefix->target()))
				{
					QSet<Model::Node*> foundSymbols;
					Model::SymbolMatcher matcher{reference->name()};
					reference->parent()->findSymbols(foundSymbols, matcher, reference, Model::Node::SEARCH_UP,
																Model::Node::ANY_SYMBOL, false);

					if ((foundSymbols.size() > 1 && !reference->target()) ||
						 (foundSymbols.size() == 1 && foundSymbols.contains(reference->target())))
						reference->setPrefix(nullptr);
				}
			}
		}
		else if (!DCast<OOModel::MetaCallExpression>(current))
			workStack << current->children();
	}

	clang_.exportMergeMapToJson("export-config.json");
}

} // namespace cppimport
