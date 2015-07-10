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

#include <clang/AST/DeclCXX.h>

void EnvisionAstConsumer::HandleTagDeclDefinition(clang::TagDecl* tagDecl)
{
//	auto &sourceManager = compilerInstance_.getSourceManager();
//	auto sourceLocation = tagDecl->getSourceRange().getBegin();
//	sourceLocation = sourceManager.getExpansionLoc(sourceLocation);
//	if (sourceManager.getFileID(sourceLocation) != sourceManager.getMainFileID()) return;

	auto classDecl = llvm::dyn_cast<clang::CXXRecordDecl>(tagDecl);
	if (!classDecl) return;

	auto context = classDecl->getDeclContext();
	if (context->isNamespace())
	{
		auto namespaceDecl = llvm::dyn_cast<clang::NamespaceDecl>(context);
		auto namespaceName = QString::fromStdString(namespaceDecl->getNameAsString());
		if (namespaceName == "OOModel")
		{
			if (auto classDef = classDecl->getDefinition())
			{
				if (seenDecls_.contains(classDef)) qDebug() << "seen";
				else
				{
					qDebug() << "ClassDecl:" << QString::fromStdString(classDecl->getNameAsString());
					seenDecls_.insert(classDef);
				}
			}
		}
	}
}

