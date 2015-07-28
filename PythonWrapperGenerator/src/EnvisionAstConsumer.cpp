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

#include "EnvisionAstConsumer.h"

#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QRegularExpression>

#include <clang/AST/DeclCXX.h>

#include "APIData.h"
#include "EnvisionPPCallbacks.h"
#include "TypeUtilities.h"

EnvisionAstConsumer::EnvisionAstConsumer(clang::CompilerInstance& ci, QString currentFile)
	: compilerInstance_{ci}
{
	currentFile.replace(".cpp", ".h");
	currentFile_ = currentFile.toStdString();
	currentClassName_ = currentFile.split(QDir::separator()).last();
	currentClassName_ = currentClassName_.split(".").first();
}

void EnvisionAstConsumer::Initialize(clang::ASTContext&)
{
	compilerInstance_.getPreprocessor().addPPCallbacks(
				std::make_unique<EnvisionPPCallbacks>(compilerInstance_.getSourceManager(),
																  currentFile_, attributes_, privateAttributes_));
	compilerInstance_.getPreprocessor().enableIncrementalProcessing();
}

void EnvisionAstConsumer::HandleTagDeclDefinition(clang::TagDecl* tagDecl)
{
	if (auto classDecl = llvm::dyn_cast<clang::CXXRecordDecl>(tagDecl))
		HandleClassDecl(classDecl);
	else if (auto enumDecl = llvm::dyn_cast<clang::EnumDecl>(tagDecl))
		HandleEnumDecl(enumDecl);
	else
		Q_ASSERT(false); // There should not be any other decl here, check clang documentation.
}

void EnvisionAstConsumer::HandleEnumDecl(clang::EnumDecl* enumDecl)
{
	// Ignore private enums:
	if (enumDecl->getAccess() == clang::AccessSpecifier::AS_private) return;

	QString enumName = QString::fromStdString(enumDecl->getNameAsString());
	QString qualifiedName = QString::fromStdString(enumDecl->getQualifiedNameAsString());;
	EnumData eData{enumName, qualifiedName};

	for (auto enumConstant : enumDecl->enumerators())
	{
		QString constantName = QString::fromStdString(enumConstant->getNameAsString());
		eData.values_.append({constantName, QString("%1::%2").arg(eData.qualifiedName_, constantName)});
	}

	processedEnums_ << eData;
}

void EnvisionAstConsumer::HandleClassDecl(clang::CXXRecordDecl* classDecl)
{
	auto context = classDecl->getDeclContext();
	if (context->isNamespace())
	{
		auto namespaceDecl = llvm::dyn_cast<clang::NamespaceDecl>(context);
		auto namespaceName = QString::fromStdString(namespaceDecl->getNameAsString());
		auto className = QString::fromStdString(classDecl->getNameAsString());
		if (namespaceName == APIData::instance().namespaceName_ && className == currentClassName_)
		{
			// FIXME: find a better solution for statementitemlist, currently ignore it (because we don't wrap TypedList).
			if (className == "StatementItemList") return;

			QStringList bases = baseClasses(classDecl);
			if (!allowedBases_.contains(bases[0])) return; // We only consider classes which have a base that we allow.

			ClassData cData{className, QString::fromStdString(classDecl->getQualifiedNameAsString())};

			for (auto base : classDecl->bases())
				cData.baseClasses_ << TypeUtilities::typePtrToString(base.getType().getTypePtr());

			QSet<QString> seenMethods;
			QSet<QString> possibleAttributeSetters;
			for (auto method : classDecl->methods())
			{
				QString methodName = QString::fromStdString(method->getNameAsString());
				if (seenMethods.contains(methodName)) continue;

				auto it = attributes_.find(methodName);
				if (it != attributes_.end())
				{
					cData.attributes_.append(attribute(it.key(), it.value(), cData.qualifiedName_, method));
					seenMethods << it.key() << it.value();
				}
				else
				{
					// ignore private attributes:
					auto privateIt = privateAttributes_.find(methodName);
					if (privateIt != privateAttributes_.end())
						seenMethods << methodName << privateIt.value();
					else if (methodName.startsWith("set"))
						possibleAttributeSetters << methodName;
				}
			}
			// check if we have some more attributes which don't have an attribute macro:
			for (auto method : classDecl->methods())
			{
				// Ignore private methods:
				if (method->getAccess() == clang::AccessSpecifier::AS_private) continue;

				QString methodName = QString::fromStdString(method->getNameAsString());
				if (seenMethods.contains(methodName)) continue;

				if (possibleAttributeSetters.contains(methodName)) continue;
				for (QString setterName : possibleAttributeSetters)
				{
					QString possibleGetterName = setterName.mid(4); // drop set and first letter
					possibleGetterName.prepend(setterName[3].toLower());
					if (methodName == possibleGetterName)
					{
						// Found another attribute:
						cData.attributes_.append(attribute(methodName, setterName, cData.qualifiedName_, method));
						seenMethods << setterName << methodName;
						break;
					}
				}
			}
			// Add enums which are declared in this class:
			for (auto pEnum : processedEnums_)
			{
				QStringList names = pEnum.qualifiedName_.split("::");
				if (names[names.size()-2] == className)
					cData.enums_ << pEnum;
			}
			processedEnums_.clear();
			// Add class to api structure
			APIData::instance().addIncludeFile(QString::fromStdString(currentFile_));
			cData.abstract_ = classDecl->isAbstract();
			APIData::instance().insertClassData(cData, bases);
		}
	}
}

ClassAttribute EnvisionAstConsumer::attribute(const QString& attributeName, const QString& attributeSetterName,
													const QString& qualifiedClassName, const clang::CXXMethodDecl* method)
{
	QString getter = QString("&%1::%2").arg(qualifiedClassName, attributeName);
	auto returnTypePtr = method->getReturnType().getTypePtr();
	if (returnTypePtr->isReferenceType())
	{
		auto refType = llvm::dyn_cast<clang::ReferenceType>(returnTypePtr);
		auto pointeeType = refType->getPointeeType();
		if (pointeeType.isConstQualified())
		{
			QString pointeeTypeString = TypeUtilities::typePtrToString(pointeeType.getTypePtr());
			QString newGetter = QString("make_function((const %1& (%2::*)())%3,"
												 " return_value_policy<copy_const_reference>())")
					.arg(pointeeTypeString, qualifiedClassName, getter);
			getter = newGetter;
		}
	}
	else if (returnTypePtr->isPointerType())
	{
		getter = QString("make_function(%1, return_internal_reference<>())").arg(getter);
		checkForTypedList(returnTypePtr);
	}
	QString setter = QString("&%1::%2").arg(qualifiedClassName, attributeSetterName);
	return {attributeName, getter, setter};
}

QStringList EnvisionAstConsumer::baseClasses(clang::CXXRecordDecl* classDecl)
{
	QStringList result {};
	for (auto base : classDecl->bases())
		result << baseClasses(base.getType()->getAsCXXRecordDecl());
	result << QString::fromStdString(classDecl->getQualifiedNameAsString());
	result.removeAll("Core::Reflect");
	return result;
}

void EnvisionAstConsumer::checkForTypedList(const clang::Type* type)
{
	QString fullName = TypeUtilities::typePtrToString(type);
	if (fullName.contains("TypedList"))
	{
		static QRegularExpression typedListMatcher("([^<]+)<([\\w<>:]+)>");
		auto match = typedListMatcher.match(fullName);
		Q_ASSERT(match.hasMatch());
		QString itemType = match.captured(2);
		APIData::instance().insertTypeList(itemType);
	}
}
