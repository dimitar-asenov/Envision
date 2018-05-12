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
				std::make_unique<EnvisionPPCallbacks>(compilerInstance_.getSourceManager(), currentFile_, attributes_));
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
	// Only consider enums that are in the current class:
	auto classContext = llvm::dyn_cast<clang::RecordDecl>(enumDecl->getDeclContext());
	if (!classContext ||
		 QString::fromStdString(classContext->getNameAsString()) != currentClassName_)
		return;
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
			QStringList bases = baseClasses(classDecl);
			if (!allowedBases_.contains(bases[0])) return; // We only consider classes which have a base that we allow.

			auto cData = buildClassInfo(classDecl);
			auto typedListIt = std::find_if(classDecl->bases_begin(), classDecl->bases_end(),
				[](const clang::CXXBaseSpecifier& base)
				{
					auto baseName = TypeUtilities::typePtrToString(base.getType().getTypePtr());
					return baseName.contains("TypedList");
				});
			if (typedListIt != classDecl->bases_end())
			{
				// Parse TypedList base class as well, this is usually in the form Reclect<TypedList<T>>
				// Therefore we extract the bases first template parameter and assume that it is the TypedList.
				auto baseDecl = typedListIt->getType().getTypePtr()->getAsCXXRecordDecl();
				if (auto templateClass = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(baseDecl))
				{
					auto typedListDecl = templateClass->getTemplateInstantiationArgs()[0]
							.getAsType().getTypePtr()->getAsCXXRecordDecl();
					APIData::instance().insertClassData(buildClassInfo(typedListDecl), baseClasses(typedListDecl));
				}
			}

			addClassEnums(cData);
			// Add class to api structure
			APIData::instance().addIncludeFile(QString::fromStdString(currentFile_));
			cData.abstract_ = classDecl->isAbstract();
			APIData::instance().insertClassData(cData, bases);
		}
	}
}

void EnvisionAstConsumer::resolveOverloads(ClassData& cData,
														 const QMultiHash<QString, clang::CXXMethodDecl*>& overloads)
{
	auto keys = overloads.uniqueKeys();
	for (const auto& key : keys)
	{
		auto values = overloads.values(key);
		if (values.size() <= 1) continue;
		// Remove non-public and templated methods.
		values.erase(std::remove_if(values.begin(), values.end(), [](const clang::CXXMethodDecl* method) {
			return method->getAccess() != clang::AccessSpecifier::AS_public ||
					(method->getTemplatedKind() != clang::FunctionDecl::TemplatedKind::TK_NonTemplate &&
					 method->getTemplatedKind() != clang::FunctionDecl::TemplatedKind::TK_MemberSpecialization);
		}), values.end());
		if (values.isEmpty()) continue;

		// We want to replace the stored methods in cData thus we first remove everything.
		cData.methods_.erase(
			std::remove_if(cData.methods_.begin(), cData.methods_.end(), [key](const ClassMethod& descriptor) {
				return descriptor.name_ == key;
		}), cData.methods_.end());

		QString functionAddress = QString("&%1::%2").arg(cData.methodAddressPrefix(), key);
		for (int i = 0; i < values.size(); ++i)
		{
			auto method = values[i];
			QString overloadName = QString("%1_%2%3").arg(cData.className_, key, QString::number(i+1));
			QString returnType = TypeUtilities::qualTypeToString(method->getReturnType());
			QStringList argumentTypes;
			for (auto arg : method->parameters())
				argumentTypes.push_back(TypeUtilities::qualTypeToString(arg->getType()));
			QString newFunctionName = QString("*%1").arg(overloadName);
			if (!method->isStatic())
				newFunctionName.prepend("::").prepend(cData.qualifiedName_);
			QString signature = QString("%1 (%2)(%3)").arg(returnType, newFunctionName, argumentTypes.join(", "));
			if (method->isConst()) signature.append(" const");

			cData.overloadAliases_.append({signature, functionAddress});
			if (method->getReturnType().getTypePtr()->isPointerType())
				overloadName = QString("make_function(%1, return_internal_reference<>())").arg(overloadName);
			else if (method->getReturnType().getTypePtr()->isReferenceType())
				overloadName = QString("make_function(%1, return_internal_reference<>())").arg(overloadName);
			cData.methods_.append({key, overloadName});
		}
	}
}

QString EnvisionAstConsumer::functionStringFor(const QString& methodName, const QString& qualifiedClassName,
															  const clang::CXXMethodDecl* method)
{
	QString functionName = QString("&%1::%2").arg(qualifiedClassName, methodName);
	auto returnTypePtr = method->getReturnType().getTypePtr();
	if (returnTypePtr->isReferenceType())
	{
		auto refType = llvm::dyn_cast<clang::ReferenceType>(returnTypePtr);
		auto pointeeType = refType->getPointeeType();
		if (pointeeType.isConstQualified())
		{
			QString pointeeTypeString = TypeUtilities::typePtrToString(pointeeType.getTypePtr());
			QString castString = QString("(const %1& (%2::*)())").arg(pointeeTypeString, qualifiedClassName);
			if (method->isStatic()) castString = "";
			functionName = QString("make_function(%1%2,"
												 " return_value_policy<copy_const_reference>())")
					.arg(castString, functionName);
		}
		else
			functionName = QString("make_function(%1, return_internal_reference<>())").arg(functionName);
	}
	else if (returnTypePtr->isPointerType())
	{
		functionName = QString("make_function(%1, return_internal_reference<>())").arg(functionName);
		checkForTypedList(returnTypePtr);
	}
	return functionName;
}

