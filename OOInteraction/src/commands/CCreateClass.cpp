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

#include "commands/CCreateClass.h"

#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/declarations/Class.h"

#include "InteractionBase/src/events/SetCursorEvent.h"
#include "VisualizationBase/src/items/RootItem.h"

using namespace OOModel;

namespace OOInteraction {

CCreateClass::CCreateClass() : CreateNamedObjectWithAttributes("class",
		{{"public", "private","protected"}})
{
}

Interaction::CommandResult* CCreateClass::create(Visualization::Item* /*source*/, Visualization::Item* target,
	const QString& name, const QStringList& attributes)
{
	auto pr = dynamic_cast<OOModel::Project*> (target->node());

	OOModel::Class* cl = new OOModel::Class();
	if (!name.isEmpty()) cl->setName(name);

	// Set visibility
	if (attributes.first() == "private" ) cl->modifiers()->set(Modifier::Private);
	else if (attributes.first()  == "protected" ) cl->modifiers()->set(Modifier::Protected);
	else if (attributes.first()  == "public" ) cl->modifiers()->set(Modifier::Public);

	bool newModel = false;
	if (pr)
	{
		pr->beginModification("create class");
		pr->classes()->append(cl);
		pr->endModification();
	}
	else
	{
		newModel = true;
		auto model = new Model::Model();
		model->setRoot(cl);

		target->scene()->addTopLevelItem( new Visualization::RootItem(cl) );
		target->scene()->listenToModel(model);
	}

	target->setUpdateNeeded(Visualization::Item::StandardUpdate);
		if (newModel) target->scene()->addPostEventAction(new Interaction::SetCursorEvent(target->scene(), cl,
				Interaction::SetCursorEvent::CursorDefault, true));
		else target->scene()->addPostEventAction(new Interaction::SetCursorEvent(target, cl,
				Interaction::SetCursorEvent::CursorDefault, true));

	return new Interaction::CommandResult();
}

} /* namespace OOInteraction */
