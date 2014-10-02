/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "JavaCompiler.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Node.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/overlays/BoxOverlay.h"
#include "VisualizationBase/src/overlays/OverlayAccessor.h"

#include "JavaExport/src/exporter/JavaExporter.h"

#include "../CommandLineCompiler.h"
#include "../CompilerOutputParser.h"

namespace OODebug {

void JavaCompiler::compileTree(Model::TreeManager* manager, const QString& pathToProjectContainerDirectory)
{
	auto nodeItemMap = Visualization::Item::nodeItemsMap();

	std::shared_ptr<Export::TextToNodeMap> map;
	std::shared_ptr<Export::SourceDir> dir;

	auto exportErrors = JavaExport::JavaExporter::exportTree(manager, pathToProjectContainerDirectory, map, dir);
	// First handle export errors.
	for (auto& error : exportErrors)
	{
		auto node = error.node();
		auto it = nodeItemMap.find(node);
		while (it != nodeItemMap.end() && it.key() == node)
		{
			visualizeMessage(it.value(), node, error.message());
			++it;
		}
	}

	static const QString buildFolder("build");
	QDir buildDir(pathToProjectContainerDirectory + QDir::separator() + buildFolder);
	if (!buildDir.exists())
	{
		QDir projectDir(pathToProjectContainerDirectory);
		Q_ASSERT(projectDir.exists());
		Q_ASSERT(projectDir.mkdir(buildFolder));
	}
	const QStringList buildFolderArgs = {"-d", QString("..") + QDir::separator() + buildFolder};
	CommandLineCompiler compiler("javac", &CompilerOutputParser::parseJavacErrorFormat);

	for (auto file : dir->recursiveFiles())
	{
		Q_ASSERT(file);
		auto filePath = file->path();
		// remove the src prefix
		if (filePath.startsWith(QString("src") + QDir::separator()))
			filePath.replace(0, 4, "");
		auto feedback = compiler.compileFile(pathToProjectContainerDirectory + QDir::separator() + "src",
														 filePath, buildFolderArgs);
		for (auto& message : feedback.messages())
		{
			// In the map we have the src prefix
			auto fileName = message->getFileName();
			if (fileName.startsWith(QString(".") + QDir::separator()))
				fileName.replace(0, 1, "src");
			else
				fileName.prepend(QString("src") + QDir::separator());
			// lines and columns -1 because javac begins at 1 and TextToNodeMap at 0
			Model::Node* node = nullptr;
			if (auto rootMsg = message->getRootMessage())
				node = map->node(fileName, rootMsg->getLineNumber() - 1, rootMsg->getColumnNumber() - 1);
			else
				node = map->node(fileName, message->getLineNumber() - 1, message->getColumnNumber() - 1);
			Q_ASSERT(node);

			auto it = nodeItemMap.find(node);
			while (it != nodeItemMap.end() && it.key() == node)
			{
				// TODO for notes which have a rootMessage add a link to the message location.
				visualizeMessage(it.value(), node, message->getMessage());
				++it;
			}
		}
	}
}

void JavaCompiler::visualizeMessage(Visualization::Item* item, Model::Node* node, const QString& message)
{
	static const QString overlayGroupName("CompilerMessages");

	auto scene = item->scene();
	auto overlayGroup = scene->overlayGroup(overlayGroupName);

	if (!overlayGroup) overlayGroup = scene->addOverlayGroup(overlayGroupName);

	overlayGroup->addOverlay(makeOverlay( new Visualization::BoxOverlay(item,
		[node, message](Visualization::BoxOverlay *){
		return message;
	})));
}

} /* namespace OODebug */
