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

TranslateManager::TranslateManager(CppImportUtilities* utils)
	: utils_(utils)
{}

TranslateManager::~TranslateManager()
{
	SAFE_DELETE(nh_);
}

void TranslateManager::setSourceManager(clang::SourceManager* mngr)
{
	nh_->setSourceManager(mngr);
}

OOModel::Module *TranslateManager::insertNamespace(clang::NamespaceDecl *nd, int depth)
{
	QString hash = QString::fromStdString(nd->getNameAsString());
	hash.append("_").append(depth);
	if(nameSpaceMap_.contains(hash))
		return nameSpaceMap_.value(hash);

	OOModel::Module* ooModule = new OOModel::Module();
	ooModule->setName(QString::fromStdString(nd->getNameAsString()));
	nameSpaceMap_.insert(hash, ooModule);
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
			if(method->items()->size())
				return method;
			// the method which is in the map is just a declaration
			// therefore it might miss some arguments name which we collect here
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

OOModel::Field* TranslateManager::insertField(clang::FieldDecl* fDecl)
{
	const QString hash = nh_->hashRecord(fDecl->getParent());
	if(classMap_.contains(hash))
	{
		OOModel::Field* field = new OOModel::Field();
		field->setName(QString::fromStdString(fDecl->getNameAsString()));
		classMap_.value(hash)->fields()->append(field);
		return field;
	}
	return nullptr;
}

OOModel::Method* TranslateManager::addNewMethod(clang::CXXMethodDecl* mDecl, OOModel::Method::MethodKind kind)
{
	const QString hash = nh_->hashMethod(mDecl);
	OOModel::Method* method = new OOModel::Method(QString::fromStdString(mDecl->getNameAsString()), kind);
	// process result type
	OOModel::Expression* restype = utils_->convertClangType(mDecl->getResultType());
	if(restype)
	{
		OOModel::FormalResult* methodResult = new OOModel::FormalResult();
		methodResult->setTypeExpression(restype);
		method->results()->append(methodResult);
	}
	// process arguments
	clang::FunctionDecl::param_const_iterator it = mDecl->param_begin();
	for(;it != mDecl->param_end();++it)
	{
		OOModel::FormalArgument* arg = new OOModel::FormalArgument();
		arg->setName(QString::fromStdString((*it)->getNameAsString()));
		OOModel::Expression* type = utils_->convertClangType((*it)->getType());
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
	OOModel::Expression* restype = utils_->convertClangType(functionDecl->getResultType());
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
		OOModel::Expression* type = utils_->convertClangType((*it)->getType());
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
