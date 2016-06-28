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

#include "CSceneHandlerLoad.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "ModelBase/src/model/TreeManager.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/views/MainView.h"

using namespace Visualization;

namespace Interaction {


CSceneHandlerLoad::CSceneHandlerLoad() : CommandWithFlags{"load", {{"library"}, {"quick"}}, true}
{}

CommandResult* CSceneHandlerLoad::executeNamed(Visualization::Item*, Visualization::Item*,
		const std::unique_ptr<Visualization::Cursor>&, const QString& name, const QStringList& attributes)
{
	auto manager = new Model::TreeManager{};
	manager->load(new FilePersistence::SimpleTextFileStore{"projects/"}, name, attributes.first() == "library");

	if (!attributes.contains("library"))
	{

		auto mainScene = VisualizationManager::instance().mainScene();
		mainScene->addTopLevelNode(manager->root());
		mainScene->listenToTreeManager(manager);

		if (attributes.contains("quick")) mainScene->setApproximateUpdate(true);

		// Center view and zoom so that the entire project is within the window
		mainScene->updateNow();
		for (auto v : mainScene->views())
			if (auto mainView = dynamic_cast<Visualization::MainView*>(v))
			{
				auto size = mainView->viewport()->size();

				double sceneHeight = mainScene->sceneRect().height();
				double scale = 1;
				int scaleLevel = 2;
				while (sceneHeight*scale >= size.height())
				{
					scaleLevel++;
					scale *= 0.5;
				}

				mainView->zoom(scaleLevel);
				mainView->centerOn(mainScene->sceneRect().center());
				break;
			}
	}

	return new CommandResult{};
}

QStringList CSceneHandlerLoad::availableProjectsOnDisk()
{
	auto dir = QDir{ "projects/" };
	return dir.entryList( QDir::AllDirs | QDir::NoDot | QDir::NoDotDot, QDir::Name);
}

QStringList CSceneHandlerLoad::possibleNames(Visualization::Item*, Visualization::Item*,
															const std::unique_ptr<Visualization::Cursor>&)
{
	return availableProjectsOnDisk();
}

}
