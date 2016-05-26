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
#include "GenericActions.h"

#include "VisualizationBase/src/icons/Icon.h"
#include "ModelBase/src/nodes/List.h"
#include "ModelBase/src/model/TreeManager.h"
#include "VisualizationBase/src/items/RootItem.h"
#include "FilePersistence/src/SystemClipboard.h"

namespace Interaction {

bool GenericActions::deleteItem(Visualization::Item *target, QKeySequence, ActionRegistry::InputState)
{
	if (!DCast<Visualization::Icon>(target)) return false;

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

	return true;
}

bool GenericActions::changePurpose(Visualization::Item *target, QKeySequence, ActionRegistry::InputState)
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
	return true;
}

bool GenericActions::copy(Visualization::Item *target, QKeySequence, ActionRegistry::InputState)
{
	if (target->ignoresCopyAndPaste()) return false;
	QList<const Model::Node*> nodesToCopy;
	auto selected = target->scene()->selectedItems();

	// Get all items from the current selection that have a node
	for (int i = 0; i<selected.size(); ++i)
	{
		auto item = selected.at(i);
		if (item->hasNode()) nodesToCopy.append(item->node());
	}

	// In case there is exactly one selected item and it has no node, try to find the first parent that has a node
	if (nodesToCopy.size() == 0 && selected.size() == 1)
	{
		auto item = selected.at(0);
		while (item)
		{
			if (item->hasNode())
			{
				nodesToCopy.append(item->node());
				break;
			}

			item = item->parent();
		}
	}

	if (nodesToCopy.size() > 0)
	{

		FilePersistence::SystemClipboard clipboard;
		arrangeNodesForClipboard(nodesToCopy);
		clipboard.putNodes(nodesToCopy);
	}
	return true;
}

void GenericActions::arrangeNodesForClipboard(QList<const Model::Node*>& list)
{
	if (list.size() > 0)
	{
		// Determine if all nodes are elements of a list
		const Model::List* parent = DCast<const Model::List> (list.first()->parent());
		if (parent)
		{
			for (int i = 1; i<list.size(); ++i)
			{
				if (list[i]->parent() != parent)
				{
					parent = nullptr;
					break;
				}
			}
		}

		if (parent)
		{
			// The selection consists only of nodes which are elements of the same list. Arrange them properly
			// according to the list's order.
			// Bubble sort
			for (int i = list.size() - 1; i > 0; --i)
				for (int k = 0; k < i; ++k)
					if (parent->indexOf(list[k]) > parent->indexOf(list[k+1])) list.swap(k, k+1);
		}
	}
}

bool GenericActions::cut(Visualization::Item *target, QKeySequence keys, ActionRegistry::InputState state)
{
	if (!copy(target, keys, state)) return false;
	return deleteItem(target, keys, state);
}

bool GenericActions::paste(Visualization::Item *target, QKeySequence, ActionRegistry::InputState)
{
	//TODO: Make this an undoable action!!!
	auto itemWithNode = target->findAncestorWithNode();
	if (target->ignoresCopyAndPaste() || !itemWithNode || itemWithNode->ignoresCopyAndPaste()) return false;

	FilePersistence::SystemClipboard clipboard;
	if (clipboard.numNodes() == 1 && target->scene()->selectedItems().size() == 1 && target->isSelected())
		if (itemWithNode->node()->typeName() == clipboard.currentNodeType())
		{
			itemWithNode->node()->manager()->beginModification(itemWithNode->node(), "paste");
			itemWithNode->node()->load(clipboard);
			itemWithNode->node()->manager()->nodeIdMap().remove(itemWithNode->node());
			itemWithNode->node()->manager()->nodeIdMap().setId(itemWithNode->node(), clipboard.currentNodeID());
			itemWithNode->node()->manager()->endModification();
			itemWithNode->setUpdateNeeded(Visualization::Item::StandardUpdate);

			return true;
		}

	return false;
}

}
