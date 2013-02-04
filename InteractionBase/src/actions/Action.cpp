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

/*
 * Action.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: Dimitar Asenov
 */

#include "Action.h"

#include "../InteractionBaseException.h"

namespace Interaction {

QList<Action*>& Action::actions(int nodeTypeId)
{
	static QMap<int, QList<Action*>* > map;
	auto val = map.find(nodeTypeId);
	if (val != map.end()) return **val;
	else
	{
		map.insert(nodeTypeId, new QList<Action*>{});
		return *map.value(nodeTypeId);
	}
}

Action::Action(const QString& shortcut, const QString& name)
	: shortcut_{shortcut}, name_{name}
{}

Action::Action(const QString& shortcut, const QString& name, ActionFunctionOnNode action)
	: shortcut_{shortcut}, name_{name}, actionOnNode_{action}
{}

Action::Action(const QString& shortcut, const QString& name, ActionFunctionOnItem action)
: shortcut_{shortcut}, name_{name}, actionOnItem_{action}
{}

Action::~Action()
{
}

void Action::execute(Model::Node* node)
{
	if (actionOnNode_) actionOnNode_(node);
	else throw InteractionBaseException("Nothing to execute for action" + name_ + "(" + shortcut_ + ").");
}

void Action::execute(Visualization::Item* itemWithNode)
{
	Q_ASSERT(itemWithNode->node());
	if (actionOnItem_) actionOnItem_(itemWithNode);
	else execute(itemWithNode->node());
}

} /* namespace Interaction */
