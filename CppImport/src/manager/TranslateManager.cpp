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

#include "TranslateManager.h"
#include "../visitors/ExpressionVisitor.h"

namespace CppImport {

TranslateManager::TranslateManager(ClangHelpers& clang, OOModel::Project* root, ExpressionVisitor* visitor)
 : clang_{clang}, rootProject_{root}, exprVisitor_{visitor}, nh_{new NodeHasher{clang}}
{}

TranslateManager::~TranslateManager()
{
	SAFE_DELETE(nh_);
}

void TranslateManager::setUtils(CppImportUtilities* utils)
{
	Q_ASSERT(utils);
	utils_ = utils;
}

OOModel::Module* TranslateManager::insertNamespace(clang::NamespaceDecl* namespaceDecl)
{
	const QString hash = nh_->hashNameSpace(namespaceDecl);
	if (nameSpaceMap_.contains(hash))
		return nameSpaceMap_.value(hash);
	auto ooModule = clang_.createNamedNode<OOModel::Module>(namespaceDecl);
	nameSpaceMap_.insert(hash, ooModule);
	if (namespaceDecl->getDeclContext()->isTranslationUnit())
		clang_.projectForLocation(namespaceDecl->getLocation())->modules()->append(ooModule);
	else if (auto p = llvm::dyn_cast<clang::NamespaceDecl>(namespaceDecl->getDeclContext()))
	{
		auto pHash = nh_->hashNameSpace(p);
		if (!nameSpaceMap_.contains(pHash))
			return nullptr;
		nameSpaceMap_.value(pHash)->modules()->append(ooModule);
	}
	else
		return nullptr;
	return ooModule;
}

OOModel::Class* TranslateManager::createClass(clang::CXXRecordDecl* recordDecl)
{
	OOModel::Class::ConstructKind constructKind;
	if (recordDecl->isClass())
		constructKind = OOModel::Class::ConstructKind::Class;
	else if (recordDecl->isStruct())
		constructKind = OOModel::Class::ConstructKind::Struct;
	else if (recordDecl->isUnion())
		constructKind = OOModel::Class::ConstructKind::Union;
	else
		Q_ASSERT(false);

	auto result = clang_.createNamedNode<OOModel::Class>(recordDecl, constructKind);
	if (recordDecl->hasAttr<clang::FinalAttr>())
		result->modifiers()->set(OOModel::Modifier::Final);
	return result;
}

OOModel::Class* TranslateManager::lookupClass(clang::CXXRecordDecl* rDecl)
{
	const QString hash = nh_->hashRecord(rDecl);
	auto it = classMap_.find(hash);
	if (it != classMap_.end())
		return *it;
	return {};
}

bool TranslateManager::insertClass(clang::CXXRecordDecl* rDecl, OOModel::Class*& createdClass)
{
	const QString hash = nh_->hashRecord(rDecl);
	// if rdecl is not managed yet add it:
	if (!classMap_.contains(hash))
	{
		createdClass = createClass(rDecl);
		classMap_.insert(hash, createdClass);
		return true;
	}
	clang_.envisionToClangMap().mapAst(rDecl->getSourceRange(), classMap_.value(hash));
	return false;
}

bool TranslateManager::insertClassTemplate(clang::ClassTemplateDecl* classTemplate, OOModel::Class*& createdClass)
{
	const QString hash = nh_->hashClassTemplate(classTemplate);
	if (!classMap_.contains(hash))
	{
		createdClass = createClass(classTemplate->getTemplatedDecl());
		classMap_.insert(hash, createdClass);
		return true;
	}
	clang_.envisionToClangMap().mapAst(classTemplate->getSourceRange(), classMap_.value(hash));
	return false;
}

bool TranslateManager::insertClassTemplateSpec
(clang::ClassTemplateSpecializationDecl* classTemplate, OOModel::Class*& createdClass)
{
	const QString hash = nh_->hashClassTemplateSpec(classTemplate);
	if (!classMap_.contains(hash))
	{
		createdClass = createClass(classTemplate);
		classMap_.insert(hash, createdClass);
		return true;
	}
	clang_.envisionToClangMap().mapAst(classTemplate->getSourceRange(), classMap_.value(hash));
	return false;
}

OOModel::Method* TranslateManager::insertMethodDecl(clang::CXXMethodDecl* mDecl, OOModel::Method::MethodKind kind)
{
	OOModel::Method* method = nullptr;
	// only consider methods where the parent has already been visited
	if (classMap_.contains(nh_->hashRecord(mDecl->getParent())))
	{
		const QString hash = nh_->hashMethod(mDecl);
		if (!methodMap_.contains(hash))
		{
			method = addNewMethod(mDecl, kind);
		}
		else
		{
			method = methodMap_.value(hash);
			clang_.envisionToClangMap().mapAst(mDecl->getSourceRange(), method);
			clang_.envisionToClangMap().mapAst(mDecl->getLocation(), method->nameNode());

			for (int i = 0; i< method->arguments()->size(); i++)
			{
				auto argName = QString::fromStdString(mDecl->getParamDecl(i)->getNameAsString());
				if (argName.isEmpty()) continue;

				auto ooArg = method->arguments()->at(i);
				if (ooArg->name().isEmpty())
					ooArg->setName(argName);
				else if (argName != ooArg->name())
					Q_ASSERT(false && "multiple different argument names for the same argument");
			}
			return method;
		}
	}
	return method;
}

OOModel::Method* TranslateManager::insertFunctionDecl(clang::FunctionDecl* functionDecl)
{
	OOModel::Method* ooFunction = nullptr;
	const QString hash = nh_->hashFunction(functionDecl);
	if (!functionMap_.contains(hash))
	{
		ooFunction = addNewFunction(functionDecl);
	}
	else
	{
		ooFunction = functionMap_.value(hash);
		clang_.envisionToClangMap().mapAst(functionDecl->getSourceRange(), ooFunction);

		if (ooFunction->items()->size())
			return ooFunction;
		// the method which is in the map is just a declaration
		// therefore it might miss some arguments name which we collect here
		for (int i = 0; i< ooFunction->arguments()->size(); i++)
		{
			OOModel::FormalArgument* ooArg = ooFunction->arguments()->at(i);
			// note that this never should/can be out of range otherwise the hash would be different
			ooArg->setName(QString::fromStdString(functionDecl->getParamDecl(i)->getNameAsString()));
		}
		return ooFunction;
	}
	return ooFunction;
}

OOModel::Field* TranslateManager::insertField(clang::FieldDecl* fieldDecl)
{
	const QString hash = nh_->hashRecord(fieldDecl->getParent());
	if (classMap_.contains(hash))
	{
		auto ooField = clang_.createNamedNode<OOModel::Field>(fieldDecl);
		classMap_.value(hash)->fields()->append(ooField);
		return ooField;
	}
	return nullptr;
}

OOModel::Field* TranslateManager::insertStaticField(clang::VarDecl* varDecl, bool& wasDeclared)
{
	const QString hash = nh_->hashStaticField(varDecl);
	if (staticFieldMap_.contains(hash))
	{
		wasDeclared = true;
		clang_.envisionToClangMap().mapAst(varDecl->getSourceRange(), staticFieldMap_.value(hash));
		return staticFieldMap_.value(hash);
	}
	wasDeclared = false;
	const QString parentHash = nh_->hashParentOfStaticField(varDecl->getDeclContext());
	if (classMap_.contains(parentHash))
	{
		auto ooField = clang_.createNamedNode<OOModel::Field>(varDecl);
		classMap_.value(parentHash)->fields()->append(ooField);
		staticFieldMap_.insert(hash, ooField);
		return ooField;
	}
	return nullptr;
}

OOModel::Field* TranslateManager::insertNamespaceField(clang::VarDecl* varDecl, bool& wasDeclared)
{
	const QString hash = nh_->hashNamespaceField(varDecl);
	if (namespaceFieldMap_.contains(hash))
	{
		wasDeclared = true;
		clang_.envisionToClangMap().mapAst(varDecl->getSourceRange(), namespaceFieldMap_.value(hash));
		return namespaceFieldMap_.value(hash);
	}
	wasDeclared = false;
	const QString parentHash = nh_->hashNameSpace(llvm::dyn_cast<clang::NamespaceDecl>(varDecl->getDeclContext()));
	if (nameSpaceMap_.contains(parentHash))
	{
		auto ooField = clang_.createNamedNode<OOModel::Field>(varDecl);
		nameSpaceMap_.value(parentHash)->fields()->append(ooField);
		namespaceFieldMap_.insert(hash, ooField);
		return ooField;
	}
	return nullptr;
}

OOModel::ExplicitTemplateInstantiation* TranslateManager::insertExplicitTemplateInstantiation
(clang::ClassTemplateSpecializationDecl* explicitTemplateInst)
{
	OOModel::ExplicitTemplateInstantiation* ooExplicitTemplateInst = nullptr;
	const QString hash = nh_->hashClassTemplateSpec(explicitTemplateInst);
	if (!explicitTemplateInstMap_.contains(hash))
	{
		ooExplicitTemplateInst = clang_.createNode<OOModel::ExplicitTemplateInstantiation>(
																										explicitTemplateInst->getSourceRange());
		explicitTemplateInstMap_.insert(hash, ooExplicitTemplateInst);
	}
	clang_.envisionToClangMap().mapAst(explicitTemplateInst->getSourceRange(), explicitTemplateInstMap_.value(hash));
	return ooExplicitTemplateInst;
}

OOModel::NameImport* TranslateManager::insertUsingDecl(clang::UsingDecl* usingDecl)
{
	OOModel::NameImport* ooName = nullptr;
	const QString hash = nh_->hashUsingDecl(usingDecl);
	if (!usingDeclMap_.contains(hash))
	{
		ooName = clang_.createNode<OOModel::NameImport>(usingDecl->getSourceRange());
		usingDeclMap_.insert(hash, ooName);
	}
	return ooName;
}

OOModel::NameImport* TranslateManager::insertUsingDirective(clang::UsingDirectiveDecl* usingDirective)
{
	OOModel::NameImport* ooName = nullptr;
	const QString hash = nh_->hashUsingDirective(usingDirective);
	if (!usingDirectiveMap_.contains(hash))
	{
		ooName = clang_.createNode<OOModel::NameImport>(usingDirective->getSourceRange());
		usingDirectiveMap_.insert(hash, ooName);
	}
	return ooName;
}

OOModel::NameImport* TranslateManager::insertUnresolvedUsing(clang::UnresolvedUsingValueDecl* unresolvedUsing)
{
	OOModel::NameImport* ooName = nullptr;
	const QString hash = nh_->hashUnresolvedUsingDecl(unresolvedUsing);
	if (!usingDeclMap_.contains(hash))
	{
		ooName = clang_.createNode<OOModel::NameImport>(unresolvedUsing->getSourceRange());
		usingDeclMap_.insert(hash, ooName);
	}
	return ooName;
}

OOModel::TypeAlias* TranslateManager::insertNamespaceAlias(clang::NamespaceAliasDecl* namespaceAlias)
{
	OOModel::TypeAlias* ooAlias = nullptr;
	const QString hash = nh_->hashNameSpaceAlias(namespaceAlias);
	if (!namespacAliasMap_.contains(hash))
	{
		ooAlias = clang_.createNamedNode<OOModel::TypeAlias>(namespaceAlias);
		namespacAliasMap_.insert(hash, ooAlias);
	}
	return ooAlias;
}

OOModel::TypeAlias* TranslateManager::insertTypeAlias(clang::TypedefNameDecl* typeAlias)
{
	OOModel::TypeAlias* ooAlias = nullptr;
	const QString hash = nh_->hashTypeAlias(typeAlias);
	if (!typeAliasMap_.contains(hash))
	{
		ooAlias = clang_.createNamedNode<OOModel::TypeAlias>(typeAlias);
		typeAliasMap_.insert(hash, ooAlias);
	}
	return ooAlias;
}

OOModel::TypeAlias* TranslateManager::insertTypeAliasTemplate(clang::TypeAliasTemplateDecl* typeAliasTemplate)
{
	OOModel::TypeAlias* ooAlias = nullptr;
	const QString hash = nh_->hashTypeAliasTemplate(typeAliasTemplate);
	if (!typeAliasMap_.contains(hash))
	{
		ooAlias = clang_.createNamedNode<OOModel::TypeAlias>(typeAliasTemplate);
		typeAliasMap_.insert(hash, ooAlias);
	}
	return ooAlias;
}

void TranslateManager::addMethodResultAndArguments(clang::FunctionDecl* functionDecl,
																					OOModel::Method* method)
{
	// process result type
	if (!llvm::isa<clang::CXXConstructorDecl>(functionDecl) && !llvm::isa<clang::CXXDestructorDecl>(functionDecl))
	{
		auto functionTypeLoc = functionDecl->getTypeSourceInfo()->getTypeLoc().castAs<clang::FunctionTypeLoc>();
		method->results()->append(
					clang_.createNode<OOModel::FormalResult>(functionTypeLoc.getReturnLoc().getSourceRange(), QString{},
																		 utils_->translateQualifiedType(functionTypeLoc.getReturnLoc())));
	}
	// process arguments
	for (auto it = functionDecl->param_begin(); it != functionDecl->param_end(); ++it)
	{
		auto formalArgument = clang_.createNamedNode<OOModel::FormalArgument>(*it,
															utils_->translateQualifiedType((*it)->getTypeSourceInfo()->getTypeLoc()));
		if ((*it)->hasDefaultArg())
			formalArgument->setInitialValue(exprVisitor_->translateExpression((*it)->getInit()));
		method->arguments()->append(formalArgument);
	}
}

OOModel::Method* TranslateManager::addNewMethod(clang::CXXMethodDecl* mDecl, OOModel::Method::MethodKind kind)
{
	auto hash = nh_->hashMethod(mDecl);
	OOModel::Method* method = nullptr;
	if (kind == OOModel::Method::MethodKind::Conversion)
		method = clang_.createNode<OOModel::Method>(mDecl->getSourceRange(), QString{},
																  OOModel::Method::MethodKind::Conversion);
	else if (mDecl->isOverloadedOperator())
		method = clang_.createNode<OOModel::Method>(mDecl->getSourceRange(),
																  utils_->overloadOperatorToString(mDecl->getOverloadedOperator()),
																  OOModel::Method::MethodKind::OperatorOverload);
	else
		method = clang_.createNamedNode<OOModel::Method>(mDecl, kind);
	addMethodResultAndArguments(mDecl, method);

	// find the correct class to add the method
	if (classMap_.contains(nh_->hashRecord(mDecl->getParent())))
	{
		auto parent = classMap_.value(nh_->hashRecord(mDecl->getParent()));
		parent->methods()->append(method);
	}
	else
		std::cout << "ERROR TRANSLATEMNGR: METHOD DECL NO PARENT FOUND" << std::endl;

	methodMap_.insert(hash, method);
	return method;
}

OOModel::Method* TranslateManager::addNewFunction(clang::FunctionDecl* functionDecl)
{
	auto ooFunction = functionDecl->isOverloadedOperator() ?
				clang_.createNode<OOModel::Method>(functionDecl->getSourceRange(),
															  utils_->overloadOperatorToString(functionDecl->getOverloadedOperator()),
															  OOModel::Method::MethodKind::OperatorOverload) :
				clang_.createNamedNode<OOModel::Method>(functionDecl);
	addMethodResultAndArguments(functionDecl, ooFunction);
	functionMap_.insert(nh_->hashFunction(functionDecl), ooFunction);
	return ooFunction;
}

bool TranslateManager::containsClass(clang::CXXRecordDecl* recordDecl)
{
	return classMap_.contains(nh_->hashRecord(recordDecl));
}

}
