/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

namespace CppImport {

ClangAstVisitor::ClangAstVisitor(OOModel::Project* project, CppImportLogger* logger)
:  log_{logger}
{
	trMngr_ = new TranslateManager(project);
	exprVisitor_ = new ExpressionVisitor(this, log_);
	utils_ = new CppImportUtilities(log_, exprVisitor_);
	exprVisitor_->setUtilities(utils_);
	trMngr_->setUtils(utils_);
	templArgVisitor_ = new TemplateArgumentVisitor(exprVisitor_, utils_, log_);
	ooStack_.push(project);
}

ClangAstVisitor::~ClangAstVisitor()
{
	SAFE_DELETE(utils_);
	SAFE_DELETE(exprVisitor_);
	SAFE_DELETE(trMngr_);
}

void ClangAstVisitor::setSourceManager(const clang::SourceManager* sourceManager)
{
	Q_ASSERT(sourceManager);
	sourceManager_ = sourceManager;
	trMngr_->setSourceManager(sourceManager);
}

Model::Node*ClangAstVisitor::ooStackTop()
{
	return ooStack_.top();
}

void ClangAstVisitor::pushOOStack(Model::Node* node)
{
	Q_ASSERT(node);
	ooStack_.push(node);
}

Model::Node*ClangAstVisitor::popOOStack()
{
	return ooStack_.pop();
}

bool ClangAstVisitor::VisitDecl(clang::Decl* decl)
{
	if(!shouldModel(decl->getLocation()))
		return true;

	if(decl && !llvm::isa<clang::AccessSpecDecl>(decl))
	{
		log_->writeError(className_, decl, CppImportLogger::Reason::NOT_SUPPORTED);
		return true;
	}
	return Base::VisitDecl(decl);
}

bool ClangAstVisitor::TraverseNamespaceDecl(clang::NamespaceDecl* namespaceDecl)
{
	if(!shouldModel(namespaceDecl->getLocation()))
		return true;
	OOModel::Module* ooModule = trMngr_->insertNamespace(namespaceDecl);
	if(!ooModule)
	{
		log_->writeError(className_, namespaceDecl, CppImportLogger::Reason::INSERT_PROBLEM);
		// this is a severe error which should not happen therefore stop visiting
		return false;
	}
	// visit the body of the namespace
	ooStack_.push(ooModule);
	clang::DeclContext::decl_iterator it = namespaceDecl->decls_begin();
	for(;it!=namespaceDecl->decls_end();++it)
	{
		TraverseDecl(*it);
	}
	ooStack_.pop();
	return true;
}

bool ClangAstVisitor::TraverseClassTemplateDecl(clang::ClassTemplateDecl* classTemplate)
{
	if(!shouldModel(classTemplate->getLocation()) || !classTemplate->isThisDeclarationADefinition())
		return true;

	clang::CXXRecordDecl* recordDecl = classTemplate->getTemplatedDecl();
	if(auto ooClass = createClass(recordDecl))
	{
		if(!trMngr_->insertClassTemplate(classTemplate, ooClass))
			// this class is already visited
			return true;
		TraverseClass(recordDecl, ooClass);
		// visit type arguments if any
		auto templateParamList = classTemplate->getTemplateParameters();
		for( auto templateParam = templateParamList->begin();
			  templateParam != templateParamList->end(); ++templateParam)
			ooClass->typeArguments()->append(templArgVisitor_->translateTemplateArg(*templateParam));
	}
	return true;
}

bool ClangAstVisitor::TraverseClassTemplateSpecializationDecl
(clang::ClassTemplateSpecializationDecl* specializationDecl)
{
	if(!shouldModel(specializationDecl->getLocation()) || !specializationDecl->isThisDeclarationADefinition())
		return true;

	//	explicit instation declaration
	if(!specializationDecl->isExplicitSpecialization() && specializationDecl->isExplicitInstantiationOrSpecialization())
	{
		auto ooExplicitTemplateInst = trMngr_->insertExplicitTemplateInstantiation(specializationDecl);
		if(!ooExplicitTemplateInst)
			// already inserted
			return true;

		auto ooRef = new OOModel::ReferenceExpression(QString::fromStdString(specializationDecl->getNameAsString()));
		for(unsigned i = 0; i < specializationDecl->getTemplateArgs().size(); i++)
		{
			ooRef->typeArguments()->append(utils_->translateTemplateArgument
													 (specializationDecl->getTemplateArgs().get(i),
													  specializationDecl->getLocStart()));
		}
		if(auto p = llvm::dyn_cast<clang::NamedDecl>(specializationDecl->getSpecializedTemplate()->getDeclContext()))
			ooRef->setPrefix(new OOModel::ReferenceExpression(QString::fromStdString(p->getNameAsString())));
		ooExplicitTemplateInst->setInstantiatedClass(ooRef);
		// add to model
		if(auto decl = DCast<OOModel::Declaration>(ooStack_.top()))
			decl->subDeclarations()->append(ooExplicitTemplateInst);
		else if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(new OOModel::DeclarationStatement(ooExplicitTemplateInst));
		else
			log_->writeError(className_, specializationDecl, CppImportLogger::Reason::INSERT_PROBLEM);
		return true;
	}

	if(auto ooClass = createClass(specializationDecl))
	{
		if(!trMngr_->insertClassTemplateSpec(specializationDecl, ooClass))
			// this class is already visited
			return true;
		TraverseClass(specializationDecl, ooClass);

		auto originalParams = specializationDecl->getSpecializedTemplate()->getTemplateParameters();
		// visit type arguments if any
		for(unsigned i = 0; i < specializationDecl->getTemplateArgs().size(); i++)
		{
			auto typeArg = new OOModel::FormalTypeArgument();
			typeArg->setSpecializationExpression(utils_->translateTemplateArgument
															 (specializationDecl->getTemplateArgs().get(i),
																			  specializationDecl->getLocStart()));
			typeArg->setName(QString::fromStdString(originalParams->getParam(i)->getNameAsString()));
			ooClass->typeArguments()->append(typeArg);
		}
	}
	return true;
}

bool ClangAstVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl* recordDecl)
{
	if(!shouldModel(recordDecl->getLocation()) || !recordDecl->isThisDeclarationADefinition())
		return true;

	if(auto ooClass = createClass(recordDecl))
	{
		if(!trMngr_->insertClass(recordDecl,ooClass))
			// this class is already visited
			return true;
		TraverseClass(recordDecl, ooClass);
		// visit type arguments if any
		if(auto describedTemplate = recordDecl->getDescribedClassTemplate())
		{
			auto templateParamList = describedTemplate->getTemplateParameters();
			for( auto templateParam = templateParamList->begin();
				  templateParam != templateParamList->end(); ++templateParam)
				ooClass->typeArguments()->append(templArgVisitor_->translateTemplateArg(*templateParam));
		}
	}
	else
		log_->writeError(className_, recordDecl, CppImportLogger::Reason::NOT_SUPPORTED);
	return true;
}

