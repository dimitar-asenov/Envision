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
#include "Exporter.h"
#include "FragmentLayouter.h"
#include "TextToNodeMap.h"
#include "../ExportPlugin.h"
#include "../tree/SourceDir.h"
#include "../ExportException.h"


namespace Export {

std::shared_ptr<TextToNodeMap> Exporter::exportToFileSystem(const QString& pathToProjectContainerDir,
																				SourceDir* projectDir, FragmentLayouter* layouter)
{
	QDir dir{pathToProjectContainerDir};
	if (!dir.exists(projectDir->name()))
		if ( !dir.mkpath(projectDir->name()) )
			throw ExportException("Could not create directory: " + dir.absoluteFilePath(projectDir->name()));

	Q_ASSERT(dir.exists(projectDir->name()));
	dir.cd(projectDir->name());

	if (!(QFileInfo{dir.absolutePath()}).isWritable())
		throw ExportException("Trying to export to a non writable directory : " + dir.absolutePath());

	auto map = std::shared_ptr<TextToNodeMap>(new TextToNodeMap{});
	saveDir(dir, projectDir, layouter, map.get());
	return map;
}

void Exporter::saveDir(QDir& fileSystemDir, SourceDir* sourceDir, FragmentLayouter* layouter, TextToNodeMap* map)
{
	// First remove all directories and files which are no longer part of the project
	auto entries = fileSystemDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDot | QDir::NoDotDot);
	for (auto entry : entries)
	{
		if (entry.isDir() && !sourceDir->hasSubDir(entry.fileName()))
		{
			QDir sub{fileSystemDir.path()};
			sub.cd(entry.fileName());

			log.info("Removing unnecessary directory during export: " + sub.absolutePath());
			if (!sub.removeRecursively())
				throw ExportException("Could not remove directory : " + sub.absolutePath());
		}
		else if (entry.isFile() && !sourceDir->hasFile(entry.fileName()))
		{
			log.info("Removing unnecessary file during export: " + fileSystemDir.absoluteFilePath(entry.fileName()));
			if (!fileSystemDir.remove(entry.fileName()))
				throw ExportException("Could not remove file : " + fileSystemDir.absoluteFilePath(entry.fileName()));
		}
	}

	// Save all files
	for (auto& f : sourceDir->files())
		saveFile(fileSystemDir, &f, layouter, map);

	// Save all directories recursively
	for (auto& d : sourceDir->directories())
	{
		QDir sub{fileSystemDir};
		sub.mkdir(d.name());
		sub.cd(d.name());

		saveDir(sub, &d, layouter, map);
	}
}

void Exporter::saveFile(QDir& fileSystemDir, SourceFile* sourceFile, FragmentLayouter* layouter, TextToNodeMap* map)
{
	QString fileText = layouter->render(sourceFile, map);

	QString fileName = fileSystemDir.absoluteFilePath(sourceFile->name());
	QFile file{fileName};

	// Check to see if a file with the same name and content already exists
	if (fileSystemDir.exists(sourceFile->name()))
	{
		Q_ASSERT(file.exists());

		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			if (fileText == file.readAll()) return;
		}
		else throw ExportException("Could not open file " + fileName + " for reading");

		// File has changed, remove it
		if (!file.remove()) throw ExportException("Could not remove file " + fileName);
	}

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		throw ExportException("Could not open file " + fileName + " for writing");

	auto data = fileText.toUtf8();
	auto written = file.write(data);
	file.close();

	if (written != data.size()) throw ExportException("Error writing to file " + fileName);
}

}
