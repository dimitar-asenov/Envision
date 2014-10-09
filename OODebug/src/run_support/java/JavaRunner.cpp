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

#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/declarations/Method.h"

#include "Export/src/writer/TextToNodeMap.h"
#include "JavaExport/src/exporter/JavaExporter.h"

#include "../../compiler/java/JavaCompiler.h"
#include "../MainMethodFinder.h"
#include "../../OODebugException.h"

namespace OODebug {

void JavaRunner::runTree(Model::TreeManager* manager, const QString& pathToProjectContainerDirectory)
{
	auto project = DCast<OOModel::Project>(manager->root());
	Q_ASSERT(project);

	MainMethodFinder finder;
	auto mainMethod = finder.visit(project);
	if (!mainMethod)
	{
		// TODO: display a warning;
		return;
	}

	JavaCompiler::compileTree(manager, pathToProjectContainerDirectory);
	auto map = JavaExport::JavaExporter::exportMaps().map(project);

	// find the file of the main method:
	auto locations = map->locations(mainMethod);
	Q_ASSERT(locations.size());
	// The filename should be the same in every entry as we query for a method.
	auto fileName = locations.at(0).filename_;
	// remove the ending
	fileName.replace(".java", "");
	// NOTE: This next line is dependent on export plugin
	fileName.replace(QString("src") + QDir::separator(), "");

	QProcess runProcess;
	// For now we don't handle error and standard stream
	runProcess.setProcessChannelMode(QProcess::MergedChannels);
	runProcess.start("java", {"-cp", pathToProjectContainerDirectory + QDir::separator() + "build", fileName});
	// block until finished
	// TODO: this is not a solution, discuss what is better.
	runProcess.waitForFinished(-1);
	if (runProcess.exitStatus() != QProcess::NormalExit)
	{
		auto error = runProcess.error();
		if (error == QProcess::FailedToStart)
			throw new OODebugException("No java on the system?");
		else
			throw new OODebugException(QString("Uknown error %1").arg(error));
	}
	// TODO display output.
	qDebug() << runProcess.readAllStandardOutput();
}

} /* namespace OODebug */
