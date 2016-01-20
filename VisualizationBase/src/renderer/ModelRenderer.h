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

#pragma once

#include "../visualizationbase_api.h"
#include "SzLevelOrderingManager.h"
#include "VisualizationGroup.h"
#include "VisualizationGroupsManager.h"

namespace Visualization {

class Item;

class VISUALIZATIONBASE_API ModelRenderer
{
	public:
		ModelRenderer();
		virtual ~ModelRenderer();

		/**
		 * Returns an Item which has a parent \a parent and is a rendering of \a node.
		 *
		 * If \a purpose is specified (\a purpose >= 0) it is used as the target purpose when determining an appropriate
		 * visualization. Otherwise the purpose of \a parent is used. If \a parent is nullptr the default purpose is used.
		 */
		Item* render(Item* parent, Model::Node* node, int purpose = -1, int semanticZoomLevel = -1);

		/**
		 * (Re)renders an item in the context of a parent item.
		 *
		 * Returns true is the item was destroyed or rerendered. If nothing happened to the item, the return value is
		 * false.
		 */
		bool sync(Item*& item, Item* parent, Model::Node* node);

		QPair<int, VisualizationGroup::ItemConstructor>
		bestVisualizationForContext(Item* parent, Model::Node*, int purpose = -1, int semanticZoomLevel = -1);

		/**
		 * Returns whether a specific visualization has been registered for nodes of type \a nodeTypeId  and the provided
		 * \a purpose.
		 */
		bool hasVisualization(int nodeTypeId, int purpose = 0, int semanticZoomLevel = 0);

		void registerVisualization(int nodeTypeId, VisualizationGroup::ItemConstructor visualization, int itemTypeId);
		void registerVisualization(int nodeTypeId, int purpose, int semanticZoomLevel,
											VisualizationGroup::ItemConstructor visualization, int itemTypeId);
		void registerVisualization(int nodeTypeId, int purpose, VisualizationGroup::ItemConstructor visualization,
				int itemTypeId);
		void registerVisualization(int nodeTypeId, QString purpose, QString semanticZoomLevel,
											VisualizationGroup::ItemConstructor visualization, int itemTypeId);
		void registerVisualization(int nodeTypeId, QString purpose, VisualizationGroup::ItemConstructor visualization,
				int itemTypeId);
		void registerGroup(int nodeTypeId, int purpose, int semanticZoomLevel, VisualizationGroup* group);
		void registerGroup(int nodeTypeId, VisualizationGroup* group);

		int registerVisualizationPurpose(const QString& name);
		int numRegisteredPurposes() const;
		QString purposeName(int purpose);
		int purposeId(QString name);

		int registerSemanticZoomLevel(const QString& name, int orderingNumber);
		int numRegisteredSemanticZoomLevels() const;
		QString semanticZoomLevelName(int semanticZoomLevel);
		int semanticZoomLevelId(QString name);

		int getCoarserSemanticZoomLevel(int currentSemanticZoomLevel);
		int getFinerSemanticZoomLevel(int currentSemanticZoomLevel);

		static QImage renderToImage(Model::Node* aNode);
		static void renderToSVG(Model::Node* aNode, QString path);

	private:
		const static int VISUALIZATION_CHOICE_STRATEGY_TYPE_OVER_SEMANTIC_ZOOM_LEVEL_OVER_PURPOSE = 1;
		const static int VISUALIZATION_CHOICE_STRATEGY_TYPE_OVER_PURPOSE_OVER_SEMANTIC_ZOOM_LEVEL = 2;

		QVector<QString > purposes_;
		QVector<QString > semanticZoomLevels_;
		SzLevelOrderingManager szLevelOrderingManager_;
		VisualizationGroupsManager visualizationGroupsManager_;

		int visualizationChoiceStrategy_{VISUALIZATION_CHOICE_STRATEGY_TYPE_OVER_SEMANTIC_ZOOM_LEVEL_OVER_PURPOSE};

		QPair<int, VisualizationGroup::ItemConstructor>
		visualizationChoiceStrategyTypeOverSemanticZoomLevelOverPurpose(Item* parent, Model::Node* node,
																									 int purpose, int semanticZoomLevel);
		QPair<int, VisualizationGroup::ItemConstructor>
		visualizationChoiceStrategyTypeOverPurposeOverSemanticZoomLevel(Item* parent, Model::Node* node,
																									 int purpose, int semanticZoomLevel);

		/**
		 * Returns a visualization that is most suitable for the given parameter values.
		 *
		 * It uses a priority based approach that first searches for a match according to a function provided as \a
		 * option1. If no match was found it proceeds with \a option2 then with \a option3.
		 */
		QPair<int, VisualizationGroup::ItemConstructor>
		basicStrategy(Item *parent, Model::Node *node, int purpose, int semanticZoomLevel,
								  std::function<QVector<VisualizationGroup*> ((int, int, int))> option1,
								  std::function<QVector<VisualizationGroup*> ((int, int, int))> option2,
								  std::function<QVector<VisualizationGroup*> ((int, int, int))> option3);

};

inline void ModelRenderer::registerVisualization(int nodeTypeId, int purpose,
																 VisualizationGroup::ItemConstructor visualization, int itemTypeId)
{
	registerVisualization(nodeTypeId, purpose, 0, visualization, itemTypeId);
}

inline void ModelRenderer::registerVisualization(int nodeTypeId, VisualizationGroup::ItemConstructor visualization,
		int itemTypeId)
{
	registerVisualization(nodeTypeId, 0, 0, visualization, itemTypeId);
}

inline void ModelRenderer::registerVisualization(int nodeTypeId, QString purpose,
																 VisualizationGroup::ItemConstructor visualization, int itemTypeId)
{
	registerVisualization(nodeTypeId, purposeId(purpose), 0, visualization, itemTypeId);
}

inline void ModelRenderer::registerVisualization(int nodeTypeId, QString purpose, QString semanticZoomLevel,
																 VisualizationGroup::ItemConstructor visualization, int itemTypeId)
{
	registerVisualization(nodeTypeId, purposeId(purpose), semanticZoomLevelId(semanticZoomLevel), visualization,
			itemTypeId);
}

inline void ModelRenderer::registerGroup(int nodeTypeId, VisualizationGroup* group)
{
	registerGroup(nodeTypeId, 0, 0, group);
}

template<typename VIS, typename NODE>
Item* createVisualization(Item* parent, Model::Node* node)
{
	return new VIS{parent, static_cast<NODE*>(node)};
}

inline int ModelRenderer::numRegisteredPurposes() const { return purposes_.size(); }
inline QString ModelRenderer::purposeName(int purpose) { return purposes_[purpose]; }
inline int ModelRenderer::purposeId(QString name) { return purposes_.indexOf(name); }
inline int ModelRenderer::numRegisteredSemanticZoomLevels() const { return semanticZoomLevels_.size(); }
inline QString ModelRenderer::semanticZoomLevelName(int semanticZoomLevel)
	{ return semanticZoomLevels_[semanticZoomLevel]; }
inline int ModelRenderer::semanticZoomLevelId(QString name) { return semanticZoomLevels_.indexOf(name); }

}
