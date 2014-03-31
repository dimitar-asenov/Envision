/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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
	registerVisualizationPurpose("default_purpose");
	registerSemanticZoomLevel("default_zoom_level", 0);
}

ModelRenderer::~ModelRenderer()
{
	szLevelOrderingManager_.clear();
	visualizationGroupsManager_.clear();
	purposes_.clear();
	semanticZoomLevels_.clear();
}

Item* ModelRenderer::render(Item* parent, Model::Node* node, int purpose, int semanticZoomLevel)
{
	switch (visualizationChoiceStrategy_)
	{
	case VISUALIZATION_CHOICE_STRATEGY_TYPE_OVER_SEMANTIC_ZOOM_LEVEL_OVER_PURPOSE:
		return visualizationChoiceStrategyTypeOverSemanticZoomLevelOverPurpose(parent, node, purpose, semanticZoomLevel);
	case VISUALIZATION_CHOICE_STRATEGY_TYPE_OVER_PURPOSE_OVER_SEMANTIC_ZOOM_LEVEL:
		return visualizationChoiceStrategyTypeOverPurposeOverSemanticZoomLevel(parent, node, purpose, semanticZoomLevel);
	default:
		Q_ASSERT(false);
		return nullptr;
	}
}

Item* ModelRenderer::visualizationChoiceStrategyTypeOverSemanticZoomLevelOverPurpose(Item* parent, Model::Node* node,
																												 int purpose, int semanticZoomLevel)
{
	return basicStrategy(parent, node, purpose, semanticZoomLevel,
				[this] (int id, int, int semanticZoomLevel)
				 {return this->visualizationGroupsManager_.getByTypeIdAndSemanticZoomLevel(id, semanticZoomLevel); },
				[this] (int id, int purpose, int)
				 {return this->visualizationGroupsManager_.getByTypeIdAndPurpose(id, purpose); },
				[this] (int id, int, int)
				 {return this->visualizationGroupsManager_.getByTypeId(id); });
}

Item* ModelRenderer::visualizationChoiceStrategyTypeOverPurposeOverSemanticZoomLevel(Item* parent, Model::Node* node,
																												 int purpose, int semanticZoomLevel)
{
	return basicStrategy(parent, node, purpose, semanticZoomLevel,
				[this] (int id, int purpose, int)
				 {return this->visualizationGroupsManager_.getByTypeIdAndPurpose(id, purpose); },
				[this] (int id, int, int semanticZoomLevel)
				 {return this->visualizationGroupsManager_.getByTypeIdAndSemanticZoomLevel(id, semanticZoomLevel); },
				[this] (int id, int, int)
				 {return this->visualizationGroupsManager_.getByTypeId(id); });
}

