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

#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QHash>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/AST/ASTConsumer.h>

namespace clang {
	class EnumDecl;
	class Type;
}

struct ClassData;
struct EnumData;
struct ClassAttribute;

class EnvisionAstConsumer : public clang::ASTConsumer
{
	public:
		EnvisionAstConsumer(clang::CompilerInstance& ci, QString currentFile);

		virtual void Initialize(clang::ASTContext& Context) override;
		virtual void HandleTagDeclDefinition(clang::TagDecl* tagDecl) override;

		void HandleEnumDecl(clang::EnumDecl* enumDecl);
		void HandleClassDecl(clang::CXXRecordDecl* classDecl);

	private:
		clang::CompilerInstance& compilerInstance_;
		QString currentClassName_;
		std::string currentFile_;

		QHash<QString,QString> attributes_;
		QHash<QString,QString> privateAttributes_;

		QSet<clang::TagDecl*> seenDecls_;
		QList<EnumData> processedEnums_;

		// All imported nodes have to inherit from Model::Node
		QStringList allowedBases_{"Model::Node"};

		void resolveOverloads(ClassData& cData, const QMultiHash<QString, clang::CXXMethodDecl*>& overloads);

		QString functionStringFor(const QString& methodName, const QString& qualifiedClassName,
										  const clang::CXXMethodDecl* method);
		ClassAttribute attribute(const QString& attributeName, const QString& attributeSetterName,
								const QString& qualifiedClassName, const clang::CXXMethodDecl* method);

		QStringList baseClasses(clang::CXXRecordDecl* classDecl);

		void checkForTypedList(const clang::Type* type);
};
