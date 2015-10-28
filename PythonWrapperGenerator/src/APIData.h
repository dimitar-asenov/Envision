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

#pragma once

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtCore/QDir>

struct EnumData
{
		EnumData(QString name, QString qualifiedName) : enumName_{name}, qualifiedName_{qualifiedName} {}
		QString enumName_;
		QString qualifiedName_;
		QList<QPair<QString, QString>> values_;
};

struct ClassAttribute
{
		QString name_;
		QString getterQualified_;
		QString setterQualified_;
};

struct ClassMethod
{
		QString name_;
		QString wrappedFunctionPointer_;
		bool static_{false};
};

struct OverloadDescriptor
{
		QString signature_;
		QString functionAddress_;
};

struct ClassData
{
		ClassData() = default;
		ClassData(QString name, QString qualifiedName) : className_{name}, qualifiedName_{qualifiedName} {}
		QString className_;
		QString qualifiedName_;
		QStringList baseClasses_;
		QList<OverloadDescriptor> overloadAliases_;
		QList<ClassAttribute> attributes_;
		QList<ClassMethod> methods_;
		QList<EnumData> enums_;
		bool abstract_{false};
};

struct ClassDataNode;

struct APIData
{
		QString includePrefix_;
		QString namespaceName_;
		QStringList includePaths_;

		static APIData& instance();

		void addIncludeFile(QString filePath);

		void insertClassData(ClassData data, QStringList classHierarchy);

		void insertTypeList(QString itemType);

		QList<ClassData> classes() const;

		QHash<QString, QString> typedLists() const;
	private:
		APIData() = default;

		ClassDataNode* classRoot_{};
		QHash<QString, QString> typedLists_;
};
