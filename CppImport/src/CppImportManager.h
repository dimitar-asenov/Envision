/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

namespace Model {
	class TreeManager;
}

namespace CppImport {

/**
 * This is the core manager of the CppImport plugin.
 * To import C++ code you have to specify a path to import with the \a setImportPath method.
 *	After having set the source path you can translate the code with the \a createTreeManager method.
 *
 * To experiment/test you may want to use the \a setupTest method.
 */
class CPPIMPORT_API CppImportManager
{
	public:
		~CppImportManager();
		/**
		 * Use this function to set \a sourcePath which you want to import.
		 * If the project has subproject please specify this with setting \a subProject to true
		 * All subprojects that contain a compile_commands file will be translated
		 */
		void setImportPaths(QStringList sourcePaths, const bool subProjects = false);

		/**
		 * Creates a clang tool and translates the sourcecode to Envision's AST and returns the root
		 * Be sure to have set the sourcePath before calling this method
		 * Set \a statisticsPerProject if you want statistics output for each subproject
		 */
		Model::TreeManager* createTreeManager(const bool statisticsPerProject = false);

		/**
		 * Imports code from a test.cpp file in a subdirectory of ENVISION_ROOT/CppImport/test
		 * Specify the directory in the testSelector file.
		 * For simple tests inside the test directory you do not have to provide a compile_commands file
		 */
		void setupTest();

		/**
		 * The amount of translation units to be imported in total.
		 * Used to display import progress.
		 */
		static int& totalTranslationUnits();
		/**
		 * The amount of translation units that were imported so far.
		 * Used to display import progress.
		 */
		static int& processedTranslationUnits();

	private:
		/**
		 * Prepares everything for the project in \a sourcePath to get imported
		 */
		void initPath(const QString& sourcePath);

		/**
		 * Calculates the shortest subpath of \a subProjectPath containing a CMakeLists.txt file.
		 */
		QString mainProjectPath(const QString& path);

		/**
		 * Collects all source files in \a sourcPath and sub directories and stores them in the sourcesMap_.
		 */
		void readInFiles(const QString& sourcePath);

		/**
		 * Adds the compilation database to compilationDbMap_.
		 */
		void setCompilationDbPath(const QString& sourcePath);

		/**
		 * Creates a compile_commands file inside \a testPath
		 * Use this method only for simple test.cpp files
		 */
		void createCompilationDbForTest(const QString& testPath);

		// File endings filter
		QStringList cppFilter_ = {"*.cpp", "*.cc", "*.cxx"};

		/**
		 * the path to the main CMakeLists.txt of the project to import.
		 */
		QString projectPath_;
		// projects we have to import. For single project this list will only contain one entry
		QStringList projects_;
		QHash<QString, std::vector<std::string>* > sourcesMap_;
		clang::tooling::CompilationDatabase* compilationDb_;

		clang::tooling::CompilationDatabase*findCompilationDatabase(const QString& path);
};

}
