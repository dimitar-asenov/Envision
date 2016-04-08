/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
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

#include "CDiff.h"

#include "ModelBase/src/model/TreeManager.h"

#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/overlays/SelectionOverlay.h"
#include "VisualizationBase/src/overlays/OverlayAccessor.h"
#include "VisualizationBase/src/CustomSceneEvent.h"

#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "FilePersistence/src/version_control/GitRepository.h"

#include "VersionControlUI/src/DiffManager.h"

using namespace Visualization;
using namespace FilePersistence;

namespace Interaction {

static const QString overlayGroupName{"DiffHighlights"};

CDiff::CDiff() : CommandWithFlags{"diff", {{"project"}}, true, false}
{}

CommandResult* CDiff::executeNamed(Visualization::Item*, Visualization::Item* target,
											  const std::unique_ptr<Visualization::Cursor>&,
											  const QString&, const QStringList&) __attribute__((optnone))
{
	auto scene = target->scene();
	scene->clearFocus();
	scene->clearSelection();
	scene->setMainCursor(nullptr);

	Model::TreeManager* headManager = target->node()->manager();
	QString managerName = headManager->name();

	// TODO don't use fixed versions, removed param "name" contains selected version
	VersionControlUI::DiffManager diffManager{"HEAD^^^^", "HEAD", managerName};
	diffManager.visualize();

	return new CommandResult{};
}

QStringList CDiff::possibleNames(Visualization::Item*, Visualization::Item* target,
											const std::unique_ptr<Visualization::Cursor>&)
{
	Model::TreeManager* headManager = target->node()->manager();
	QString managerName = headManager->name();

	// get GitRepository
	QString path{"projects/"};
	path.append(managerName);

	QStringList names;
	if (GitRepository::repositoryExists(path))
	{
		GitRepository repository{path};

		names.append(repository.localBranches());
		names.append(repository.tags());
		names.append(repository.revisions());
	}
	return names;
}

}
