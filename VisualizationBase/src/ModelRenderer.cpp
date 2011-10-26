/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

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
