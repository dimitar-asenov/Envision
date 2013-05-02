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

TranslateManager::TranslateManager(Model::Model* model, OOModel::Project* project, CppImportUtilities* utils) :
	model_(model), project_(project), utils_(utils)
{
}

OOModel::Module *TranslateManager::insertNamespace(clang::NamespaceDecl *nd, int depth)
{
	if(nameSpaceMap_.contains(nd))
		return nameSpaceMap_.value(nd).first;
	else
	{
		QMap<clang::NamespaceDecl*, QPair<OOModel::Module*,int> >::iterator it = nameSpaceMap_.begin();
		for(;it!=nameSpaceMap_.end();++it)
		{
			if(it.key()->getNameAsString() == nd->getNameAsString())
			{
				// the namespace is already in the map
				// check if both have the same depth
				if(it.value().second == depth)
					// the namespace is already existing
					return it.value().first;
			}
		}
		OOModel::Module* ooModule = new OOModel::Module();
		ooModule->setName(QString::fromStdString(nd->getNameAsString()));
		// TODO handle depth of namespace
		nameSpaceMap_.insert(nd,qMakePair(ooModule,depth));
		return ooModule;
	}
}

bool TranslateManager::insertClass(clang::CXXRecordDecl* rDecl, OOModel::Class* ooClass)
{
	// if rdecl is not managed yet add it:
	if(!classMap_.contains(rDecl))
	{
		classMap_.insert(rDecl,ooClass);
		return true;
	}
	std::cout << "ERROR TRANSLATEMNGR: CLASS "<< rDecl->getNameAsString() << " ALREADY IN MAP" << std::endl;
	return false;
}

OOModel::Method* TranslateManager::insertMethodDecl(clang::CXXMethodDecl* mDecl)
{
	OOModel::Method* method = nullptr;
	// only consider methods where the parent has already been visited
	if(classMap_.contains(mDecl->getParent()))
	{
		if(!methodMap_.contains(mDecl))
		{
			// Look if there is a function with same name in map
			QMap<clang::CXXMethodDecl*,OOModel::Method*>::iterator it = methodMap_.begin();
			for(;it!=methodMap_.end();++it)
			{
				clang::CXXMethodDecl* inMapDecl = it.key();
				if(!mDecl->getNameAsString().compare(inMapDecl->getNameAsString()) &&
						!inMapDecl->isThisDeclarationADefinition() && mDecl->isThisDeclarationADefinition())
				{
					// found a pair with same name and only one is defined
					if((mDecl->getResultType() == inMapDecl->getResultType()) &&
							(mDecl->param_size() == inMapDecl->param_size()))
					{
						bool matching = true;
						for(unsigned i = 0; i < mDecl->param_size(); i++)
						{
							if(mDecl->getParamDecl(i)->getType() != inMapDecl->getParamDecl(i)->getType())
								matching = false;
						}
						if(matching)
						{
							method = it.value();
							break;
						}
					}
				}
			}
			// check if method node exists or else create one
			method = method ? method : addNewMethod(mDecl);
		}
	}
	return method;
}

OOModel::Method*TranslateManager::insertFunctionDecl(clang::FunctionDecl* functionDecl)
{
	OOModel::Method* ooFunction = nullptr;
	if(!functionMap_.contains(functionDecl))
	{
		// Look if there is a function with same name in map
		QMap<clang::FunctionDecl*,OOModel::Method*>::iterator it = functionMap_.begin();
		for(;it!=functionMap_.end();++it)
		{
			clang::FunctionDecl* inMapDecl = it.key();
			if(!functionDecl->getNameAsString().compare(inMapDecl->getNameAsString()) &&
					!inMapDecl->isThisDeclarationADefinition() && functionDecl->isThisDeclarationADefinition())
			{
				// found a pair with same name and only one is defined
				if((functionDecl->getResultType() == inMapDecl->getResultType()) &&
						(functionDecl->param_size() == inMapDecl->param_size()))
				{
					bool matching = true;
					for(unsigned i = 0; i < functionDecl->param_size(); i++)
					{
						if(functionDecl->getParamDecl(i)->getType() != inMapDecl->getParamDecl(i)->getType())
							matching = false;
					}
					if(matching)
					{
						ooFunction = it.value();
						break;
					}
				}
			}
		}
		// check if method node exists or else create one
		ooFunction = ooFunction ? ooFunction : addNewFunction(functionDecl);
	}
	return ooFunction;
}

OOModel::Field* TranslateManager::insertField(clang::FieldDecl* fDecl)
{
	clang::CXXRecordDecl* parentClass = llvm::dyn_cast<clang::CXXRecordDecl>(fDecl->getParent());
	if(parentClass && classMap_.contains(parentClass))
	{
		OOModel::Field* field = new OOModel::Field();
		field->setName(QString::fromStdString(fDecl->getNameAsString()));
		classMap_.value(parentClass)->fields()->append(field);
		return field;
	}
	return nullptr;
}

OOModel::VariableDeclaration* TranslateManager::insertVar(clang::VarDecl* vDecl)
{
	OOModel::VariableDeclaration* ooVarDecl = nullptr;
	if(!varMap_.contains(vDecl))
	{
		ooVarDecl = new OOModel::VariableDeclaration();
		ooVarDecl->setName(QString::fromStdString(vDecl->getNameAsString()));
		varMap_.insert(vDecl,ooVarDecl);
	}
	else
		std::cout << "ERROR TRANSLATEMNGR: ---------->VAR : " << vDecl->getNameAsString() << " IS ALREADY IN THE MAP" << std::endl;
	return ooVarDecl;
}

OOModel::VariableDeclaration* TranslateManager::getVar(clang::VarDecl* vDecl)
{
	if(varMap_.contains(vDecl))
		return varMap_.value(vDecl);
	return nullptr;
}

OOModel::Method* TranslateManager::addNewMethod(clang::CXXMethodDecl* mDecl)
{
	// add a new method
	OOModel::Method* method = new OOModel::Method();
	method->setName(QString::fromStdString(mDecl->getNameAsString()));
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
	if(classMap_.contains(mDecl->getParent()))
	{
		OOModel::Class* parent = classMap_.value(mDecl->getParent());
		parent->methods()->append(method);
	}
	else
		std::cout << "ERROR TRANSLATEMNGR: METHOD DECL NO PARENT FOUND" << std::endl;

	methodMap_.insert(mDecl,method);

	return method;
}

OOModel::Method*TranslateManager::addNewFunction(clang::FunctionDecl* functionDecl)
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

	functionMap_.insert(functionDecl,ooFunction);

	return ooFunction;
}

}
