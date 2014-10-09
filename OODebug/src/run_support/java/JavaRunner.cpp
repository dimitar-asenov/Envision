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

#include "../../compiler/java/JavaCompiler.h"
#include "../MainMethodFinder.h"
#include "../../OODebugException.h"

namespace OODebug {

static QProcess* runProcess_{};
static OOModel::Project* lastProject_{};

void JavaRunner::runTree(Model::TreeManager* manager, const QString& pathToProjectContainerDirectory)
{
	lastProject_ = DCast<OOModel::Project>(manager->root());
	Q_ASSERT(lastProject_);

	MainMethodFinder finder;
	auto mainMethod = finder.visit(lastProject_);
	if (!mainMethod)
		return noMainMethodWarning(lastProject_);

	JavaCompiler::compileTree(manager, pathToProjectContainerDirectory);
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

	if (runProcess_)
		runProcess_->kill(); // Deletion is done with the lambda below.
	runProcess_ = new QProcess();

	QObject::connect(runProcess_, &QProcess::readyReadStandardOutput, qApp, &handleOutput, Qt::QueuedConnection);
	QObject::connect(runProcess_, &QProcess::readyReadStandardError, qApp, &handleErrorOutput, Qt::QueuedConnection);

	// We have to make a copy here of the pointer such that we do not delete the new instance.
	// By using the kill slot we know that we will always clean the memory of the old process.
	QProcess* process = runProcess_;
	QObject::connect(process, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
						  [process](int){process->deleteLater();});

	runProcess_->start("java", {"-cp", pathToProjectContainerDirectory + QDir::separator() + "build", fileName});
}

void JavaRunner::noMainMethodWarning(Model::Node* node)
{
	Q_ASSERT(node);

	static const QString overlayGroupName("RunErrors");
	auto nodeItemMap = Visualization::Item::nodeItemsMap();
	auto it = nodeItemMap.find(node);
	while (it != nodeItemMap.end() && it.key() == node)
	{
		auto item = it.value();
		auto scene = item->scene();
		auto overlayGroup = scene->overlayGroup(overlayGroupName);

		if (!overlayGroup) overlayGroup = scene->addOverlayGroup(overlayGroupName);

		overlayGroup->addOverlay(makeOverlay( new Visualization::MessageOverlay(item,
			[node](Visualization::MessageOverlay *){
			return QString("No main method found");
		}, Visualization::MessageOverlay::itemStyles().get("warning"))));

		++it;
	}
}

void JavaRunner::handleOutput()
{
}

void JavaRunner::handleErrorOutput()
{
}

} /* namespace OODebug */
