/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "TypeUtilities.h"

#include <clang/AST/DeclTemplate.h>

QString TypeUtilities::nestedNameSpecifierToString(const clang::NestedNameSpecifier* nestedName)
{
	switch (nestedName->getKind())
	{
		case clang::NestedNameSpecifier::Identifier:
			return QString(nestedName->getAsIdentifier()->getNameStart());
		case clang::NestedNameSpecifier::Namespace:
			return QString::fromStdString(nestedName->getAsNamespace()->getQualifiedNameAsString());
		case clang::NestedNameSpecifier::NamespaceAlias:
			return QString::fromStdString(nestedName->getAsNamespaceAlias()->getQualifiedNameAsString());
		case clang::NestedNameSpecifier::Global:
			// if we have the Global specifier there can not be a prefix() other wise it is invalid C++
			Q_ASSERT(!nestedName->getPrefix());
			return "::";
		default:
			Q_ASSERT(false);// Implement support for more cases if needed.
	}
}

QString TypeUtilities::templateArgToString(const clang::TemplateArgument& templateArg)
{
	switch (templateArg.getKind())
	{
		case clang::TemplateArgument::ArgKind::Null:
			return {};
		case clang::TemplateArgument::ArgKind::Type:
			return typePtrToString(templateArg.getAsType().getTypePtr());
		case clang::TemplateArgument::ArgKind::Declaration:
			return QString::fromStdString(templateArg.getAsDecl()->getQualifiedNameAsString());
		default:
			Q_ASSERT(false); // Implement support for more cases if needed.
	}
}

QString TypeUtilities::typePtrToString(const clang::Type* type)
{
	if (auto recordType = llvm::dyn_cast<clang::RecordType>(type))
	{
		return QString::fromStdString(recordType->getDecl()->getQualifiedNameAsString());
	}
	else if (auto elaboratedType = llvm::dyn_cast<clang::ElaboratedType>(type))
	{
		// TODO: this might also have a keyword in front (like e.g. class, typename)
		return typePtrToString(elaboratedType->getNamedType().getTypePtr());
	}
	else if (auto templateSpecialization = llvm::dyn_cast<clang::TemplateSpecializationType>(type))
	{
		QString baseName = QString::fromStdString(
					templateSpecialization->getTemplateName().getAsTemplateDecl()->getQualifiedNameAsString());
		if (baseName == "Super")
		{
			// We "unwrap" the Super baseclass
			Q_ASSERT(templateSpecialization->getNumArgs() == 1u); // Super should only have one template argument!
			return templateArgToString(templateSpecialization->getArg(0));
		}
		else
		{
			QStringList stringifiedTemplateArgs;
			for (auto arg : *templateSpecialization) stringifiedTemplateArgs << templateArgToString(arg);
			return QString("%1<%2>").arg(baseName, stringifiedTemplateArgs.join(", "));
		}
	}
	else if (auto ptrType = llvm::dyn_cast<clang::PointerType>(type))
	{
		return typePtrToString(ptrType->getPointeeType().getTypePtr());
	}
	// Implement support for more cases if needed.
	qWarning() << "###Unsupported type: ###";
	type->dump();
	return {};
}
