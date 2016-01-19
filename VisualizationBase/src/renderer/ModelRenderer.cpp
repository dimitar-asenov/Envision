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
#include "../items/RootItem.h"
#include "../items/ViewItem.h"
#include "../Scene.h"

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
	auto best = bestVisualizationForContext(parent, node, purpose, semanticZoomLevel);

	Item* rendered = best.second(parent, node);
	if (!parent && rendered->purpose() < 0)
		rendered->setPurpose( purpose < 0 ? 0 : purpose);
	return rendered;
}

bool ModelRenderer::sync(Item*& item, Item* parent, Model::Node* node)
{
	Q_ASSERT(parent); // If this is ever removed, make sure to set the purpose of parentless items explicitly

	if (!node)
	{
		if (item)
		{
			SAFE_DELETE_ITEM(item);
			parent->setUpdateNeeded(Item::StandardUpdate);
			return true;
		}
		else
			return false;
	}

	Q_ASSERT(node);

	auto best = bestVisualizationForContext(parent, node);

	bool changed = false;

	if (item && ((item->node() != node) || (item->typeId() != best.first)))
	{
		changed = true;
		SAFE_DELETE_ITEM(item);
	}

	if (!item)
	{
		changed = true;
		item = best.second(parent, node); // Note that we don't need to set the purpose since we have a parent
	}

	if (changed) parent->setUpdateNeeded(Item::StandardUpdate);

	return changed;
}

QPair<int, VisualizationGroup::ItemConstructor> ModelRenderer::bestVisualizationForContext
(Item* parent, Model::Node* node, int purpose, int semanticZoomLevel)
{
	//Always render as RootItem in a view
	if (parent && DCast<ViewItem>(parent))
		return qMakePair(RootItem::typeIdStatic(),
						 [=](Item* parent, Model::Node* n) { auto root = new RootItem{n, purpose, semanticZoomLevel};
															root->setParentItem(parent);
															return root; });

	switch (visualizationChoiceStrategy_)
	{
	case VISUALIZATION_CHOICE_STRATEGY_TYPE_OVER_SEMANTIC_ZOOM_LEVEL_OVER_PURPOSE:
		return visualizationChoiceStrategyTypeOverSemanticZoomLevelOverPurpose(parent, node, purpose, semanticZoomLevel);
	case VISUALIZATION_CHOICE_STRATEGY_TYPE_OVER_PURPOSE_OVER_SEMANTIC_ZOOM_LEVEL:
		return visualizationChoiceStrategyTypeOverPurposeOverSemanticZoomLevel(parent, node, purpose, semanticZoomLevel);
	default:
		Q_ASSERT(false);
		return {};
	}
}

QPair<int, VisualizationGroup::ItemConstructor>
ModelRenderer::visualizationChoiceStrategyTypeOverSemanticZoomLevelOverPurpose(Item* parent, Model::Node* node,
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

QPair<int, VisualizationGroup::ItemConstructor>
ModelRenderer::visualizationChoiceStrategyTypeOverPurposeOverSemanticZoomLevel(Item* parent, Model::Node* node,
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

QPair<int, VisualizationGroup::ItemConstructor>
ModelRenderer::basicStrategy(Item* parent, Model::Node* node, int purpose, int semanticZoomLevel,
							std::function<QVector<VisualizationGroup*> ((int id, int purpose, int semanticZoomLevel))> option1,
							std::function<QVector<VisualizationGroup*> ((int id, int purpose, int semanticZoomLevel))> option2,
							std::function<QVector<VisualizationGroup*> ((int id, int purpose, int semanticZoomLevel))> option3)
{
	QList<int> typeIds = node->hierarchyTypeIds();

	int finalPurpose = purpose >= 0 ? purpose : (parent ? parent->childNodePurpose(node) : 0);

	if (finalPurpose >= purposes_.size())
		throw VisualizationException("Trying to render a node with an unregistered purpose id: "
											  + QString::number(finalPurpose));

	int finalSemanticZoomLevel = semanticZoomLevel >= 0 ? semanticZoomLevel :
																			(parent ? parent->childNodeSemanticZoomLevel(node) : 0);

	if (finalSemanticZoomLevel >= semanticZoomLevels_.size())
		throw VisualizationException("Trying to render a node with an unregistered semantic zoom level id: "
											  + QString::number(finalSemanticZoomLevel));

	for (int id : typeIds)
	{
		QList<QPair<VisualizationSuitabilityScore, QPair<int, VisualizationGroup::ItemConstructor>>> list;

		// Try to find a match for the specific purpose and semantic zoom level
		VisualizationGroup* group = visualizationGroupsManager_.getExactMatch(id, finalPurpose, finalSemanticZoomLevel);
		if (group)
			list << group->visualizationsForContext(parent, node);

		// If there is no exact match try the first option1
		if (list.isEmpty())
		{
			QVector<VisualizationGroup*> groups = option1(id, finalPurpose, finalSemanticZoomLevel);

			if (!groups.isEmpty())
				list << groups.first()->visualizationsForContext(parent, node);
		}

		// Option1 did not work so we try option2
		if (list.isEmpty())
		{
			QVector<VisualizationGroup*> groups = option2(id, finalPurpose, finalSemanticZoomLevel);

			if (!groups.isEmpty())
				list << groups.first()->visualizationsForContext(parent, node);
		}

		// Option2 did not work so we try option3
		if (list.isEmpty())
		{
			QVector<VisualizationGroup*> groups = option3(id, finalPurpose, finalSemanticZoomLevel);

			if (!groups.isEmpty())
				list << groups.first()->visualizationsForContext(parent, node);
		}

		if (list.size() > 0)
		{
			qSort(list);
			return list.last().second;
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
		throw VisualizationException("Ordering number already in use: " + QString::number(orderingNumber));


	semanticZoomLevels_.append(name);

	szLevelOrderingManager_.registerOrdering(semanticZoomLevelId(name), orderingNumber);

	return semanticZoomLevels_.size()-1;
}

void ModelRenderer::registerVisualization(int nodeTypeId, int purpose, int semanticZoomLevel,
		VisualizationGroup::ItemConstructor visualization, int itemTypeId)
{
	visualizationGroupsManager_.addVisualization(nodeTypeId, purpose, semanticZoomLevel, visualization, itemTypeId);
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

QImage ModelRenderer::renderToImage(Model::Node* aNode)
{
	auto aScene = new Scene{};
	aScene->addTopLevelNode(aNode);
	aScene->updateNow();
	auto anItem = aScene->currentViewItem()->findVisualizationOf(aNode);

	QRectF r = anItem->boundingRect();
	QImage image(r.width(), r.height(), QImage::Format_ARGB32);
	image.fill(Qt::transparent);
	QPainter pmapPainter(&image);
	pmapPainter.setRenderHint(QPainter::Antialiasing);
	aScene->render(&pmapPainter, QRectF(), anItem->sceneBoundingRect());

	return image;
}

void ModelRenderer::renderToSVG(Model::Node* aNode, QString path)
{
	auto aScene = new Scene{};
	aScene->addTopLevelNode(aNode);
	aScene->updateNow();
	auto anItem = aScene->currentViewItem()->findVisualizationOf(aNode);

	QSvgGenerator* svggen = new QSvgGenerator;
	svggen->setFileName(path);
	svggen->setResolution(90);
	svggen->setSize(aScene->sceneRect().size().toSize());
	QPainter svgPainter(svggen);
	svgPainter.setRenderHint(QPainter::Antialiasing);
	aScene->render(&svgPainter, QRectF(), anItem->sceneBoundingRect());
}

}
