/***********************************************************************************************************************
 * ModelRenderer.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "ModelRenderer.h"
#include "VisualizationException.h"
#include "items/ModelItem.h"
#include "items/VExtendable.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"

namespace Visualization {

ModelRenderer::ModelRenderer() :
		useDefaultExtendableNodeVisualization_(true)
{
}

ModelRenderer::~ModelRenderer()
{
}

ModelItem* ModelRenderer::render(Item* parent, Model::Node* node)
{
	int nodeTypeId = node->typeId();
	if (nodeTypeId >= visualizations.size() || !visualizations[nodeTypeId])
	{
		if (useDefaultExtendableNodeVisualization_ )
		{
			Model::ExtendableNode* ext = dynamic_cast<Model::ExtendableNode*> (node);
			if (ext) return new VExtendable(parent, ext);
		}

		throw VisualizationException("Trying to render a node type that has no registered visualization. Node type is: " + node->typeName());
	}
	else return visualizations[nodeTypeId](parent, node);
}

void ModelRenderer::registerVisualization(int typeId, ItemConstructor visualization)
{
	if (typeId >= visualizations.size()) visualizations.resize(typeId + 1);
	visualizations[typeId] = visualization;
}

}
