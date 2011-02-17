/***********************************************************************************************************************
 * ModelRenderer.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "ModelRenderer.h"
#include "VisualizationException.h"
#include "items/VExtendable.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/List.h"

namespace Visualization {

ModelRenderer::ModelRenderer() :
		useDefaultVisualizations_(true)
{
}

ModelRenderer::~ModelRenderer()
{
}

Item* ModelRenderer::render(Item* parent, Model::Node* node)
{
	int nodeTypeId = node->typeId();
	if (nodeTypeId >= visualizations.size() || !visualizations[nodeTypeId])
	{
		if (useDefaultVisualizations_ )
		{
			if (Model::List::typeIdStatic() < visualizations.size() && visualizations[Model::List::typeIdStatic()] != NULL)
			{
				Model::List* ext = dynamic_cast<Model::List*> (node);
				if (ext) return visualizations[Model::List::typeIdStatic()](parent, node);
			}

			if (Model::ExtendableNode::typeIdStatic() < visualizations.size() && visualizations[Model::ExtendableNode::typeIdStatic()] != NULL)
			{
				Model::ExtendableNode* ext = dynamic_cast<Model::ExtendableNode*> (node);
				if (ext) return visualizations[Model::ExtendableNode::typeIdStatic()](parent, node);
			}
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
