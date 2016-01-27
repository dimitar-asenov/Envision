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

const QString Exporter::previousExportsFileName_{"envision_exported_files"};

std::shared_ptr<TextToNodeMap> Exporter::exportToFileSystem(const QString& pathToProjectContainerDir,
																				SourceDir* exportTree, FragmentLayouter* layouter,
																				ExportSpan span)
{
	Exporter e{QDir{pathToProjectContainerDir}, exportTree, layouter, span};
	return e.map_;
}

Exporter::Exporter(QDir projectDirOnFilesystem, SourceDir* exportTree, FragmentLayouter* layouter,
	ExportSpan span) : rootDir_{projectDirOnFilesystem}, span_{span},
	layouter_{layouter}, map_{std::shared_ptr<TextToNodeMap>(new TextToNodeMap{})}
{
	if (!rootDir_.exists(exportTree->name()))
	{
		if (span_ == ExportSpan::SomeFiles)
			throw ExportException("You cannot export only a part of this project. You must export the entire project "
										 "in the new directory: "
										 + rootDir_.absoluteFilePath(exportTree->name()));

		if ( !rootDir_.mkpath(exportTree->name()) )
			throw ExportException("Could not create directory: " + rootDir_.absoluteFilePath(exportTree->name()));
	}

	Q_ASSERT(rootDir_.exists(exportTree->name()));
	rootDir_.cd(exportTree->name());

	if (!(QFileInfo{rootDir_.absolutePath()}).isWritable())
		throw ExportException("Trying to export to a non writable directory : " + rootDir_.absolutePath());

	readPreviousExports();
	saveDir(rootDir_, exportTree);
	deleteObsoletePreviousExports();
	saveCurrentExports();
}

void Exporter::saveDir(QDir& fileSystemDir, SourceDir* sourceDir)
{
	// Remember that we exported this directory
	currentExports_.insert(fileSystemDir.absolutePath());

	// Save all files
	for (auto& f : sourceDir->files())
		saveFile(fileSystemDir, &f);

	// Save all directories recursively
	for (auto& d : sourceDir->directories())
	{
		QDir sub{fileSystemDir};
		sub.mkdir(d.name());
		sub.cd(d.name());

		saveDir(sub, &d);
	}
}

void Exporter::saveFile(QDir& fileSystemDir, SourceFile* sourceFile)
{
	// Remember that we exported this file
	QString fileName = fileSystemDir.absoluteFilePath(sourceFile->name());
	currentExports_.insert(fileName);

	QString fileText = layouter_->render(sourceFile, map_.get());
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

void Exporter::readPreviousExports()
{
	QFile exportsFile{rootDir_.absoluteFilePath(previousExportsFileName_)};
	if (!exportsFile.exists())
	{
		if (span_ == ExportSpan::SomeFiles)
			throw ExportException("No exports file was found in " + rootDir_.absolutePath()
									 + " Please export the entire project before doing a partial export.");

		// Otherwise simply return, there is no need to read anything.
		return;
	}

	if (!exportsFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throw ExportException("Could not open previous exports file in: "
									 + rootDir_.absoluteFilePath(previousExportsFileName_));

	QTextStream textStream(&exportsFile);
	while (true)
	{
		 QString line = textStream.readLine();
		 if (line.isNull())
			  break;
		 else if (!line.isEmpty())
			 previousExports_.insert(line);
	}
}

void Exporter::deleteObsoletePreviousExports()
{
	if (span_ != ExportSpan::AllFiles) return;

	// Remove from the list all previous exports that are also current exports
	for (const auto& current : currentExports_)
		previousExports_.erase(current);

	// Whatever is left needs to be *carefully* deleted:
	// Delete all files
	// Delete any remaining *empty* directories

	// Delete all files
	auto it = previousExports_.begin();
	while (it != previousExports_.end())
	{
		QFileInfo file(*it);
		if (file.isFile())
		{
			log.info("Removing unnecessary file during export: " + file.absoluteFilePath());
			if (!QFile(*it).remove())
				throw ExportException("Could not remove previously generated file: " + file.absoluteFilePath());

			it = previousExports_.erase(it);
		}
		else ++it;
	}

	// Now, delete empty directories
	it = previousExports_.begin();
	while (it != previousExports_.end())
	{
		QDir dir{*it};
		if (dir.entryInfoList(QDir::NoDot | QDir::NoDotDot | QDir::AllEntries).isEmpty())
		{
			log.info("Removing unnecessary directory during export: " + dir.absolutePath());
			if (!dir.removeRecursively())
				throw ExportException("Could not remove directory : " + dir.absolutePath());

			it = previousExports_.erase(it);
		}
		else ++it;
	}
}

void Exporter::saveCurrentExports()
{
	// If this was a partial save, compute a full list of exports first
	if (span_ == ExportSpan::SomeFiles)
		currentExports_.insert(previousExports_.begin(), previousExports_.end());

	// currentExports_ shold now contain all generated files
	QFile exportsFile{rootDir_.absoluteFilePath(previousExportsFileName_)};

	if (!exportsFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw ExportException("Could not save exports file : " + rootDir_.absoluteFilePath(previousExportsFileName_));

	QTextStream textStream(&exportsFile);
	for (auto exportedFile : currentExports_)
		textStream  << exportedFile << '\n';

}

}
