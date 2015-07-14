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

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QStringList>

#include <clang/AST/DeclCXX.h>

#include "EnvisionPPCallbacks.h"
#include "TypeUtilities.h"

EnvisionAstConsumer::EnvisionAstConsumer(clang::CompilerInstance& ci, QString currentFile, APIData& outData)
	: compilerInstance_{ci}, outData_{outData}
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
	// We only care about stuff in the header file of the current translation unit source file.
	auto &sourceManager = compilerInstance_.getSourceManager();
	auto sourceLocation = tagDecl->getSourceRange().getBegin();
	if (sourceManager.getFilename(sourceLocation) != currentFile_) return;

	if (auto classDecl = llvm::dyn_cast<clang::CXXRecordDecl>(tagDecl))
		HandleClassDecl(classDecl);
	else if (auto enumDecl = llvm::dyn_cast<clang::EnumDecl>(tagDecl))
		HandleEnumDecl(enumDecl);
	else
		qDebug() << "Uknown Decl";
}

void EnvisionAstConsumer::HandleEnumDecl(clang::EnumDecl* enumDecl)
{
	QString enumName = QString::fromStdString(enumDecl->getNameAsString());
	QString fullEnumName = enumName;
	auto context = enumDecl->getDeclContext();
	if (auto recordDecl = llvm::dyn_cast<clang::CXXRecordDecl>(context))
	{
		QString recordName = QString::fromStdString(recordDecl->getNameAsString());
		fullEnumName.prepend("::").prepend(recordName);

		auto recordContext = recordDecl->getDeclContext();
		if (auto namespaceDecl = llvm::dyn_cast<clang::NamespaceDecl>(recordContext))
		{
			QString namespaceName = QString::fromStdString(namespaceDecl->getNameAsString());
			fullEnumName.prepend("::").prepend(namespaceName);
		}
	}
	EnumData eData(enumName);
	eData.qualifiedName_ = fullEnumName;

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
		if (namespaceName == outData_.includePrefix_ && className == currentClassName_)
		{
			ClassData cData(className);
			cData.qualifiedName_ = QString("%1::%2").arg(namespaceName, className);
			for (auto base : classDecl->bases())
			{
				auto baseString = TypeUtilities::typePtrToString(base.getType().getTypePtr());
				if (!baseString.isEmpty())
				{
					if (!baseString.contains("::"))
						cData.baseClasses_ << QString("%1::%2").arg(namespaceName, baseString);
					else
						cData.baseClasses_ << baseString;
				}
			}
			QSet<QString> seenMethods;
			QSet<QString> possibleAttributeSetters;
			for (auto method : classDecl->methods())
			{
				QString methodName = QString::fromStdString(method->getNameAsString());
				if (seenMethods.contains(methodName)) continue;

				auto it = attributes_.find(methodName);
				if (it != attributes_.end())
				{
					QString attributeName = it.key();
					QString getter = QString("&%1::%2").arg(cData.qualifiedName_, attributeName);
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
									.arg(pointeeTypeString, cData.qualifiedName_, getter);
							getter = newGetter;
						}
					}
					QString setter = QString("&%1::%2").arg(cData.qualifiedName_, it.value());
					cData.attributes_.append({attributeName, getter, setter});
					seenMethods << it.key() << it.value();
				}
				else
				{
					// ignore private attributes:
					auto privateIt = privateAttributes_.find(methodName);
					if (privateIt != privateAttributes_.end())
					{
						seenMethods << methodName << privateIt.value();
					}
					else if (methodName.startsWith("set"))
					{
						possibleAttributeSetters << methodName;
					}
				}
			}
			// check if we have some more attributes which don't have an attribute macro:
			for (auto method : classDecl->methods())
			{
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
						QString attributeName = methodName;
						QString getter = QString("%1::%2").arg(cData.qualifiedName_, attributeName);
						QString setter = QString("%1::%2").arg(cData.qualifiedName_, setterName);
						cData.attributes_.append({attributeName, getter, setter});
						seenMethods << setterName << methodName;
						break;
					}
				}
			}
			// Add enums
			cData.enums_ << processedEnums_;
			processedEnums_.clear();
			// Add class to api structure
			outData_.addIncludeFile(QString::fromStdString(currentFile_));
			outData_.classes_ << cData;
		}
	}
}
