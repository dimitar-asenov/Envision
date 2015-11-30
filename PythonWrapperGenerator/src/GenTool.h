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
#include "precompiled.h"

class GenTool
{
	public:
		void run(const QString& envisionPath);

	private:
		/**
		 * Prepares importing Envision which is structured in subdirectories.
		 * Returns the compilation database for the whole project.
		 */
		std::unique_ptr<clang::tooling::CompilationDatabase> setSubDirPath(const QString& path);

		/**
		 * Prepares everything for the project in \a sourcePath to get imported
		 */
		void initPath(const QString& sourcePath);

		/**
		 * Collects all source files in \a sourcPath and sub directories and stores them in the sourcesMap_.
		 */
		void readInFiles(const QString& sourcePath);

		// File endings filter
		QStringList cppFilter_ = {"*.cpp", "*.cc", "*.cxx"};

		// the project name which is shown in Envision
		// we set this to the innermost directory of the sourcepath
		QString projectName_;
		// projects we have to import. For single project this list will only contain one entry
		QStringList projects_;
		QHash<QString, std::vector<std::string>> sourcesMap_;
};
