/***********************************************************************************************************************
 * ModelItem.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "ModelItem.h"

namespace Visualization {

ModelItem::ModelItem(Item* parent, Model::Node* node_) : Item(parent), node(node_), revision(-1)
{
}

bool ModelItem::needsUpdate()
{
	return revision != node->getRevision();
}

void ModelItem::updateSubtreeState()
{
	Item::updateSubtreeState();
	revision = node->getRevision();
}

}