Item* ModelRenderer::basicStrategy(Item* parent, Model::Node* node,
							int purpose, int semanticZoomLevel,
							std::function<QVector<VisualizationGroup*> ((int id, int purpose, int semanticZoomLevel))> option1,
							std::function<QVector<VisualizationGroup*> ((int id, int purpose, int semanticZoomLevel))> option2,
							std::function<QVector<VisualizationGroup*> ((int id, int purpose, int semanticZoomLevel))> option3)
{
	QList<int> typeIds = node->hierarchyTypeIds();

	int finalPurpose = purpose >= 0 ? purpose : (parent ? parent->childNodePurpose(node) : 0);

	if (finalPurpose >= purposes_.size())
		throw VisualizationException("Trying to render a node with an unregistered purpose id: " + finalPurpose);

	int finalSemanticZoomLevel = semanticZoomLevel >= 0 ? semanticZoomLevel :
																			(parent ? parent->childNodeSemanticZoomLevel(node) : 0);

	if (finalSemanticZoomLevel >= semanticZoomLevels_.size())
		throw VisualizationException("Trying to render a node with an unregistered semantic zoom level id: " +
											  finalSemanticZoomLevel);

	for(int id : typeIds)
	{
		QList<QPair<VisualizationSuitabilityScore, VisualizationGroup::ItemConstructor> > list;

		// Try to find a match for the specific purpose and semantic zoom level
		VisualizationGroup* group = visualizationGroupsManager_.getExactMatch(id, finalPurpose, finalSemanticZoomLevel);
		if (group)
			list << group->visualizationsForContext(parent, node);

		// If there is no exact match try the first option1
		if (list.isEmpty())
		{
			QVector<VisualizationGroup*> groups = option1(id, finalPurpose, finalSemanticZoomLevel);

			if (!groups.isEmpty())
			{
				list << groups.first()->visualizationsForContext(parent, node);
			}
		}

		// Option1 did not work so we try option2
		if (list.isEmpty())
		{
			QVector<VisualizationGroup*> groups = option2(id, finalPurpose, finalSemanticZoomLevel);

			if (!groups.isEmpty())
			{
				list << groups.first()->visualizationsForContext(parent, node);
			}
		}

		// Option2 did not work so we try option3
		if (list.isEmpty())
		{
			QVector<VisualizationGroup*> groups = option3(id, finalPurpose, finalSemanticZoomLevel);

			if (!groups.isEmpty())
			{
				list << groups.first()->visualizationsForContext(parent, node);
			}
		}

		if (list.size() > 0)
		{
			qSort(list);
			Item* item = list.last().second(parent, node);
			if (!parent && item->purpose() < 0) item->setPurpose(finalPurpose);
			return item;
		}
	}

	throw VisualizationException("Trying to render a node type that has no registered appropriate visualization. "
			 "The Node type is: " + node->typeName() + " The desired purpose is: " + purposes_[finalPurpose] +
			 "The desired semantic zoom level is: " + semanticZoomLevels_[finalSemanticZoomLevel]);
}

int ModelRenderer::registerVisualizationPurpose(const QString& name)
{
	if (purposes_.contains(name))
		throw VisualizationException("Trying to register a visualization purpose with a name that's already registered:"
						+ name);

	purposes_.append(name);
	return purposes_.size()-1;
}

int ModelRenderer::registerSemanticZoomLevel(const QString& name, int orderingNumber)
{
	if (semanticZoomLevels_.contains(name))
		throw VisualizationException("Trying to register a semantic zoom level with a name that's already registered:"
						+ name);

	if (szLevelOrderingManager_.hasOrderingNumber(orderingNumber))
		throw VisualizationException("Ordering number already in use: " + orderingNumber);


	semanticZoomLevels_.append(name);

	szLevelOrderingManager_.registerOrdering(semanticZoomLevelId(name), orderingNumber);

	return semanticZoomLevels_.size()-1;
}

void ModelRenderer::registerVisualization(int nodeTypeId, int purpose, int semanticZoomLevel,
		VisualizationGroup::ItemConstructor visualization)
{
	visualizationGroupsManager_.addVisualization(nodeTypeId, purpose, semanticZoomLevel, visualization);
}

void ModelRenderer::registerGroup(int nodeTypeId, int purpose, int semanticZoomLevel, VisualizationGroup* group)
{
	visualizationGroupsManager_.addGroup(nodeTypeId, purpose, semanticZoomLevel, group);
}

bool ModelRenderer::hasVisualization(int nodeTypeId, int purpose, int semanticZoomLevel)
{
	return visualizationGroupsManager_.hasVisualization(nodeTypeId, purpose, semanticZoomLevel);
}

int ModelRenderer::getCoarserSemanticZoomLevel(int currentSemanticZoomLevel)
{
	return szLevelOrderingManager_.getCoarserSemanticZoomLevel(currentSemanticZoomLevel);
}

int ModelRenderer::getFinerSemanticZoomLevel(int currentSemanticZoomLevel)
{
	return szLevelOrderingManager_.getFinerSemanticZoomLevel(currentSemanticZoomLevel);
}

}
