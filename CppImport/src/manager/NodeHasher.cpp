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

#include "NodeHasher.h"
#include "../CppImportException.h"

namespace CppImport {

NodeHasher::NodeHasher()
{}

void NodeHasher::setSourceManager(clang::SourceManager* mngr)
{
	srcMngr_ = mngr;
}

const QString NodeHasher::hashFunction(const clang::FunctionDecl* functionDecl)
{
	QString hash = QString::fromStdString(functionDecl->getNameAsString());
	hash.prepend(QString::fromStdString(functionDecl->getResultType().getAsString()).append("_"));
	hash.append("_").append(functionDecl->getNumParams());

	for(unsigned i = 0; i < functionDecl->param_size(); i++)
	{
		hash.append("_").append(hashType(functionDecl->getParamDecl(i)->getType()));
	}
	return hash;
}

const QString NodeHasher::hashMethod(const clang::CXXMethodDecl* methodDecl)
{
	QString hash = QString::fromStdString(methodDecl->getNameAsString());
	hash.prepend(QString::fromStdString(methodDecl->getResultType().getCanonicalType().getAsString()).append("_"));
	hash.append("_").append(hashRecord(methodDecl->getParent()));
	hash.append("_").append(methodDecl->getNumParams());

	for(unsigned i = 0; i < methodDecl->param_size(); i++)
	{
		hash.append("_").append(hashType(methodDecl->getParamDecl(i)->getType()));
	}
	return hash;
}

const QString NodeHasher::hashRecord(const clang::RecordDecl* recordDecl)
{
	if(auto classTemplateSpec = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(recordDecl))
		return hashClassTemplateSpec(classTemplateSpec);
	QString hash = QString::fromStdString(recordDecl->getNameAsString());
	if(auto parentNamedDecl = llvm::dyn_cast<clang::NamedDecl>(recordDecl->getParent()))
		hash.prepend("_").prepend(QString::fromStdString(parentNamedDecl->getNameAsString()));
	if(auto cxxRecord = llvm::dyn_cast<clang::CXXRecordDecl>(recordDecl))
	{
		if(auto classTemplate = cxxRecord->getDescribedClassTemplate())
		{
			auto templateParamList = classTemplate->getTemplateParameters();
			for( auto templateParam = templateParamList->begin();
				  templateParam != templateParamList->end(); ++templateParam)
			{
				hash.append("_");
				// TODO: maybe just use character data from srcmanager
				if(auto templateType = llvm::dyn_cast<clang::TemplateTypeParmDecl>(*templateParam))
					hash.append(hashTemplateTypeParm(templateType));
				else if(auto nonTemplateType = llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(*templateParam))
					hash.append(hashTemplateTypeParm(nonTemplateType));
			}
		}
	}
	return hash;
}

const QString NodeHasher::hashClassTemplate(const clang::ClassTemplateDecl* classTemplate)
{
	QString hash = QString::fromStdString(classTemplate->getNameAsString());
	if(auto parentNamedDecl = llvm::dyn_cast<clang::NamedDecl>(classTemplate->getTemplatedDecl()->getParent()))
		hash.prepend("_").prepend(QString::fromStdString(parentNamedDecl->getNameAsString()));
	auto templateParamList = classTemplate->getTemplateParameters();
	for( auto templateParam = templateParamList->begin();
		  templateParam != templateParamList->end(); ++templateParam)
	{
		hash.append("_");
		// TODO: maybe just use character data from srcmanager
		if(auto templateType = llvm::dyn_cast<clang::TemplateTypeParmDecl>(*templateParam))
			hash.append(hashTemplateTypeParm(templateType));
		else if(auto nonTemplateType = llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(*templateParam))
			hash.append(hashTemplateTypeParm(nonTemplateType));
	}
	return hash;
}

const QString NodeHasher::hashClassTemplateSpec(const clang::ClassTemplateSpecializationDecl* classTemplateSpec)
{
	QString hash = QString::fromStdString(classTemplateSpec->getNameAsString());
	if(auto parentNamedDecl = llvm::dyn_cast<clang::NamedDecl>(classTemplateSpec->getParent()))
		hash.prepend("_").prepend(QString::fromStdString(parentNamedDecl->getNameAsString()));
	for(unsigned i = 0; i < classTemplateSpec->getTemplateArgs().size(); i++)
		hash.append("_").append(hashTemplateArg(classTemplateSpec->getTemplateArgs().get(i)));
	return hash;
}

const QString NodeHasher::hashType(const clang::QualType& type)
{
	return QString::fromStdString(type.getCanonicalType().getAsString());
}

const QString NodeHasher::hashTemplateTypeParm(const clang::TemplateTypeParmDecl* parm)
{
	QString hash = QString::fromStdString(parm->getNameAsString());
	if(parm->hasDefaultArgument())
		hash.append("=").append(hashType(parm->getDefaultArgument()));
	return hash;
}

const QString NodeHasher::hashTemplateTypeParm(const clang::NonTypeTemplateParmDecl* parm)
{
	QString hash = QString::fromStdString(parm->getNameAsString());
	hash.append("_").append(QString::fromStdString(parm->getType().getCanonicalType().getAsString()));
	if(parm->hasDefaultArgument())
		hash.append("=").append(QString(srcMngr_->getCharacterData(parm->getDefaultArgumentLoc())));
	return hash;
}

const QString NodeHasher::hashTemplateArg(const clang::TemplateArgument& arg)
{
	QString hash;
	switch(arg.getKind())
	{
		case clang::TemplateArgument::ArgKind::Null:
			hash = "null"; break;
		case clang::TemplateArgument::ArgKind::Type:
			hash = hashType(arg.getAsType());
			break;
		case clang::TemplateArgument::ArgKind::Declaration:
			hash = QString::fromStdString(arg.getAsDecl()->getNameAsString());
			hash.prepend("_").prepend(hashType(arg.getAsDecl()->getType()));
			break;
		case clang::TemplateArgument::ArgKind::NullPtr:
			hash = "nullptr"; break;
		case clang::TemplateArgument::ArgKind::Integral:
			hash.setNum(arg.getAsIntegral().getLimitedValue());
			break;
		case clang::TemplateArgument::ArgKind::Template:
			hash = QString::fromStdString(arg.getAsTemplate().getAsTemplateDecl()->getNameAsString());
			break;
		case clang::TemplateArgument::ArgKind::TemplateExpansion:
			// TODO: add support
			hash = "EXPANSION"; break;
		case clang::TemplateArgument::ArgKind::Expression:
			hash = QString(srcMngr_->getCharacterData(arg.getAsExpr()->getLocStart()));
			break;
		case clang::TemplateArgument::ArgKind::Pack:
			// TODO: add support
			hash = "PACK"; break;
		default:
			throw CppImportException("Invalid Template Argument kind");
	}
	return hash;
}

}
