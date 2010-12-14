/***********************************************************************************************************************
 * ModelRenderer.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "ModelRenderer.h"
#include "VisualizationException.h"
#include "ModelItem.h"

namespace Visualization {

ModelRenderer::ModelRenderer()
{
}

ModelItem* ModelRenderer::render(Item* parent, Model::Node* node)
{
	int nodeTypeId = node->getTypeId();
	if (nodeTypeId >= visualizations.size() || !visualizations[nodeTypeId]) throw VisualizationException("Trying to render a node type that has no registered visualization.");
	return visualizations[nodeTypeId](parent, node);
}

void ModelRenderer::registerVisualization(int typeId, ItemConstructor visualization)
{
	if (typeId >= visualizations.size()) visualizations.resize(typeId + 1);
	visualizations[typeId] = visualization;
}

}
