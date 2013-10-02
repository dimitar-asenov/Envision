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

#include "TranslateManager.h"

namespace CppImport {

TranslateManager::TranslateManager(OOModel::Project* root)
: rootProject_{root}
{}

TranslateManager::~TranslateManager()
{
	SAFE_DELETE(nh_);
}

void TranslateManager::setSourceManager(const clang::SourceManager* mngr)
{
	nh_->setSourceManager(mngr);
}

void TranslateManager::setUtils(CppImportUtilities* utils)
{
	Q_ASSERT(utils);
	utils_ = utils;
}

OOModel::Module *TranslateManager::insertNamespace(clang::NamespaceDecl* namespaceDecl)
{
	const QString hash = nh_->hashNameSpace(namespaceDecl);
	if(nameSpaceMap_.contains(hash))
		return nameSpaceMap_.value(hash);
	OOModel::Module* ooModule = new OOModel::Module(QString::fromStdString(namespaceDecl->getNameAsString()));
	nameSpaceMap_.insert(hash, ooModule);
	if(namespaceDecl->getDeclContext()->isTranslationUnit())
		rootProject_->modules()->append(ooModule);
	else if(auto p = llvm::dyn_cast<clang::NamespaceDecl>(namespaceDecl->getDeclContext()))
	{
		const QString pHash = nh_->hashNameSpace(p);
		if(!nameSpaceMap_.contains(pHash))
			return nullptr;
		nameSpaceMap_.value(pHash)->modules()->append(ooModule);
	}
	else
		return nullptr;
	return ooModule;
}

bool TranslateManager::insertClass(clang::CXXRecordDecl* rDecl, OOModel::Class* ooClass)
{
	const QString hash = nh_->hashRecord(rDecl);
	// if rdecl is not managed yet add it:
	if(!classMap_.contains(hash))
	{
		classMap_.insert(hash,ooClass);
		return true;
	}
	return false;
}

bool TranslateManager::insertClassTemplate(clang::ClassTemplateDecl* classTemplate, OOModel::Class* ooClass)
{
	const QString hash = nh_->hashClassTemplate(classTemplate);
	if(!classMap_.contains(hash))
	{
		classMap_.insert(hash, ooClass);
		return true;
	}
	return false;
}

bool TranslateManager::insertClassTemplateSpec
(clang::ClassTemplateSpecializationDecl* classTemplate, OOModel::Class* ooClass)
{
	const QString hash = nh_->hashClassTemplateSpec(classTemplate);
	if(!classMap_.contains(hash))
	{
		classMap_.insert(hash, ooClass);
		return true;
	}
	return false;
}

OOModel::Method* TranslateManager::insertMethodDecl(clang::CXXMethodDecl* mDecl, OOModel::Method::MethodKind kind)
{
	OOModel::Method* method = nullptr;
	// only consider methods where the parent has already been visited
	if(classMap_.contains(nh_->hashRecord(mDecl->getParent())))
	{
		const QString hash = nh_->hashMethod(mDecl);
		if(!methodMap_.contains(hash))
			method = addNewMethod(mDecl, kind);
		else
		{
			method = methodMap_.value(hash);
			// If the method in the map is just a declaration and the method we currently have is a definition
			// there might be some argument names in the definition which are not yet considered.
			// Therefore we look at them now.
			if(method->items()->size() || !mDecl->isThisDeclarationADefinition())
				return method;
			for(int i = 0; i< method->arguments()->size(); i++)
			{
				OOModel::FormalArgument* ooArg = method->arguments()->at(i);
				// note that this never should/can be out of range otherwise the hash would be different
				ooArg->setName(QString::fromStdString(mDecl->getParamDecl(i)->getNameAsString()));
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
	if(!functionMap_.contains(hash))
		ooFunction = addNewFunction(functionDecl);
	else
	{
		ooFunction = functionMap_.value(hash);
		if(ooFunction->items()->size())
			return ooFunction;
		// the method which is in the map is just a declaration
		// therefore it might miss some arguments name which we collect here
		for(int i = 0; i< ooFunction->arguments()->size(); i++)
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
	if(classMap_.contains(hash))
	{
		OOModel::Field* ooField = new OOModel::Field();
		ooField->setName(QString::fromStdString(fieldDecl->getNameAsString()));
		classMap_.value(hash)->fields()->append(ooField);
		return ooField;
	}
	return nullptr;
}

OOModel::Field* TranslateManager::insertStaticField(clang::VarDecl* varDecl, bool& wasDeclared)
{
	const QString hash = nh_->hashStaticField(varDecl);
	if(staticFieldMap_.contains(hash))
	{
		wasDeclared = true;
		return staticFieldMap_.value(hash);
	}
	wasDeclared = false;
	const QString parentHash = nh_->hashParentOfStaticField(varDecl->getDeclContext());
	if(classMap_.contains(parentHash))
	{
		OOModel::Field* ooField = new OOModel::Field(QString::fromStdString(varDecl->getNameAsString()));
		classMap_.value(parentHash)->fields()->append(ooField);
		staticFieldMap_.insert(hash, ooField);
		return ooField;
	}
	return nullptr;
}

OOModel::ExplicitTemplateInstantiation* TranslateManager::insertExplicitTemplateInstantiation
(const clang::ClassTemplateSpecializationDecl* explicitTemplateInst)
{
	OOModel::ExplicitTemplateInstantiation* ooExplicitTemplateInst = nullptr;
	const QString hash = nh_->hashClassTemplateSpec(explicitTemplateInst);
	if(!explicitTemplateInstMap_.contains(hash))
	{
		ooExplicitTemplateInst = new OOModel::ExplicitTemplateInstantiation();
		explicitTemplateInstMap_.insert(hash, ooExplicitTemplateInst);
	}
	return ooExplicitTemplateInst;
}

OOModel::NameImport* TranslateManager::insertUsingDecl(clang::UsingDecl* usingDecl)
{
	OOModel::NameImport* ooName = nullptr;
	const QString hash = nh_->hashUsingDecl(usingDecl);
	if(!usingDeclMap_.contains(hash))
	{
		ooName = new OOModel::NameImport();
		usingDeclMap_.insert(hash, ooName);
	}
	return ooName;
}

OOModel::NameImport* TranslateManager::insertUsingDirective(clang::UsingDirectiveDecl* usingDirective)
{
	OOModel::NameImport* ooName = nullptr;
	const QString hash = nh_->hashUsingDirective(usingDirective);
	if(!usingDirectiveMap_.contains(hash))
	{
		ooName = new OOModel::NameImport();
		usingDirectiveMap_.insert(hash, ooName);
	}
	return ooName;
}

OOModel::NameImport* TranslateManager::insertUnresolvedUsing(clang::UnresolvedUsingValueDecl* unresolvedUsing)
{
	OOModel::NameImport* ooName = nullptr;
	const QString hash = nh_->hashUnresolvedUsingDecl(unresolvedUsing);
	if(!usingDeclMap_.contains(hash))
	{
		ooName = new OOModel::NameImport();
		usingDeclMap_.insert(hash, ooName);
	}
	return ooName;
}

OOModel::TypeAlias*TranslateManager::insertNamespaceAlias(clang::NamespaceAliasDecl* namespaceAlias)
{
	OOModel::TypeAlias* ooAlias = nullptr;
	const QString hash = nh_->hashNameSpaceAlias(namespaceAlias);
	if(!namespacAliasMap_.contains(hash))
	{
		ooAlias = new OOModel::TypeAlias();
		namespacAliasMap_.insert(hash, ooAlias);
	}
	return ooAlias;
}

OOModel::TypeAlias*TranslateManager::insertTypeAlias(clang::TypedefNameDecl* typeAlias)
{
	OOModel::TypeAlias* ooAlias = nullptr;
	const QString hash = nh_->hashTypeAlias(typeAlias);
	if(!typeAliasMap_.contains(hash))
	{
		ooAlias = new OOModel::TypeAlias();
		typeAliasMap_.insert(hash, ooAlias);
	}
	return ooAlias;
}

OOModel::TypeAlias* TranslateManager::insertTypeAliasTemplate(clang::TypeAliasTemplateDecl* typeAliasTemplate)
{
	OOModel::TypeAlias* ooAlias = nullptr;
	const QString hash = nh_->hashTypeAliasTemplate(typeAliasTemplate);
	if(!typeAliasMap_.contains(hash))
	{
		ooAlias = new OOModel::TypeAlias();
		typeAliasMap_.insert(hash, ooAlias);
	}
	return ooAlias;
}

OOModel::Method* TranslateManager::addNewMethod(clang::CXXMethodDecl* mDecl, OOModel::Method::MethodKind kind)
{
	const QString hash = nh_->hashMethod(mDecl);
	// remove type argument from name because clang has that in the name of constructors of template classes.
	// template<class T> class List{ List();}; results in List<T> as name.
	QString name = QString::fromStdString(mDecl->getNameAsString());
	if(name.endsWith(">"))
		name = name.left(name.indexOf("<"));
	OOModel::Method* method = new OOModel::Method(name, kind);
	if(!llvm::isa<clang::CXXConstructorDecl>(mDecl) && !llvm::isa<clang::CXXDestructorDecl>(mDecl))
	{
		// process result type
		OOModel::Expression* restype = utils_->translateQualifiedType(mDecl->getResultType(), mDecl->getLocStart());
		if(restype)
		{
			OOModel::FormalResult* methodResult = new OOModel::FormalResult();
			methodResult->setTypeExpression(restype);
			method->results()->append(methodResult);
		}
	}
	// process arguments
	clang::FunctionDecl::param_const_iterator it = mDecl->param_begin();
	for(;it != mDecl->param_end();++it)
	{
		OOModel::FormalArgument* arg = new OOModel::FormalArgument();
		arg->setName(QString::fromStdString((*it)->getNameAsString()));
		OOModel::Expression* type = utils_->translateQualifiedType((*it)->getType(), (*it)->getLocStart());
		if(type) arg->setTypeExpression(type);
		method->arguments()->append(arg);
	}
	// find the correct class to add the method
	if(classMap_.contains(nh_->hashRecord(mDecl->getParent())))
	{
		OOModel::Class* parent = classMap_.value(nh_->hashRecord(mDecl->getParent()));
		parent->methods()->append(method);
	}
	else
		std::cout << "ERROR TRANSLATEMNGR: METHOD DECL NO PARENT FOUND" << std::endl;

	methodMap_.insert(hash, method);

	return method;
}

OOModel::Method* TranslateManager::addNewFunction(clang::FunctionDecl* functionDecl)
{
	// add a new method
	OOModel::Method* ooFunction= new OOModel::Method();
	ooFunction->setName(QString::fromStdString(functionDecl->getNameAsString()));
	// process result type
	OOModel::Expression* restype = utils_->translateQualifiedType(functionDecl->getResultType(),
																					  functionDecl->getLocStart());
	if(restype)
	{
		OOModel::FormalResult* methodResult = new OOModel::FormalResult();
		methodResult->setTypeExpression(restype);
		ooFunction->results()->append(methodResult);
	}
	// process arguments
	clang::FunctionDecl::param_const_iterator it = functionDecl->param_begin();
	for(;it != functionDecl->param_end();++it)
	{
		OOModel::FormalArgument* arg = new OOModel::FormalArgument();
		arg->setName(QString::fromStdString((*it)->getNameAsString()));
		OOModel::Expression* type = utils_->translateQualifiedType((*it)->getType(), (*it)->getLocStart());
		if(type) arg->setTypeExpression(type);
		ooFunction->arguments()->append(arg);
	}

	functionMap_.insert(nh_->hashFunction(functionDecl),ooFunction);

	return ooFunction;
}

bool TranslateManager::containsClass(clang::CXXRecordDecl* recordDecl)
{
	return classMap_.contains(nh_->hashRecord(recordDecl));
}

}
