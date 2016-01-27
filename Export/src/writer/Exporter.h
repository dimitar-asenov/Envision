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

#include "../export_api.h"

namespace Export {

class SourceDir;
class SourceFile;
class FragmentLayouter;
class TextToNodeMap;

class EXPORT_API Exporter {
	public:
		enum class ExportSpan : int {AllFiles,		/**< Use this option to indicate that all files are exported. If
																there is an existing envision_exported_files file, it will be used
																to match the newly exported file names to the old ones and any
																obsolete filenames from previous exports will be deleted. */
												SomeFiles	/**< Use this option to indicate that only some files are exported.
																New files will be added to the list of exported files in the
																envision_exported_files file but no existing files will be deleted
																(only overwritten if needed). */};

		static std::shared_ptr<TextToNodeMap> exportToFileSystem(const QString& pathToProjectContainerDir,
																					SourceDir* exportTree, FragmentLayouter* layouter,
																					ExportSpan span);

	private:
		Exporter(QDir projectDirOnFilesystem, SourceDir* exportTree, FragmentLayouter* layouter,
					ExportSpan span);

		void saveDir(QDir& fileSystemDir, SourceDir* sourceDir);
		void saveFile(QDir& fileSystemDir, SourceFile* sourceFile);

		void readPreviousExports();
		void deleteObsoletePreviousExports();
		void saveCurrentExports();

		QDir rootDir_;
		ExportSpan span_{};
		FragmentLayouter* layouter_{};

		std::shared_ptr<TextToNodeMap> map_;

		std::set<QString> previousExports_;
		std::set<QString> currentExports_;

		static const QString previousExportsFileName_;
};

}
