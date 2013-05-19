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

#pragma once

#include "cppimport_api.h"
#include "CppImportUtilities.h"

namespace CppImport {

class TranslateManager
{
	public:
		TranslateManager(Model::Model* model, OOModel::Project* project, CppImportUtilities* utils);
		OOModel::Module* insertNamespace(clang::NamespaceDecl* nd, int depth);
		bool insertClass(clang::CXXRecordDecl* rDecl, OOModel::Class* ooClass);
		bool containsClass(clang::CXXRecordDecl* recordDecl);
		OOModel::Method* insertMethodDecl(clang::CXXMethodDecl* mDecl, OOModel::Method::MethodKind kind);
		OOModel::Method* insertFunctionDecl(clang::FunctionDecl* functionDecl);
		OOModel::Field* insertField(clang::FieldDecl* fDecl);
		OOModel::VariableDeclaration* insertVar(clang::VarDecl* vDecl);
		OOModel::VariableDeclaration* getVar(clang::VarDecl* vDecl);

	private:
		OOModel::Method* addNewMethod(clang::CXXMethodDecl* mDecl, OOModel::Method::MethodKind kind);
		OOModel::Method* addNewFunction(clang::FunctionDecl* functionDecl);

		Model::Model* model_{};
		OOModel::Project* project_{};

		QMap<clang::NamespaceDecl*, QPair<OOModel::Module*,int> > nameSpaceMap_;
		QMap<clang::CXXRecordDecl*, OOModel::Class*> classMap_;
		QMap<clang::CXXMethodDecl*, OOModel::Method*> methodMap_;
		QMap<clang::FunctionDecl*, OOModel::Method*> functionMap_;
		QMap<clang::VarDecl*, OOModel::VariableDeclaration*> varMap_;

		CppImportUtilities* utils_{};
};

inline bool TranslateManager::containsClass(clang::CXXRecordDecl* recordDecl)
{
	return classMap_.contains(recordDecl);
}

}
