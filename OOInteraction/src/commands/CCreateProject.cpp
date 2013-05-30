/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "CCreateProject.h"

#include "OOModel/src/declarations/Project.h"
#include "VisualizationBase/src/items/RootItem.h"
#include "InteractionBase/src/events/SetCursorEvent.h"

namespace OOInteraction {

CCreateProject::CCreateProject() : CreateNamedObjectWithAttributes("project", {})
{
}

Interaction::CommandResult* CCreateProject::create(Visualization::Item* /*source*/, Visualization::Item* target,
	const QString& name, const QStringList& /*attributes*/)
{
	auto parent = dynamic_cast<OOModel::Project*> (target->node());

	OOModel::Project* project = nullptr;
	bool newModel = false;
	if (parent)
	{
		project = new OOModel::Project();
		if (!name.isEmpty()) project->setName(name);

		parent->beginModification("create project");
		parent->projects()->append(project);
		parent->endModification();
	}
	else
	{
		newModel = true;
		auto model = new Model::Model();
		project = dynamic_cast<OOModel::Project*> (model->createRoot("Project"));

		if (!name.isEmpty())
		{
			project->beginModification("set project name");
			project->setName(name);
			project->endModification();
		}

		target->scene()->addTopLevelItem( new Visualization::RootItem(project) );
		target->scene()->listenToModel(model);
	}

	target->setUpdateNeeded(Visualization::Item::StandardUpdate);
	if (newModel) target->scene()->addPostEventAction(new Interaction::SetCursorEvent(target->scene(), project,
			Interaction::SetCursorEvent::CursorDefault, true));
	else target->scene()->addPostEventAction(new Interaction::SetCursorEvent(target, project,
			Interaction::SetCursorEvent::CursorDefault, true));

	return new Interaction::CommandResult();
}

} /* namespace OOInteraction */
