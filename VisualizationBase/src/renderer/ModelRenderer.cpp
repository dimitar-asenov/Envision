/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "ModelRenderer.h"
#include "../VisualizationException.h"
#include "../items/Item.h"

#include "ModelBase/src/nodes/Node.h"

namespace Visualization {

ModelRenderer::ModelRenderer()
{
	registerVisualizationPurpose("Default");
}

ModelRenderer::~ModelRenderer()
{
	for(auto groups : groups_)
		for(auto g : groups)
		SAFE_DELETE(g);

	groups_.clear();
	purposes_.clear();
}

Item* ModelRenderer::render(Item* parent, Model::Node* node, int purpose)
{
	QList<int> typeIds = node->hierarchyTypeIds();

	int finalPurpose = purpose >= 0 ? purpose : (parent ? parent->childNodePurpose(node) : 0);

	if (finalPurpose >= purposes_.size())
		throw VisualizationException("Trying to render a node with an unregistered purpose id: " + finalPurpose);

	for(int id : typeIds)
	{
		QList<QPair<VisualizationSuitabilityScore, VisualizationGroup::ItemConstructor> > list;

		// Try to find a match for the specific purpose
		if (id < groups_.size() && finalPurpose < groups_[id].size() && groups_[id][finalPurpose])
			list << groups_[id][finalPurpose]->visualizationsForContext(parent, node);

		// If there is no match for the specific purpose try to find a generic match
		if (list.isEmpty() && id < groups_.size() && !groups_[id].isEmpty() && groups_[id][0])
			list << groups_[id][0]->visualizationsForContext(parent, node);

		if (list.size() > 0)
		{
			qSort(list);
			Item* item = list.last().second(parent, node);
			if (!parent && item->purpose() < 0) item->setPurpose(finalPurpose);
			return item;
		}
	}

	throw VisualizationException("Trying to render a node type that has no registered appropriate visualization. "
			 "The Node type is: " + node->typeName() + " The desired purpose is: " + purposes_[finalPurpose]);
}

int ModelRenderer::registerVisualizationPurpose(const QString& name)
{
	if (purposes_.contains(name))
		throw VisualizationException("Trying to register a visualization purpose with a name that's already registered:"
						+ name);

	purposes_.append(name);
	return purposes_.size()-1;
}

void ModelRenderer::registerVisualization(int nodeTypeId, int purpose,
		VisualizationGroup::ItemConstructor visualization)
{
	Q_ASSERT(nodeTypeId > 0);
	if (nodeTypeId >= groups_.size()) groups_.resize(nodeTypeId + 1);
	if (purpose >= groups_[nodeTypeId].size()) groups_[nodeTypeId].resize(purpose+1);

	if (!groups_[nodeTypeId][purpose]) groups_[nodeTypeId][purpose] = new VisualizationGroup();

	groups_[nodeTypeId][purpose]->addVisualization(visualization);
}

void ModelRenderer::registerGroup(int nodeTypeId, int purpose, VisualizationGroup* group)
{
	if (nodeTypeId >= groups_.size()) groups_.resize(nodeTypeId + 1);
	if (purpose >= groups_[nodeTypeId].size()) groups_[nodeTypeId].resize(purpose+1);

	if (!groups_[nodeTypeId][purpose]) groups_[nodeTypeId][purpose] = new VisualizationGroup();

	groups_[nodeTypeId][purpose]->addSubGroup(group);
}

bool ModelRenderer::hasVisualization(int nodeTypeId, int purpose)
{
	Q_ASSERT(nodeTypeId > 0);
	if (nodeTypeId >= groups_.size()) return false;
	if (purpose >= groups_[nodeTypeId].size()) return false;
	if (!groups_[nodeTypeId][purpose]) return false;
	return true;
	//TODO: One could also check whether there are any registered visualizations/subgroups.
}

}
