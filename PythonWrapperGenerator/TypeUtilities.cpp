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

#include <QtCore/QDebug>

#include <clang/AST/DeclTemplate.h>

QString TypeUtilities::nestedNameSpecifierToString(const clang::NestedNameSpecifier* nestedName)
{
	QString translated;
	switch (nestedName->getKind())
	{
		case clang::NestedNameSpecifier::Identifier:
			translated = QString(nestedName->getAsIdentifier()->getNameStart());
			break;
		case clang::NestedNameSpecifier::Namespace:
			translated = QString::fromStdString(nestedName->getAsNamespace()->getQualifiedNameAsString());
			break;
		case clang::NestedNameSpecifier::NamespaceAlias:
			translated = QString::fromStdString(nestedName->getAsNamespaceAlias()->getQualifiedNameAsString());
			break;
		case clang::NestedNameSpecifier::TypeSpec:
			// TODO: handle the case where this cast is not a reference
//			if (auto typeRef = DCast<OOModel::ReferenceExpression>
//					(translateTypePtr(nestedName->getAsType(), location)))
//				currentRef = typeRef;
//			else
//			{
//				log_->writeError(className_, location, CppImportLogger::Reason::OTHER,
//									  "Unsupported NestedNameSpecifier TypeSpecNameSpecifier");
//				returnExpr = createErrorExpression("TypeSpecNameSpecifier");
//			}
			break;
		case clang::NestedNameSpecifier::TypeSpecWithTemplate:
			// TODO: handle the case where this cast is not a reference
//			if (auto typeRef = DCast<OOModel::ReferenceExpression>
//					(translateTypePtr(nestedName->getAsType(), location)))
//				currentRef = typeRef;
//			else
//			{
//				log_->writeError(className_, location, CppImportLogger::Reason::OTHER,
//									  "Unsupported NestedNameSpecifier TypeSpecWithTemplate");
//				returnExpr = createErrorExpression("Could not translate TypeSpecWithTemplate");
//			}
			break;
		case clang::NestedNameSpecifier::Global:
			// if we have the Global specifier there can not be a prefix() other wise it is invalid C++
			Q_ASSERT(!nestedName->getPrefix());
			translated = "::";
			break;
		default:
			// In version 3.6 this is only NestedNameSpecifier::Super, which is a Microsoft specific extension (_super).
			Q_ASSERT(false);
			break;
	}
	return translated;
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
		case clang::TemplateArgument::ArgKind::NullPtr:
			return {}; //new OOModel::NullLiteral();
		case clang::TemplateArgument::ArgKind::Integral:
			return {}; //new OOModel::IntegerLiteral(templateArg.getAsIntegral().getLimitedValue());
//		case clang::TemplateArgument::ArgKind::Template:
//			return new OOModel::ReferenceExpression(
//						QString::fromStdString(templateArg.getAsTemplate().getAsTemplateDecl()->getNameAsString()));
//		case clang::TemplateArgument::ArgKind::TemplateExpansion:
//			// TODO: add support
//			log_->writeError(className_, location, CppImportLogger::Reason::OTHER,
//								  "Unsupported TemplateArgument EXPANSION");
//			return createErrorExpression("Unsupported TemplateArgument EXPANSION");
//		case clang::TemplateArgument::ArgKind::Expression:
//			return exprVisitor_->translateExpression(templateArg.getAsExpr());
//		case clang::TemplateArgument::ArgKind::Pack:
//			// TODO: add support
//			log_->writeError(className_, location, CppImportLogger::Reason::OTHER,
//								  "Unsupported TemplateArgument PACK");
//			return createErrorExpression("Unsupported TemplateArgument PACK");
		default:
			qDebug() << "Unsupported template arg" << templateArg.getKind();
			return {};
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
			unsigned int numArgs = templateSpecialization->getNumArgs();
			if (numArgs == 1)
			{
				auto argument = templateSpecialization->getArg(0);
				return templateArgToString(argument);
			}
			else qDebug() << "Not 1 argument in super template";
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
	} else qDebug() << "Other type" << type->getTypeClassName();
	Q_ASSERT(false);
}
