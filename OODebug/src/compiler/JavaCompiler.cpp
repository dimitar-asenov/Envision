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

#include "InteractionBase/src/commands/CommandResult.h"
#include "InteractionBase/src/commands/CommandError.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Node.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/overlays/MessageOverlay.h"
#include "VisualizationBase/src/overlays/OverlayAccessor.h"

#include "OOModel/src/declarations/Project.h"

#include "JavaExport/src/exporter/JavaExporter.h"

#include "CommandLineCompiler.h"
#include "CompilerOutputParser.h"

namespace OODebug {

const QString JavaCompiler::COMPILER_MESSAGE_GROUP{"CompilerMessages"};

Interaction::CommandResult* JavaCompiler::compileTree(Model::TreeManager* manager,
																		const QString& pathToProjectContainerDirectory,
																		bool includeDebugSymbols)
{
	bool compilationOk = true;
	// Remove previous error messages
	for (auto scene : Visualization::Scene::allScenes())
	{
		Q_ASSERT(scene);
		scene->removeOverlayGroup(COMPILER_MESSAGE_GROUP);
	}

	auto project = DCast<OOModel::Project>(manager->root());
	Q_ASSERT(project);

	auto nodeItemMap = Visualization::Item::nodeItemsMap();

	// Export the project first if we need to
	std::shared_ptr<Export::TextToNodeMap> map;
	if (JavaExport::JavaExporter::exportMaps().storedRevision(project) == project->revision())
	{
		map = JavaExport::JavaExporter::exportMaps().map(project);
	}
	else
	{
		auto exportErrors = JavaExport::JavaExporter::exportTree(manager, pathToProjectContainerDirectory);
		// If we have export errors compilation is not okay:
		if (exportErrors.size()) compilationOk = false;

		// Handle export errors.
		for (auto& error : exportErrors)
		{
			auto node = error.node();
			auto it = nodeItemMap.find(node);
			while (it != nodeItemMap.end() && it.key() == node)
			{
				visualizeMessage(it.value(), error.message());
				++it;
			}
		}
		map = JavaExport::JavaExporter::exportMaps().map(project);
	}
	Q_ASSERT(map);

	if (!compilationOk) // If export failed we can exit here.
		return new Interaction::CommandResult{new Interaction::CommandError{"Export failed, check error messages!"}};

	// Create a build folder and setup the compiler
	static const QString buildFolder{"build"};
	QDir buildDir{pathToProjectContainerDirectory + QDir::separator() + buildFolder};
	if (!buildDir.exists())
	{
		QDir projectDir{pathToProjectContainerDirectory};
		Q_ASSERT(projectDir.exists());
		Q_ASSERT(projectDir.mkdir(buildFolder));
	}
	QStringList args = {"-d", QString{".."} + QDir::separator() + buildFolder};
	if (includeDebugSymbols) args << "-g";
	CommandLineCompiler compiler{"javac", &CompilerOutputParser::parseJavacErrorFormat};

	QSet<uint> seenMessages;

	// Compile each file and show the messages received from it
	for (auto file : map->files())
	{
		// remove the src prefix
		if (file.startsWith(QString{"src"} + QDir::separator()))
			file.replace(0, 4, "");
		auto feedback = compiler.compileFile(pathToProjectContainerDirectory + QDir::separator() + "src",
														 file, args);
		for (auto& message : feedback.messages())
		{
			// In the map we have the src prefix
			auto fileName = message->fileName();
			if (fileName.startsWith(QString{"."} + QDir::separator()))
				fileName.replace(0, 1, "src");
			else
				fileName.prepend(QString{"src"} + QDir::separator());

			// check if we already mapped this message
			uint hash = qHash(QString{"%1%2%3%4"}.arg(fileName).
									arg(message->lineNumber(), message->columnNumber()).arg(message->message()));
			if (seenMessages.contains(hash))
				continue;
			seenMessages.insert(hash);

			// lines and columns -1 because javac begins at 1 and TextToNodeMap at 0
			Model::Node* node = nullptr;
			if (auto rootMsg = message->rootMessage())
				node = map->node(fileName, rootMsg->lineNumber() - 1, rootMsg->columnNumber() - 1);
			else
				node = map->node(fileName, message->lineNumber() - 1, message->columnNumber() - 1);
			Q_ASSERT(node);

			auto it = nodeItemMap.find(node);
			while (it != nodeItemMap.end() && it.key() == node)
			{
				auto type = message->type();
				QString typeString = type == CompilerMessage::Error ? "error" :
														(type == CompilerMessage::Warning ? "warning" : "default");
				// TODO for notes which have a rootMessage add a link to the message location.
				visualizeMessage(it.value(), message->message(), typeString);
				++it;
			}

			if (compilationOk && CompilerMessage::Error == message->type()) compilationOk = false;
		}
	}
	if (compilationOk)
		return new Interaction::CommandResult{};
	else
		return new Interaction::CommandResult{new Interaction::CommandError{"Compilation failed, check error messages!"}};
}

void JavaCompiler::visualizeMessage(Visualization::Item* item, const QString& message, const QString& type)
{
	auto overlay = new Visualization::MessageOverlay{item,
		[message](Visualization::MessageOverlay *){
		return message;
	}, Visualization::MessageOverlay::itemStyles().get(type)};

	item->addOverlay(overlay, COMPILER_MESSAGE_GROUP);
}

}
