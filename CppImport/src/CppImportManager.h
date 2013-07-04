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

namespace CppImport {

/**
 * This is the core manager of the CppImport plugin.
 * To import C++ code you have to specify a path to import with the \a setImportPath method.
 *	After having set the source path you can translate the code with the \a createModel method.
 *
 * To experiment/test you may want to use the \a setupTest method.
 */
class CPPIMPORT_API CppImportManager
{
	public:
		~CppImportManager();

		/**
		 * Use this function to set \a sourcePath which you want to import.
		 * If the path of the compile_commands.json file differs from the \a sourcePath
		 * please set the \a compilationDbPath
		 *	returns true if succeeded
		 */
		bool setImportPath(const QString& sourcePath, const QString& compilationDbPath = QString());

		/**
		 * Creates a clang tool and translates the sourcecode to Envision's AST and returns the root
		 * Be sure to have set the sourcePath before calling this method
		 */
		Model::Model* createModel();

		/**
		 * Imports code from a test.cpp file in a subdirectory of ENVISION_ROOT/CppImport/test
		 * Specify the directory in the testSelector file.
		 * Note that the general compile_commands.json file is use so you do not need to provide a compilation database.
		 *	returns true if succeeded
		 */
		bool setupTest();

	protected:
		bool setCompilationDbPath(const QString& path);
		clang::CompilerInstance compilerInstance_{};
		clang::tooling::ClangTool* myTool_{};
		clang::tooling::CompilationDatabase* compilationDB_{};
		std::vector<std::string> sources_{};

};

}
