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

#include "CCreateField.h"

#include "CommandHelper.h"

#include "OOModel/src/expressions/EmptyExpression.h"

#include "InteractionBase/src/events/SetCursorEvent.h"

using namespace OOModel;

namespace OOInteraction {

CCreateField::CCreateField() : CreateNamedObjectWithAttributes("field",
		{{"public", "private", "protected"}, {"static"}})
{
}

Interaction::CommandResult* CCreateField::executeNamed(Visualization::Item* /*source*/, Visualization::Item* target,
	const std::unique_ptr<Visualization::Cursor>&, const QString& name, const QStringList& attributes)
{
	Q_ASSERT(target->node());
	auto f = new OOModel::Field{};
	f->setTypeExpression(new OOModel::EmptyExpression{});
	if (!name.isEmpty()) f->setName(name);

	// Set visibility
	if (attributes.first() == "private" ) f->modifiers()->set(Modifier::Private);
	else if (attributes.first() == "protected" ) f->modifiers()->set(Modifier::Protected);
	else if (attributes.first() == "public" ) f->modifiers()->set(Modifier::Public);

	// Set scope
	if (attributes.last() == "static") f->modifiers()->set(Modifier::Static);

	target->node()->beginModification("create field");
	if (auto parent = DCast<OOModel::Project> (target->node()))
		parent->fields()->append(f);
	else if (auto parent = DCast<OOModel::Module> (target->node()))
		parent->fields()->append(f);
	else if (auto parent = DCast<OOModel::Class> (target->node()))
		parent->fields()->append(f);
	else Q_ASSERT(false);
	target->node()->endModification();

	target->setUpdateNeeded(Visualization::Item::StandardUpdate);
	target->scene()->addPostEventAction(new Interaction::SetCursorEvent(target,
			(name.isEmpty() ? static_cast<Model::Node*>(f->nameNode()) : f->typeExpression()),
			Interaction::SetCursorEvent::CursorDefault, false));

	return new Interaction::CommandResult{};
}

}
