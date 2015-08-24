/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
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
#include "KeyInputEventFunctions.h"

#include "VisualizationBase/src/icons/Icon.h"
#include "ModelBase/src/nodes/List.h"
#include "VisualizationBase/src/items/RootItem.h"

namespace Interaction {

void KeyInputEventFunctions::deleteItem(Visualization::Item *target, QKeySequence, KeyInputHandler::InputState)
{
	if (DCast<Visualization::Icon>(target))
	{
		auto p = target;
		while (!p->node() && p->parent()) p = p->parent();

		if (auto node = p->node())
		{
			// Check if the parent of the node is a list and if so, delete this node
			if (auto list = DCast<Model::List>(node->parent()))
			{
				list->beginModification("removeChild");
				list->remove(node);
				list->endModification();
				p->setUpdateNeeded(Visualization::Item::StandardUpdate);
			}
			// Check if this is a root item and remove it from the scene. The corresponding model is not removed.
			else if (p->parent() && DCast<Visualization::RootItem>(p->parent()))
				p->scene()->removeTopLevelItem(p->parent());
		}
	}
}

void KeyInputEventFunctions::changePurpose(Visualization::Item *target, QKeySequence, KeyInputHandler::InputState)
{
	auto n = target;
	while (n && ! n->node()) n = n->parent();

	auto p = n->parent();
	if ( p )
	{
		int purpose = 0;
		if (p->definesChildNodePurpose(n->node()))
		{
			purpose = n->purpose() + 1;
			if ( purpose == target->scene()->renderer()->numRegisteredPurposes())
				purpose = -1; // Undefine

		}

		if (purpose >= 0) p->setChildNodePurpose(n->node(), purpose);
		else p->clearChildNodePurpose(n->node());
	}
}

}