bool ClangAstVisitor::TraverseFunctionDecl(clang::FunctionDecl* functionDecl)
{
	if(!shouldModel(functionDecl->getLocation()) || llvm::isa<clang::CXXMethodDecl>(functionDecl))
		return true;

	if(auto ooFunction = trMngr_->insertFunctionDecl(functionDecl))
	{
		if(!ooFunction->parent())
		{
			// insert in model
			if(OOModel::Project* curProject = DCast<OOModel::Project>(ooStack_.top()))
				curProject->methods()->append(ooFunction);
			else if(OOModel::Module* curModel = DCast<OOModel::Module>(ooStack_.top()))
				curModel->methods()->append(ooFunction);
			else if(OOModel::Class* curClass = DCast<OOModel::Class>(ooStack_.top()))
			{
				// happens in the case of friend functions
				log_->writeError(className_, functionDecl, CppImportLogger::Reason::NOT_SUPPORTED);
				curClass->methods()->append(ooFunction);
			}
			else
				log_->writeError(className_, functionDecl, CppImportLogger::Reason::INSERT_PROBLEM);
		}
		if(!ooFunction->items()->size())
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
	return TraverseFunctionDecl(functionDecl->getTemplatedDecl());
}

bool ClangAstVisitor::TraverseVarDecl(clang::VarDecl* varDecl)
{
	if(!shouldModel(varDecl->getLocation()))
		return true;

	if(llvm::isa<clang::ParmVarDecl>(varDecl))
		return true;

	// check if this variable is only from a explicit/implicit template instantiation - if so we do not translate it.
	if(clang::TSK_ExplicitInstantiationDeclaration == varDecl->getTemplateSpecializationKind() ||
			clang::TSK_ExplicitInstantiationDefinition == varDecl->getTemplateSpecializationKind() ||
			clang::TSK_ImplicitInstantiation == varDecl->getTemplateSpecializationKind())
		return true;

	OOModel::VariableDeclaration* ooVarDecl = nullptr;
	OOModel::VariableDeclarationExpression* ooVarDeclExpr = nullptr;
	QString varName = QString::fromStdString(varDecl->getNameAsString());

	bool wasDeclared = false;
	if(varDecl->isStaticDataMember())
	{
		if(!(ooVarDecl = trMngr_->insertStaticField(varDecl, wasDeclared)))
		{
			log_->writeError(className_, varDecl, CppImportLogger::Reason::NO_PARENT);
			return true;
		}
	}
	else
	{
		if(!inBody_)
		{
			ooVarDecl = new OOModel::VariableDeclaration(varName);
			ooVarDeclExpr = new OOModel::VariableDeclarationExpression(ooVarDecl);
			ooExprStack_.push(ooVarDeclExpr);
		}
		else if(auto project = DCast<OOModel::Project>(ooStack_.top()))
		{
			ooVarDecl = new OOModel::Field(varName);
			project->fields()->append(ooVarDecl);
		}
		else if(auto module = DCast<OOModel::Module>(ooStack_.top()))
		{
			ooVarDecl = new OOModel::Field(varName);
			module->fields()->append(ooVarDecl);
		}
		else if(auto ooClass = DCast<OOModel::Class>(ooStack_.top()))
		{
			ooVarDecl = new OOModel::Field(varName);
			ooClass->fields()->append(ooVarDecl);
		}
		else if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
		{
			ooVarDecl = new OOModel::VariableDeclaration(varName);
			// TODO: remove variabledeclaration expression as soon we have a DeclarationStatement
			itemList->append(new OOModel::ExpressionStatement(new OOModel::VariableDeclarationExpression(ooVarDecl)));
		}
		else
		{
			log_->writeWarning(className_, varDecl, CppImportLogger::Reason::INSERT_PROBLEM);
			return true;
		}
	}

	if(varDecl->hasInit())
	{
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(varDecl->getInit()->IgnoreImplicit());
		if(!ooExprStack_.empty())
		{
			// make sure we have not ourself as init (if init couldn't be converted)
			if(ooVarDeclExpr != ooExprStack_.top())
				ooVarDecl->setInitialValue(ooExprStack_.pop());
		}
		else
			log_->writeError(className_, varDecl->getInit(), CppImportLogger::Reason::NOT_SUPPORTED);
		inBody_ = inBody;
	}
	if(wasDeclared)
		// we know the rest of the information already
		return true;

	// set the type
	ooVarDecl->setTypeExpression(utils_->translateQualifiedType(varDecl->getType(), varDecl->getLocStart()));
	// modifiers
	ooVarDecl->modifiers()->set(utils_->translateStorageSpecifier(varDecl->getStorageClass()));
	return true;
}

bool ClangAstVisitor::TraverseFieldDecl(clang::FieldDecl* fieldDecl)
{
	if(!shouldModel(fieldDecl->getLocation()))
		return true;
	OOModel::Field* field = trMngr_->insertField(fieldDecl);
	if(!field)
	{
		log_->writeError(className_, fieldDecl, CppImportLogger::Reason::NO_PARENT);
		return true;
	}
	if(fieldDecl->hasInClassInitializer())
	{
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(fieldDecl->getInClassInitializer());
		if(!ooExprStack_.empty())
			field->setInitialValue(ooExprStack_.pop());
		else
			log_->writeError(className_, fieldDecl->getInClassInitializer(), CppImportLogger::Reason::NOT_SUPPORTED);
		inBody_ = inBody;
	}

	field->setTypeExpression(utils_->translateQualifiedType(fieldDecl->getType(), fieldDecl->getLocStart()));
	// modifiers
	field->modifiers()->set(utils_->translateAccessSpecifier(fieldDecl->getAccess()));
	return true;
}

bool ClangAstVisitor::TraverseEnumDecl(clang::EnumDecl* enumDecl)
{
	if(!shouldModel(enumDecl->getLocation()))
		return true;

	OOModel::Class* ooEnumClass = new OOModel::Class
			(QString::fromStdString(enumDecl->getNameAsString()), OOModel::Class::ConstructKind::Enum);
	// insert in model
	if(OOModel::Project* curProject = DCast<OOModel::Project>(ooStack_.top()))
		curProject->classes()->append(ooEnumClass);
	else if(OOModel::Module* curModel = DCast<OOModel::Module>(ooStack_.top()))
		curModel->classes()->append(ooEnumClass);
	else if(OOModel::Class* curClass = DCast<OOModel::Class>(ooStack_.top()))
		curClass->classes()->append(ooEnumClass);
	else if(OOModel::StatementItemList* itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
		itemList->append(new OOModel::DeclarationStatement(ooEnumClass));
	else
	{
		log_->writeWarning(className_, enumDecl, CppImportLogger::Reason::INSERT_PROBLEM);
		// no need to further process this enum
		return true;
	}
	clang::EnumDecl::enumerator_iterator it = enumDecl->enumerator_begin();
	bool inBody = inBody_;
	inBody_ = false;
	for(;it!=enumDecl->enumerator_end();++it)
	{
		// check if there is an initializing expression if so visit it first and then add it to the enum
		if(auto e = it->getInitExpr())
		{
			TraverseStmt(e);
			ooEnumClass->enumerators()->append(new OOModel::Enumerator
														  (QString::fromStdString(it->getNameAsString()),ooExprStack_.pop()));
		}
		else
			ooEnumClass->enumerators()->append(new OOModel::Enumerator(QString::fromStdString(it->getNameAsString())));
	}
	inBody_ = inBody;
	return true;
}

bool ClangAstVisitor::WalkUpFromTypedefNameDecl(clang::TypedefNameDecl* typedefDecl)
{
	// This method is a walkup such that it covers both subtypes (typedef and typealias)
	if(!shouldModel(typedefDecl->getLocation()))
		return true;
	if(auto ooTypeAlias = trMngr_->insertTypeAlias(typedefDecl))
	{
		ooTypeAlias->setTypeExpression(utils_->translateQualifiedType(typedefDecl->getUnderlyingType(),
																						  typedefDecl->getLocStart()));
		ooTypeAlias->setName(QString::fromStdString(typedefDecl->getNameAsString()));
		if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(new OOModel::DeclarationStatement(ooTypeAlias));
		else if(auto declaration = DCast<OOModel::Declaration>(ooStack_.top()))
			declaration->subDeclarations()->append(ooTypeAlias);
		else
			log_->writeError(className_, typedefDecl, CppImportLogger::Reason::INSERT_PROBLEM);
	}
	return true;
}

bool ClangAstVisitor::TraverseTypeAliasTemplateDecl(clang::TypeAliasTemplateDecl* typeAliasTemplate)
{
	if(!shouldModel(typeAliasTemplate->getLocation()))
		return true;
	if(auto ooTypeAlias = trMngr_->insertTypeAliasTemplate(typeAliasTemplate))
	{
		auto typeAlias = typeAliasTemplate->getTemplatedDecl();
		ooTypeAlias->setTypeExpression(utils_->translateQualifiedType(typeAlias->getUnderlyingType(),
																						  typeAlias->getLocStart()));
		ooTypeAlias->setName(QString::fromStdString(typeAliasTemplate->getNameAsString()));
		// type arguments
		auto templateParamList = typeAliasTemplate->getTemplateParameters();
		for( auto templateParam = templateParamList->begin();
			  templateParam != templateParamList->end(); ++templateParam)
			ooTypeAlias->typeArguments()->append(templArgVisitor_->translateTemplateArg(*templateParam));
		// insert in model
		if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(new OOModel::DeclarationStatement(ooTypeAlias));
		else if(auto declaration = DCast<OOModel::Declaration>(ooStack_.top()))
			declaration->subDeclarations()->append(ooTypeAlias);
		else
			log_->writeError(className_, typeAliasTemplate, CppImportLogger::Reason::INSERT_PROBLEM);
	}
	return true;
}

bool ClangAstVisitor::TraverseNamespaceAliasDecl(clang::NamespaceAliasDecl* namespaceAlias)
{
	if(!shouldModel(namespaceAlias->getLocation()))
		return true;
	if(auto ooTypeAlias = trMngr_->insertNamespaceAlias(namespaceAlias))
	{
		ooTypeAlias->setName(QString::fromStdString(namespaceAlias->getNameAsString()));
		OOModel::ReferenceExpression* nameRef = new OOModel::ReferenceExpression
				(QString::fromStdString(namespaceAlias->getAliasedNamespace()->getNameAsString()));
		if(auto prefix = namespaceAlias->getQualifier())
			nameRef->setPrefix(utils_->translateNestedNameSpecifier(prefix, namespaceAlias->getLocation()));
		ooTypeAlias->setTypeExpression(nameRef);
		if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(new OOModel::DeclarationStatement(ooTypeAlias));
		else if(auto declaration = DCast<OOModel::Declaration>(ooStack_.top()))
			declaration->subDeclarations()->append(ooTypeAlias);
		else
		{
			SAFE_DELETE(ooTypeAlias);
			log_->writeError(className_, namespaceAlias, CppImportLogger::Reason::INSERT_PROBLEM);
		}
	}
	return true;
}

bool ClangAstVisitor::TraverseUsingDecl(clang::UsingDecl* usingDecl)
{
	if(!shouldModel(usingDecl->getLocation()))
		return true;
	if(auto ooNameImport = trMngr_->insertUsingDecl(usingDecl))
	{
		OOModel::ReferenceExpression* nameRef = new OOModel::ReferenceExpression
				(QString::fromStdString(usingDecl->getNameAsString()));
		if(auto prefix = usingDecl->getQualifier())
			nameRef->setPrefix(utils_->translateNestedNameSpecifier(prefix, usingDecl->getLocStart()));
		ooNameImport->setImportedName(nameRef);
		if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(new OOModel::DeclarationStatement(ooNameImport));
		else if(auto declaration = DCast<OOModel::Declaration>(ooStack_.top()))
			declaration->subDeclarations()->append(ooNameImport);
		else
		{
			SAFE_DELETE(ooNameImport);
			log_->writeError(className_, usingDecl, CppImportLogger::Reason::INSERT_PROBLEM);
		}
	}
	return true;
}

bool ClangAstVisitor::TraverseUsingDirectiveDecl(clang::UsingDirectiveDecl* usingDirectiveDecl)
{
	if(!shouldModel(usingDirectiveDecl->getLocation()))
		return true;
	if(auto ooNameImport = trMngr_->insertUsingDirective(usingDirectiveDecl))
	{
		OOModel::ReferenceExpression* nameRef = new OOModel::ReferenceExpression
				(QString::fromStdString(usingDirectiveDecl->getNominatedNamespaceAsWritten()->getNameAsString()));
		if(auto prefix = usingDirectiveDecl->getQualifier())
			nameRef->setPrefix(utils_->translateNestedNameSpecifier(prefix, usingDirectiveDecl->getLocStart()));
		ooNameImport->setImportedName(nameRef);
		if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(new OOModel::DeclarationStatement(ooNameImport));
		else if(auto declaration = DCast<OOModel::Declaration>(ooStack_.top()))
			declaration->subDeclarations()->append(ooNameImport);
		else
		{
			SAFE_DELETE(ooNameImport);
			log_->writeError(className_, usingDirectiveDecl, CppImportLogger::Reason::INSERT_PROBLEM);
		}
	}
	return true;
}

bool ClangAstVisitor::TraverseUnresolvedUsingValueDecl(clang::UnresolvedUsingValueDecl* unresolvedUsing)
{
	if(!shouldModel(unresolvedUsing->getLocation()))
		return true;
	if(auto ooNameImport = trMngr_->insertUnresolvedUsing(unresolvedUsing))
	{
		OOModel::ReferenceExpression* nameRef = new OOModel::ReferenceExpression
				(QString::fromStdString(unresolvedUsing->getNameInfo().getAsString()));
		if(auto prefix = unresolvedUsing->getQualifier())
			nameRef->setPrefix(utils_->translateNestedNameSpecifier(prefix, unresolvedUsing->getLocStart()));
		ooNameImport->setImportedName(nameRef);
		if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(new OOModel::DeclarationStatement(ooNameImport));
		else if(auto declaration = DCast<OOModel::Declaration>(ooStack_.top()))
			declaration->subDeclarations()->append(ooNameImport);
		else
		{
			SAFE_DELETE(ooNameImport);
			log_->writeError(className_, unresolvedUsing, CppImportLogger::Reason::INSERT_PROBLEM);
		}
	}
	return true;
}

bool ClangAstVisitor::TraverseStmt(clang::Stmt* S)
{
	if(S && llvm::isa<clang::Expr>(S))
	{
		// always ignore implicit stuff
		auto expr = exprVisitor_->translateExpression(S->IgnoreImplicit());

		if(!inBody_)
			ooExprStack_.push(expr);
		else if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
			itemList->append(new OOModel::ExpressionStatement(expr));

		return true;
	}

	return Base::TraverseStmt(S);
}

bool ClangAstVisitor::VisitStmt(clang::Stmt* S)
{
	if(S && !llvm::isa<clang::CompoundStmt>(S) && !llvm::isa<clang::NullStmt>(S))
	{
		log_->writeError(className_, S, CppImportLogger::Reason::NOT_SUPPORTED);
		return true;
	}
	return Base::VisitStmt(S);
}

bool ClangAstVisitor::TraverseIfStmt(clang::IfStmt* ifStmt)
{
	if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		OOModel::IfStatement* ooIfStmt = new OOModel::IfStatement();
		// append the if stmt to current stmt list
		itemList->append(ooIfStmt);
		// condition
		bool inBody = inBody_;
		inBody_ = false;
		if(auto varDecl = ifStmt->getConditionVariable())
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
	if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		OOModel::LoopStatement* ooLoop = new OOModel::LoopStatement();
		// append the loop to current stmt list
		itemList->append(ooLoop);
		// condition
		bool inBody = inBody_;
		inBody_ = false;
		if(auto varDecl = whileStmt->getConditionVariable())
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
	if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		OOModel::LoopStatement* ooLoop = new OOModel::LoopStatement(OOModel::LoopStatement::LoopKind::PostCheck);
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
	if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		OOModel::LoopStatement* ooLoop = new OOModel::LoopStatement();
		itemList->append(ooLoop);
		bool inBody = inBody_;
		inBody_ = false;
		// init
		TraverseStmt(forStmt->getInit());
		if(!ooExprStack_.empty())
		ooLoop->setInitStep(ooExprStack_.pop());
		// condition
		TraverseStmt(forStmt->getCond());
		if(!ooExprStack_.empty())
		ooLoop->setCondition(ooExprStack_.pop());
		// update
		TraverseStmt(forStmt->getInc());
		if(!ooExprStack_.empty())
		ooLoop->setUpdateStep(ooExprStack_.pop());
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
	if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		OOModel::ForEachStatement* ooLoop = new OOModel::ForEachStatement();
		const clang::VarDecl* loopVar = forRangeStmt->getLoopVariable();
		itemList->append(ooLoop);
		ooLoop->setVarName(QString::fromStdString(loopVar->getNameAsString()));
		ooLoop->setVarType(utils_->translateQualifiedType(loopVar->getType(), loopVar->getLocStart()));
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(forRangeStmt->getRangeInit());
		if(!ooExprStack_.empty())
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
	if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		OOModel::ReturnStatement* ooReturn = new OOModel::ReturnStatement();
		itemList->append(ooReturn);
		// return expression
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(returnStmt->getRetValue());
		if(!ooExprStack_.empty())
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
	if(inBody_)
	{
		for(auto declIt = declStmt->decl_begin(); declIt != declStmt->decl_end(); ++declIt)
			TraverseDecl(*declIt);
		return true;
	}
	if(!declStmt->isSingleDecl())
	{
		OOModel::CommaExpression* ooComma = new OOModel::CommaExpression();
		bool inBody = inBody_;
		inBody_ = false;
		QList<OOModel::Expression*> exprList;
		for(auto declIt = declStmt->decl_begin(); declIt != declStmt->decl_end(); declIt++)
		{
			TraverseDecl(*declIt);
			if(!ooExprStack_.empty())
				exprList.append(ooExprStack_.pop());
			else
				log_->writeError(className_, *declIt, CppImportLogger::Reason::NOT_SUPPORTED);
		}
		int size = exprList.size();
		auto currentCommaExpr = ooComma;
		for(int i = 0; i < size; i++)
		{
			if((i+2) < size)
			{
				currentCommaExpr->setLeft(exprList.at(i));
				auto next = new OOModel::CommaExpression();
				currentCommaExpr->setRight(next);
				currentCommaExpr = next;
			}
			else if((i+1) < size)
				currentCommaExpr->setLeft(exprList.at(i));
			else
				currentCommaExpr->setRight(exprList.at(i));
		}

		if(!(inBody_ = inBody))
			ooExprStack_.push(ooComma);
		else
			log_->writeError(className_, declStmt, CppImportLogger::Reason::INSERT_PROBLEM);
		return true;
	}
	return TraverseDecl(declStmt->getSingleDecl());
}

bool ClangAstVisitor::TraverseCXXTryStmt(clang::CXXTryStmt* tryStmt)
{
	if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		OOModel::TryCatchFinallyStatement* ooTry = new OOModel::TryCatchFinallyStatement();
		itemList->append(ooTry);
		bool inBody = inBody_;
		inBody_ = true;
		// visit the body
		ooStack_.push(ooTry->tryBody());
		TraverseStmt(tryStmt->getTryBlock());
		ooStack_.pop();
		// visit catch blocks
		unsigned end = tryStmt->getNumHandlers();
		for(unsigned i = 0; i < end; i++)
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
	OOModel::CatchClause* ooCatch = new OOModel::CatchClause();
	// save inBody var
	bool inBody = inBody_;
	inBody_ = false;
	// visit exception to catch
	if(catchStmt->getExceptionDecl())
	{
		TraverseDecl(catchStmt->getExceptionDecl());
		if(!ooExprStack_.empty())
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
	if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
	{
		OOModel::SwitchStatement* ooSwitchStmt = new OOModel::SwitchStatement();
		itemList->append(ooSwitchStmt);
		// save inbody var
		bool inBody = inBody_;
		inBody_ = false;
		// traverse condition
		if(auto varDecl = switchStmt->getConditionVariable())
			TraverseDecl(varDecl);
		else
			TraverseStmt(switchStmt->getCond());
		if(!ooExprStack_.empty())
			ooSwitchStmt->setSwitchExpression(ooExprStack_.pop());
		// body
		inBody_ = true;
		if(auto body = llvm::dyn_cast<clang::CompoundStmt>(switchStmt->getBody()))
		{
			ooStack_.push(ooSwitchStmt->body());
			// Visit everything before the first case/default statement
			auto bodyIt = body->body_begin();
			while(bodyIt != body->body_end() && !llvm::isa<clang::CaseStmt>(*bodyIt) &&
					!llvm::isa<clang::DefaultStmt>(*bodyIt))
				TraverseStmt(*bodyIt++);

			// push a dummy itemlist such that at every case/default statement we can first pop the stack
			auto itemList = new OOModel::StatementItemList();
			ooStack_.push(itemList);
			// visit the rest
			while(bodyIt != body->body_end())
				TraverseStmt(*bodyIt++);

			// pops the body from the last case statement
			ooStack_.pop();
			// delete the dummy list
			SAFE_DELETE(itemList);
			// pop the body of the switch statement
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
	// pop the body of the previous case
	ooStack_.pop();
	OOModel::CaseStatement* ooSwitchCase = new OOModel::CaseStatement();
	// insert in model
	if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
		itemList->append(ooSwitchCase);
	else
	{
		log_->writeError(className_, caseStmt, CppImportLogger::Reason::INSERT_PROBLEM);
		return true;
	}
	// traverse condition
	inBody_ = false;
	TraverseStmt(caseStmt->getLHS());
	if(!ooExprStack_.empty())
		ooSwitchCase->setCaseExpression(ooExprStack_.pop());
	inBody_ = true;
	// traverse statements/body
	ooStack_.push(ooSwitchCase->body());
	TraverseStmt(caseStmt->getSubStmt());
	return true;
}

bool ClangAstVisitor::TraverseDefaultStmt(clang::DefaultStmt* defaultStmt)
{
	// pop the body of the previous case
	ooStack_.pop();
	OOModel::CaseStatement* ooDefaultCase = new OOModel::CaseStatement();
	// insert in model
	if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
		itemList->append(ooDefaultCase);
	else
	{
		log_->writeError(className_, defaultStmt, CppImportLogger::Reason::INSERT_PROBLEM);
		return true;
	}
	// traverse statements/body
	ooStack_.push(ooDefaultCase->body());
	TraverseStmt(defaultStmt->getSubStmt());
	return true;
}

bool ClangAstVisitor::TraverseBreakStmt(clang::BreakStmt* breakStmt)
{
	if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
		itemList->append(new OOModel::BreakStatement());
	else
		log_->writeError(className_, breakStmt, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::TraverseContinueStmt(clang::ContinueStmt* continueStmt)
{
	if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
		itemList->append(new OOModel::ContinueStatement());
	else
		log_->writeError(className_, continueStmt, CppImportLogger::Reason::INSERT_PROBLEM);
	return true;
}

bool ClangAstVisitor::shouldUseDataRecursionFor(clang::Stmt*)
{
	return false;
}

bool ClangAstVisitor::TraverseMethodDecl(clang::CXXMethodDecl* methodDecl, OOModel::Method::MethodKind kind)
{
	OOModel::Method* ooMethod = trMngr_->insertMethodDecl(methodDecl, kind);
	if(!ooMethod)
	{
		// TODO: at the moment we only consider a method where the parent has been visited.
		if(trMngr_->containsClass(methodDecl->getParent()))
			log_->writeError(className_, methodDecl, CppImportLogger::Reason::NO_PARENT);
		return true;
	}
	if(!ooMethod->items()->size())
	{
		// we only translate the following if the method is not yet defined (therefore the body is empty)
		// note that the following code might get executed twice once for the declaration and once for the definition.

		// translate modifiers
		ooMethod->modifiers()->set(utils_->translateAccessSpecifier(methodDecl->getAccess()));

		// handle body, typeargs and storage specifier
		TraverseFunction(methodDecl, ooMethod);

		// member initializers
		if(auto constructor = llvm::dyn_cast<clang::CXXConstructorDecl>(methodDecl))
		{
			if(constructor->getNumCtorInitializers() && !ooMethod->memberInitializers()->size())
			{
				// if the method already has member initializer we do not have to consider them anymore
				for(auto initIt = constructor->init_begin(); initIt != constructor->init_end(); ++initIt)
				{
					if(!(*initIt)->isWritten())
						continue;
					if(auto ooMemberInit = utils_->translateMemberInit((*initIt)))
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
	// insert in model
	if(auto curProject = DCast<OOModel::Project>(ooStack_.top()))
		curProject->classes()->append(ooClass);
	else if(auto curModel = DCast<OOModel::Module>(ooStack_.top()))
		curModel->classes()->append(ooClass);
	else if(auto curClass = DCast<OOModel::Class>(ooStack_.top()))
		curClass->classes()->append(ooClass);
	else if(auto itemList = DCast<OOModel::StatementItemList>(ooStack_.top()))
		itemList->append(new OOModel::DeclarationStatement(ooClass));
	else
		log_->writeError(className_, recordDecl, CppImportLogger::Reason::INSERT_PROBLEM);

	// visit child decls
	if(recordDecl->isThisDeclarationADefinition())
	{
		ooStack_.push(ooClass);
		for(auto declIt = recordDecl->decls_begin(); declIt!=recordDecl->decls_end(); ++declIt)
		{
			if(auto fDecl = llvm::dyn_cast<clang::FriendDecl>(*declIt))
			{
				// Class type
				if(auto type = fDecl->getFriendType())
					insertFriendClass(type, ooClass);
				// Functions
				if (auto friendDecl = fDecl->getFriendDecl())
				{
					if(!llvm::isa<clang::FunctionDecl>(friendDecl))
						log_->writeError(className_, friendDecl, CppImportLogger::Reason::NOT_SUPPORTED);
					else
						insertFriendFunction(llvm::dyn_cast<clang::FunctionDecl>(friendDecl), ooClass);
				}
			}
			else
				TraverseDecl(*declIt);
		}
		ooStack_.pop();

		// visit base classes
		for(auto base_itr = recordDecl->bases_begin(); base_itr!=recordDecl->bases_end(); ++base_itr)
			ooClass->baseClasses()->append(utils_->translateQualifiedType(base_itr->getType(),
																							  recordDecl->getLocStart()));
	}

	// set modifiers
	ooClass->modifiers()->set(utils_->translateAccessSpecifier(recordDecl->getAccess()));
}

void ClangAstVisitor::TraverseFunction(clang::FunctionDecl* functionDecl, OOModel::Method* ooFunction)
{
	Q_ASSERT(ooFunction);
	// only visit the body if we are at the definition
	if(functionDecl->isThisDeclarationADefinition())
	{
		if(ooFunction->items()->size())
			/* TODO: this is a double defined function this comes from functions defined in the header.
			* We might need to give this some attention as soon as we support macros
			* (could be that we include the header with different defines) but for now we just ignore it. */
			return;
		ooStack_.push(ooFunction->items());
		bool inBody = inBody_;
		inBody_ = true;
		if(auto body = functionDecl->getBody())
			TraverseStmt(body);
		inBody_ = inBody;
		ooStack_.pop();
	}
	// visit type arguments if any & if not yet visited
	if(!ooFunction->typeArguments()->size())
	{
		if(auto functionTemplate = functionDecl->getDescribedFunctionTemplate())
		{
			auto templateParamList = functionTemplate->getTemplateParameters();
			for( auto templateParam = templateParamList->begin();
				  templateParam != templateParamList->end(); ++templateParam)
				ooFunction->typeArguments()->append(templArgVisitor_->translateTemplateArg(*templateParam));
		}
		if(auto specArgs = functionDecl->getTemplateSpecializationArgsAsWritten())
		{
			unsigned templateArgs = specArgs->NumTemplateArgs;
			auto astTemplateArgsList = specArgs->getTemplateArgs();
			auto templateParamList = functionDecl->getPrimaryTemplate()->getTemplateParameters();
			for(unsigned i = 0; i < templateArgs; i++)
			{
				auto typeArg = new OOModel::FormalTypeArgument();
				typeArg->setName(QString::fromStdString(templateParamList->getParam(i)->getNameAsString()));
				typeArg->setSpecializationExpression(utils_->translateTemplateArgument
																 (astTemplateArgsList[i].getArgument(),
																  astTemplateArgsList[i].getLocation()));
				ooFunction->typeArguments()->append(typeArg);
			}
		}
	}
	// modifiers
	ooFunction->modifiers()->set(utils_->translateStorageSpecifier(functionDecl->getStorageClass()));
	if(functionDecl->isInlineSpecified())
		ooFunction->modifiers()->set(OOModel::Modifier::Inline);
	if(functionDecl->isVirtualAsWritten())
		ooFunction->modifiers()->set(OOModel::Modifier::Virtual);
}

OOModel::Class*ClangAstVisitor::createClass(clang::CXXRecordDecl* recordDecl)
{
	QString recordDeclName = QString::fromStdString(recordDecl->getNameAsString());
	if(recordDecl->isClass())
		return new OOModel::Class(recordDeclName,OOModel::Class::ConstructKind::Class);
	else if(recordDecl->isStruct())
		return new OOModel::Class(recordDeclName,OOModel::Class::ConstructKind::Struct);
	else if(recordDecl->isUnion())
		return new OOModel::Class(recordDeclName,OOModel::Class::ConstructKind::Union);
	log_->writeError(className_, recordDecl, CppImportLogger::Reason::NOT_SUPPORTED);
	return nullptr;
}

void ClangAstVisitor::insertFriendClass(clang::TypeSourceInfo* typeInfo, OOModel::Class* ooClass)
{
	if(clang::CXXRecordDecl* recordDecl = typeInfo->getType().getTypePtr()->getAsCXXRecordDecl())
		ooClass->friends()->append(new OOModel::ReferenceExpression
											(QString::fromStdString(recordDecl->getNameAsString())));
}

void ClangAstVisitor::insertFriendFunction(clang::FunctionDecl* friendFunction, OOModel::Class* ooClass)
{
	if(friendFunction->isThisDeclarationADefinition())
	{
		// TODO: this is at the moment the only solution to handle this
		ooStack_.push(ooClass);
		TraverseDecl(friendFunction);
		ooStack_.pop();
	}
	// this should happen anyway that it is clearly visible that there is a friend
	// TODO: this is not really a method call but rather a reference
	OOModel::MethodCallExpression* ooMCall = new OOModel::MethodCallExpression(
				QString::fromStdString(friendFunction->getNameAsString()));
	// TODO: handle return type & arguments & type arguments
	ooClass->friends()->append(ooMCall);
}

bool ClangAstVisitor::shouldModel(const clang::SourceLocation& location)
{
	QString fileName;
	if(auto file = sourceManager_->getPresumedLoc(location).getFilename())
		fileName = QString(file);
	if(sourceManager_->isInSystemHeader(location) || fileName.isEmpty() || fileName.contains("qt"))
		return modelSysHeader_;
	return true;
}

} // namespace cppimport
