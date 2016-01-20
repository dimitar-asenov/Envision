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

#include "JavaRunner.h"

#include "InteractionBase/src/commands/CommandResult.h"
#include "InteractionBase/src/commands/CommandError.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Node.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/overlays/MessageOverlay.h"
#include "VisualizationBase/src/overlays/OverlayAccessor.h"

#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/declarations/Method.h"

#include "Export/src/writer/TextToNodeMap.h"
#include "JavaExport/src/exporter/JavaExporter.h"

#include "../overlays/ConsoleOverlay.h"
#include "../compiler/JavaCompiler.h"
#include "MainMethodFinder.h"
#include "../OODebugException.h"
#include "RunProcess.h"

namespace OODebug {

static ConsoleOverlay* console_{};
static OOModel::Project* lastProject_{};

// TODO: this Runner can be improved by allowing multiple main methods if there are multiple packages.
// Then we can have Maps: Package->LIST[Process], Process->Console and thus allow multiple processes to run.
Interaction::CommandResult* JavaRunner::runTree(Model::TreeManager* manager,
																const QString& pathToProjectContainerDirectory, bool debug)
{
	lastProject_ = DCast<OOModel::Project>(manager->root());
	Q_ASSERT(lastProject_);

	MainMethodFinder finder;
	auto mainMethod = finder.visit(lastProject_);
	if (!mainMethod)
		return new Interaction::CommandResult{new Interaction::CommandError{"No main method found to run!"}};

	auto compileResult = JavaCompiler::compileTree(manager, pathToProjectContainerDirectory, debug);
	if (compileResult->code() != Interaction::CommandResult::OK) return compileResult;

	auto map = JavaExport::JavaExporter::exportMaps().map(lastProject_);

	// find the file of the main method:
	auto locations = map->locations(mainMethod);
	Q_ASSERT(locations.size());
	// The filename should be the same in every entry as we query for a method.
	auto fileName = locations.at(0).filename_;
	// remove the ending
	fileName.replace(".java", "");
	// NOTE: This next line is dependent on export plugin
	fileName.replace(QString("src") + QDir::separator(), "");

	auto process = runProcess().replaceProcess();

	QObject::connect(process, &QProcess::readyReadStandardOutput, qApp, &handleOutput, Qt::QueuedConnection);
	QObject::connect(process, &QProcess::readyReadStandardError, qApp, &handleErrorOutput, Qt::QueuedConnection);

	QStringList args{"-cp", pathToProjectContainerDirectory + QDir::separator() + "build"};

	// TODO: the address here should rather be stored somewhere as it depends on what the DebugConnector uses.
	if (debug)
		args << QStringList{"-Xdebug", "-Xrunjdwp:transport=dt_socket,server=y,suspend=y,address=4000"};
	args << fileName;
	process->start("java", args);
	if (debug) // Wait for the listening on port signal
		process->waitForReadyRead();
	return new Interaction::CommandResult{};
}

void JavaRunner::handleOutput()
{
	Q_ASSERT(lastProject_ && runProcess().process());
	if (!console_)
		addConsole(lastProject_);
	console_->show();
	console_->appendText(runProcess().process()->readAllStandardOutput());
}

void JavaRunner::handleErrorOutput()
{
	Q_ASSERT(lastProject_ && runProcess().process());
	if (!console_)
		addConsole(lastProject_);
	console_->show();
	console_->appendError(runProcess().process()->readAllStandardError());
}

void JavaRunner::addConsole(Model::Node* node)
{
	static const QString overlayGroupName("Console overlay");
	auto nodeItemMap = Visualization::Item::nodeItemsMap();
	auto it = nodeItemMap.find(node);
	Q_ASSERT(it != nodeItemMap.end());
	// We don't iterate here, we only want one console.
	auto item = it.value();
	auto scene = item->scene();
	auto overlayGroup = scene->overlayGroup(overlayGroupName);

	if (!overlayGroup) overlayGroup = scene->addOverlayGroup(overlayGroupName);

	console_ = new ConsoleOverlay{item};

	overlayGroup->addOverlay(makeOverlay(console_));
}

RunProcess& JavaRunner::runProcess()
{
	static RunProcess process;
	return process;
}

}
