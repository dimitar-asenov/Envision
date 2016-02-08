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

#include "NodeHasher.h"
#include "../CppImportException.h"

namespace CppImport {

NodeHasher::NodeHasher(ClangHelpers& clang) : clang_{clang} {}

const QString NodeHasher::hashFunction(const clang::FunctionDecl* functionDecl)
{
	QString hash = QString::fromStdString(functionDecl->getNameAsString());
	hash.prepend(QString::fromStdString(functionDecl->getReturnType().getAsString()).append("_"));
	hash.append("_").append(functionDecl->getNumParams());

	for (unsigned i = 0; i < functionDecl->param_size(); i++)
	{
		hash.append("_").append(hashType(functionDecl->getParamDecl(i)->getType()));
	}
	return hash;
}

const QString NodeHasher::hashMethod(const clang::CXXMethodDecl* methodDecl)
{
	QString hash = QString::fromStdString(methodDecl->getNameAsString());
	hash.prepend(QString::fromStdString(methodDecl->getReturnType().getCanonicalType().getAsString()).append("_"));
	hash.append("_").append(hashRecord(methodDecl->getParent()));
	hash.append("_").append(methodDecl->getNumParams());

	for (unsigned i = 0; i < methodDecl->param_size(); i++)
	{
		hash.append("_").append(hashType(methodDecl->getParamDecl(i)->getType()));
	}
	return hash;
}

const QString NodeHasher::hashNameSpace(const clang::NamespaceDecl* namespaceDecl)
{
	QString hash = QString::fromStdString(namespaceDecl->getNameAsString());
	if (auto ctxt = namespaceDecl->getDeclContext())
	{
		if (ctxt->isTranslationUnit())
			return hash;
		else if (auto pn = llvm::dyn_cast<clang::NamespaceDecl>(ctxt))
			return hash.prepend(hashNameSpace(pn));
		throw CppImportException{"Invalid decl context in namespace"};
	}
	return hash;
}

const QString NodeHasher::hashRecord(const clang::RecordDecl* recordDecl)
{
	if (auto classTemplateSpec = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(recordDecl))
		return hashClassTemplateSpec(classTemplateSpec);
	QString hash = QString::fromStdString(recordDecl->getNameAsString());
	if (auto parentNamedDecl = llvm::dyn_cast<clang::NamedDecl>(recordDecl->getParent()))
		hash.prepend("_").prepend(QString::fromStdString(parentNamedDecl->getNameAsString()));
	if (auto cxxRecord = llvm::dyn_cast<clang::CXXRecordDecl>(recordDecl))
	{
		if (auto classTemplate = cxxRecord->getDescribedClassTemplate())
		{
			auto templateParamList = classTemplate->getTemplateParameters();
			for (auto templateParameter : *templateParamList)
			{
				hash.append("_");
				if (auto templateType = llvm::dyn_cast<clang::TemplateTypeParmDecl>(templateParameter))
					hash.append(hashTemplateTypeParm(templateType));
				else if (auto nonTemplateType = llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(templateParameter))
					hash.append(hashTemplateTypeParm(nonTemplateType));
			}
		}
	}
	return hash;
}

const QString NodeHasher::hashClassTemplate(const clang::ClassTemplateDecl* classTemplate)
{
	QString hash = QString::fromStdString(classTemplate->getNameAsString());
	if (auto parentNamedDecl = llvm::dyn_cast<clang::NamedDecl>(classTemplate->getTemplatedDecl()->getParent()))
		hash.prepend("_").prepend(QString::fromStdString(parentNamedDecl->getNameAsString()));
	auto templateParamList = classTemplate->getTemplateParameters();
	for (auto & elem : *templateParamList)
	{
		hash.append("_");
		if (auto templateType = llvm::dyn_cast<clang::TemplateTypeParmDecl>(elem))
			hash.append(hashTemplateTypeParm(templateType));
		else if (auto nonTemplateType = llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(elem))
			hash.append(hashTemplateTypeParm(nonTemplateType));
	}
	return hash;
}

const QString NodeHasher::hashClassTemplateSpec(const clang::ClassTemplateSpecializationDecl* classTemplateSpec)
{
	QString hash = QString::fromStdString(classTemplateSpec->getNameAsString());
	if (auto parentNamedDecl = llvm::dyn_cast<clang::NamedDecl>(classTemplateSpec->getParent()))
		hash.prepend("_").prepend(QString::fromStdString(parentNamedDecl->getNameAsString()));
	for (unsigned i = 0; i < classTemplateSpec->getTemplateArgs().size(); i++)
		hash.append("_").append(hashTemplateArg(classTemplateSpec->getTemplateArgs().get(i)));
	return hash;
}

const QString NodeHasher::hashStaticField(const clang::VarDecl* varDecl)
{
	QString hash = QString::fromStdString(varDecl->getNameAsString()).prepend("_");
	hash.prepend(hashParentOfStaticField(varDecl->getDeclContext()));
	hash.append("_").append(hashType(varDecl->getType()));
	return hash;
}

const QString NodeHasher::hashNamespaceField(const clang::VarDecl* varDecl)
{
	QString hash = QString::fromStdString(varDecl->getNameAsString()).prepend("_");
	hash.prepend(hashNameSpace(llvm::dyn_cast<clang::NamespaceDecl>(varDecl->getDeclContext())));
	hash.append("_").append(hashType(varDecl->getType()));
	return hash;
}

const QString NodeHasher::hashParentOfStaticField(const clang::DeclContext* context)
{
	if (auto cts = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(context))
		return hashClassTemplateSpec(cts);
	else if (auto ct = llvm::dyn_cast<clang::ClassTemplateDecl>(context))
		return hashClassTemplate(ct);
	else if (auto c = llvm::dyn_cast<clang::CXXRecordDecl>(context))
		return hashRecord(c);
	throw CppImportException{"Invalid parent to hash a static var"};
}

const QString NodeHasher::hashUsingParent(const clang::DeclContext* context)
{
	if (context->isTranslationUnit())
		return QString{};
	if (auto ns = llvm::dyn_cast<clang::NamespaceDecl>(context))
		return hashNameSpace(ns);
	else if (auto cts = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(context))
		return hashClassTemplateSpec(cts);
	else if (auto ct = llvm::dyn_cast<clang::ClassTemplateDecl>(context))
		return hashClassTemplate(ct);
	else if (auto c = llvm::dyn_cast<clang::CXXRecordDecl>(context))
		return hashRecord(c);
	else if (auto m = llvm::dyn_cast<clang::CXXMethodDecl>(context))
		return hashMethod(m);
	else if (auto f = llvm::dyn_cast<clang::FunctionDecl>(context))
		return hashFunction(f);
	throw CppImportException{"Invalid parent for using directive or using decl"};
}

const QString NodeHasher::hashUsingDirective(const clang::UsingDirectiveDecl* usingDirective)
{
	QString hash = QString::fromStdString(usingDirective->getNominatedNamespaceAsWritten()->getNameAsString());
	if (auto p = usingDirective->getQualifier())
		hash.prepend("_").prepend(hashNestedNameSpecifier(p));
	hash.prepend("_").prepend(hashUsingParent(usingDirective->getDeclContext()));
	return hash;
}

const QString NodeHasher::hashUsingDecl(const clang::UsingDecl* usingDecl)
{
	QString hash = QString::fromStdString(usingDecl->getNameAsString());
	if (auto p = usingDecl->getQualifier())
		hash.prepend("_").prepend(hashNestedNameSpecifier(p));
	hash.prepend("_").prepend(hashUsingParent(usingDecl->getDeclContext()));
	return hash;
}

const QString NodeHasher::hashUnresolvedUsingDecl(const clang::UnresolvedUsingValueDecl* unresolvedUsing)
{
	QString hash = QString::fromStdString(unresolvedUsing->getNameInfo().getAsString());
	if (auto p = unresolvedUsing->getQualifier())
		hash.prepend("_").prepend(hashNestedNameSpecifier(p));
	hash.prepend("_").prepend(hashUsingParent(unresolvedUsing->getDeclContext()));
	return hash;
}

const QString NodeHasher::hashNameSpaceAlias(const clang::NamespaceAliasDecl* namespaceAlias)
{
	QString hash = QString::fromStdString(namespaceAlias->getNameAsString());
	hash.prepend("_").prepend(hashUsingParent(namespaceAlias->getDeclContext()));
	hash.append("_").append(hashNestedNameSpecifier(namespaceAlias->getQualifier()));
	hash.append("_").append(hashNameSpace(namespaceAlias->getNamespace()));
	return hash;
}

const QString NodeHasher::hashTypeAlias(const clang::TypedefNameDecl* typeAlias)
{
	QString hash = QString::fromStdString(typeAlias->getNameAsString());
	hash.prepend("_").prepend(hashUsingParent(typeAlias->getDeclContext()));
	hash.append("_").append(hashTypeSourceInfo(typeAlias->getTypeSourceInfo()));
	return hash;
}

const QString NodeHasher::hashTypeAliasTemplate(const clang::TypeAliasTemplateDecl* typeAliasTemplate)
{
	QString hash = hashTypeAlias(typeAliasTemplate->getTemplatedDecl());
	auto templateParamList = typeAliasTemplate->getTemplateParameters();
	for (auto templateParameter : *templateParamList)
	{
		hash.append("_");
		if (auto templateType = llvm::dyn_cast<clang::TemplateTypeParmDecl>(templateParameter))
			hash.append(hashTemplateTypeParm(templateType));
		else if (auto nonTemplateType = llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(templateParameter))
			hash.append(hashTemplateTypeParm(nonTemplateType));
	}
	return hash;
}

const QString NodeHasher::hashNestedNameSpecifier(const clang::NestedNameSpecifier* nestedName)
{
	QString hash;
	switch (nestedName->getKind())
	{
		case clang::NestedNameSpecifier::Identifier:
			hash = QString{nestedName->getAsIdentifier()->getNameStart()};
			break;
		case clang::NestedNameSpecifier::Namespace:
			hash = QString::fromStdString(nestedName->getAsNamespace()->getNameAsString());
			break;
		case clang::NestedNameSpecifier::NamespaceAlias:
			hash = QString::fromStdString(nestedName->getAsNamespaceAlias()->getNameAsString());
			break;
		case clang::NestedNameSpecifier::TypeSpec:
			hash = hashType(nestedName->getAsType()->getCanonicalTypeInternal());
			break;
		case clang::NestedNameSpecifier::TypeSpecWithTemplate:
			hash = hashType(nestedName->getAsType()->getCanonicalTypeInternal());
			break;
		case clang::NestedNameSpecifier::Global:
			hash = "_";
			break;
		default:
			// In version 3.6 this is only NestedNameSpecifier::Super, which is a Microsoft specific extension (_super).
			throw new CppImportException{QString{"Unsupported nested name specifier kind: %1"}.arg(nestedName->getKind())};
			break;
	}
	if (auto p = nestedName->getPrefix())
		hash.prepend("_").prepend(hashNestedNameSpecifier(p));
	return hash;
}

const QString NodeHasher::hashType(const clang::QualType& type)
{
	return QString::fromStdString(type.getCanonicalType().getAsString());
}

const QString NodeHasher::hashTypeSourceInfo(const clang::TypeSourceInfo* info)
{
	QString hash(clang_.sourceManager()->getCharacterData(info->getTypeLoc().getBeginLoc()));
	hash.truncate(hash.indexOf(";"));
	return hash;
}

const QString NodeHasher::hashTemplateTypeParm(const clang::TemplateTypeParmDecl* templTypeParam)
{
	QString hash = QString::fromStdString(templTypeParam->getNameAsString());
	if (templTypeParam->hasDefaultArgument())
		hash.append("=").append(hashType(templTypeParam->getDefaultArgument()));
	return hash;
}

const QString NodeHasher::hashTemplateTypeParm(const clang::NonTypeTemplateParmDecl* nonTypeTemplParam)
{
	QString hash = QString::fromStdString(nonTypeTemplParam->getNameAsString());
	hash.append("_").append(QString::fromStdString(nonTypeTemplParam->getType().getCanonicalType().getAsString()));
	if (nonTypeTemplParam->hasDefaultArgument())
		hash.append("=").append(QString{clang_.sourceManager()->getCharacterData(
													  nonTypeTemplParam->getDefaultArgumentLoc())});
	return hash;
}

const QString NodeHasher::hashTemplateArg(const clang::TemplateArgument& templateArg)
{
	QString hash;
	switch (templateArg.getKind())
	{
		case clang::TemplateArgument::ArgKind::Null:
			hash = "null"; break;
		case clang::TemplateArgument::ArgKind::Type:
			hash = hashType(templateArg.getAsType());
			break;
		case clang::TemplateArgument::ArgKind::Declaration:
			hash = QString::fromStdString(templateArg.getAsDecl()->getNameAsString());
			hash.prepend("_").prepend(hashType(templateArg.getAsDecl()->getType()));
			break;
		case clang::TemplateArgument::ArgKind::NullPtr:
			hash = "nullptr"; break;
		case clang::TemplateArgument::ArgKind::Integral:
			hash.setNum(templateArg.getAsIntegral().getLimitedValue());
			break;
		case clang::TemplateArgument::ArgKind::Template:
			hash = QString::fromStdString(templateArg.getAsTemplate().getAsTemplateDecl()->getNameAsString());
			break;
		case clang::TemplateArgument::ArgKind::TemplateExpansion:
			// TODO: add support
			hash = "EXPANSION"; break;
		case clang::TemplateArgument::ArgKind::Expression:
			hash = QString{clang_.sourceManager()->getCharacterData(templateArg.getAsExpr()->getLocStart())};
			break;
		case clang::TemplateArgument::ArgKind::Pack:
			// TODO: add support
			hash = "PACK"; break;
		default:
			throw CppImportException{"Invalid Template Argument kind"};
	}
	return hash;
}

}
