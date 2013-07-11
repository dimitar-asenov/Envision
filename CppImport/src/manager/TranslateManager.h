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
#include "../CppImportUtilities.h"
#include "NodeHasher.h"

namespace CppImport {

/**
 * This class is to manage the whole translation process.
 * It keeps track of nodes already translated and completes them if needed.
 */
class CPPIMPORT_API TranslateManager
{
	public:
		TranslateManager(CppImportUtilities* utils, OOModel::Project* root);
		~TranslateManager();
		void setSourceManager(const clang::SourceManager* mngr);
		OOModel::Module* insertNamespace(clang::NamespaceDecl* namespaceDecl);

		/**
		 * Inserts the class \a ooClass to the managed class if it is not yet managed.
		 * Returns true if the class is not yet managed (insert success) else false
		 */
		bool insertClass(clang::CXXRecordDecl* rDecl, OOModel::Class* ooClass);
		/**
		 * Inserts the class \a ooClass to the managed class if it is not yet managed.
		 * Returns true if the class is not yet managed (insert success) else false
		 */
		bool insertClassTemplate(clang::ClassTemplateDecl* classTemplate, OOModel::Class* ooClass);
		/**
		 * Inserts the class \a ooClass to the managed class if it is not yet managed.
		 * Returns true if the class is not yet managed (insert success) else false
		 */
		bool insertClassTemplateSpec(clang::ClassTemplateSpecializationDecl* classTemplate, OOModel::Class* ooClass);
		bool containsClass(clang::CXXRecordDecl* recordDecl);
		/**
		 * Inserts the method \a mDecl to the manager if not yet managed.
		 * If the method is already managed the arguments may be completed.
		 * Returns either the new method created or the existing one.
		 */
		OOModel::Method* insertMethodDecl(clang::CXXMethodDecl* mDecl, OOModel::Method::MethodKind kind);
		/**
		 * Inserts the method \a mDecl to the manager if not yet managed.
		 * If the method is already managed the arguments may be completed.
		 * Returns either the new method created or the existing one.
		 */
		OOModel::Method* insertFunctionDecl(clang::FunctionDecl* functionDecl);
		/**
		 * Inserts the field \a fieldDecl to the manager and into the correct class.
		 * If the parent is not yet visited this method returns a nullptr.
		 *
		 */
		OOModel::Field* insertField(clang::FieldDecl* fieldDecl);

		/**
		 * Inserts the static field \a varDecl to the manager and into the correct class.
		 * The parameter \a wasDeclared will be set to true if the declaration of this field was visited
		 * (only need to visit initializer now).
		 * The caller has to make sure that this is a static field.
		 * If the parent is not yet visited this method returns a nullptr.
		 * Note that clang treats a static field as a VarDecl
		 */
		OOModel::Field* insertStaticField(clang::VarDecl* varDecl, bool& wasDeclared);

	private:
		OOModel::Method* addNewMethod(clang::CXXMethodDecl* mDecl, OOModel::Method::MethodKind kind);
		OOModel::Method* addNewFunction(clang::FunctionDecl* functionDecl);

		QMap<QString, OOModel::Module*> nameSpaceMap_;
		QMap<QString, OOModel::Class*> classMap_;
		QMap<QString, OOModel::Method*> methodMap_;
		QMap<QString, OOModel::Method*> functionMap_;
		QMap<QString, OOModel::Field*> staticFieldMap_;

		CppImportUtilities* utils_{};
		OOModel::Project* rootProject_{};
		NodeHasher* nh_{new NodeHasher()};
};

}
