/***********************************************************************************************************************
 * ModelItem.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/ModelItem.h"
#include "Scene.h"
#include "VisualizationException.h"

namespace Visualization {

ModelItem::ModelItem(Item* parent, Model::Node* node_, const ItemStyle *style) : Item(parent, style), node(node_), revision(-1)
{
}

bool ModelItem::needsUpdate()
{
	return revision != node->revision();
}

void ModelItem::updateSubtreeState()
{
	Item::updateSubtree();
	revision = node->revision();
}

ModelRenderer* ModelItem::renderer()
{
	if ( (static_cast<Scene*>(scene()))->renderer() ) return (static_cast<Scene*>(scene()))->renderer();
	throw VisualizationException("The scene of a ModelItem has no renderer.");
};

}