ClassAttribute EnvisionAstConsumer::attribute(const QString& attributeName, const QString& attributeSetterName,
													const QString& qualifiedClassName, const clang::CXXMethodDecl* method)
{
	QString getter = functionStringFor(attributeName, qualifiedClassName, method);
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

ClassData EnvisionAstConsumer::buildClassInfo(clang::CXXRecordDecl* classDecl)
{
	auto className = QString::fromStdString(classDecl->getNameAsString());
	auto qualifiedName = QString::fromStdString(classDecl->getQualifiedNameAsString());

	bool isTemplate = false;
	if (auto templatedDecl = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(classDecl))
	{
		auto argDecl = templatedDecl->getTemplateInstantiationArgs()[0].getAsType().getTypePtr()->getAsCXXRecordDecl();
		auto argName = QString::fromStdString(argDecl->getNameAsString());
		auto qualifiedArg = QString::fromStdString(argDecl->getQualifiedNameAsString());
		qualifiedName.append(QString("<%1>").arg(qualifiedArg));
		className.append("Of" + argName);
		isTemplate = true;
	}

	ClassData cData{className, qualifiedName};
	if (isTemplate)
		cData.usingAlias_ = className;

	addBases(cData, classDecl);

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
		else if (methodName.size() > 3 && methodName.startsWith("set"))
		{
			possibleAttributeSetters << methodName;
		}
	}
	QMultiHash<QString, clang::CXXMethodDecl*> overloads;
	// check if we have some more attributes which don't have an attribute macro:
	for (auto method : classDecl->methods())
	{
		// Ignore de-/constructors & deleted methods & overloaded operators:
		if (llvm::isa<clang::CXXConstructorDecl>(method) || llvm::isa<clang::CXXDestructorDecl>(method)) continue;
		if (method->isDeleted()) continue;
		if (method->isOverloadedOperator()) continue;

		QString methodName = QString::fromStdString(method->getNameAsString());

		// Ignore registerNodeType, it does not make sense to have this function in Python. Furthermore it takes function
		// pointers as arguments which does not make sense in Python.
		// TODO: in general we should exclude functions that take functions pointers and lambdas.
		// A check would have to check all arguments (maybe also return type) for function pointer type.
		// Note that they might be hidden behind a typedef type.
		if (methodName == "registerNodeType") continue;

		// Ignore findSymbols as python doesn't seem to like the unique_ptr.
		if (methodName == "findSymbols") continue;

		if (method->getAccess() != clang::AccessSpecifier::AS_public ||
			 (method->getTemplatedKind() != clang::FunctionDecl::TemplatedKind::TK_NonTemplate &&
			  method->getTemplatedKind() != clang::FunctionDecl::TemplatedKind::TK_MemberSpecialization) ||
			 // workaround since we only consider cpp files at the moment:
			 methodName == "firstAncestorOfType")
		{
			// For wrapping methods we should still consider non public and template methods for overload resolution.
			overloads.insertMulti(methodName, method);
			continue;
		}
		if (seenMethods.contains(methodName)) continue;

		if (TypeUtilities::typePtrToString(method->getReturnType().getTypePtr()).contains("iterator")) continue;

		if (possibleAttributeSetters.contains(methodName)) continue;
		bool usedAsAttribute = false;
		for (const QString& setterName : possibleAttributeSetters)
		{
			QString possibleGetterName = setterName.mid(4); // drop set and first letter
			possibleGetterName.prepend(setterName[3].toLower());
			if (methodName == possibleGetterName)
			{
				// Found another attribute:
				cData.attributes_.append(attribute(methodName, setterName, cData.qualifiedName_, method));
				seenMethods << setterName << methodName;
				usedAsAttribute = true;
				break;
			}
		}
		if (!usedAsAttribute)
		{
			// found a free standing method to export.
			cData.methods_.append({methodName, functionStringFor(methodName, cData.methodAddressPrefix(), method),
										  method->isStatic()});
			overloads.insertMulti(methodName, method);
		}
	}
	resolveOverloads(cData, overloads);
	return cData;
}

void EnvisionAstConsumer::checkForTypedList(const clang::Type* type)
{
	QString fullName = TypeUtilities::typePtrToString(type);
	if (fullName.contains("TypedList"))
	{
		static QRegularExpression typedListMatcher("([^<]+)<([\\w<>:]+)>");
		auto match = typedListMatcher.match(fullName);
		if (match.hasMatch())
		{
			QString itemType = match.captured(2);
			APIData::instance().insertTypeList(itemType);
		}
	}
}

void EnvisionAstConsumer::addClassEnums(ClassData& cData)
{
	for (const auto& pEnum : processedEnums_)
	{
		QStringList names = pEnum.qualifiedName_.split("::");
		if (names[names.size()-2] == cData.className_)
			cData.enums_ << pEnum;
	}
	processedEnums_.clear();
}

void EnvisionAstConsumer::addBases(ClassData& cData, const clang::CXXRecordDecl* classDecl)
{
	for (auto base : classDecl->bases())
		cData.baseClasses_ << TypeUtilities::typePtrToString(base.getType().getTypePtr());
}
