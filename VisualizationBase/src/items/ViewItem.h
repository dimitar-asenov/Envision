/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/
#pragma once

#include "../visualizationbase_api.h"
#include "../../VisualizationBase/src/declarative/DeclarativeItem.h"

namespace Visualization {

class ViewItemNode;

/**
 * The ViewItem class represents the visualization of an entire view within a single item.
 *
 * All items in a view should be added and removed via this ViewItem class. Each Scene object contains
 * a list of ViewItem objects which can be used for that. Using this, it is possible to control what
 * is shown on the screen.
 */
class VISUALIZATIONBASE_API ViewItem : public Super<DeclarativeItem<ViewItem>> {

	ITEM_COMMON_CUSTOM_STYLENAME(ViewItem, DeclarativeItemBaseStyle)

	public:
		ViewItem(Item* parent, QString name = QString(), StyleType* style = itemStyles().get());

		static void initializeForms();

		void insertColumn(int column);
		Model::Node* insertNode(Model::Node* node, int column = 0, int row = 0);
		void removeNode(Model::Node* node);
		QList<Model::Node*> allNodes() const;
		QPoint positionOfNode(Model::Node* node) const;
		QPoint positionOfItem(Item* item) const;
		Model::Node* nodeAt(int column, int row);

		void addSpacing(int column, int row, Model::Node* spacingTarget);

		void addArrow(Model::Node* from, Model::Node* to, QString layer);
		QList<QPair<Item*, Item*>> arrowsForLayer(QString layer);
		QString fullLayerName(QString localLayer);

		const QString name() const;

		virtual void updateGeometry(int availableWidth, int availableHeight) override;
	private:
		QVector<QVector<Model::Node*>> nodes_;
		QString name_;

		struct ArrowToAdd {
			Model::Node* from_{};
			Model::Node* to_{};
			QString layer_;
			ArrowToAdd(Model::Node* from, Model::Node* to, QString layer)
				: from_(from), to_(to), layer_(layer) {}
		};
		QList<ArrowToAdd> arrowsToAdd_;
		QHash<QString, QList<QPair<Item*, Item*>>> arrows_;
		void addArrowLayer(QString layer);
		void removeArrowsForNode(Model::Node* node);

		void insertViewItemNode(ViewItemNode* node, int column, int row);

		void ensurePositionExists(int column, int row);
		void ensureColumnExists(int column);

		QVector<QVector<Model::Node*>> nodesGetter();
};

inline const QString ViewItem::name() const { return name_; }
inline QVector<QVector<Model::Node*>> ViewItem::nodesGetter() { return nodes_; }
inline QString ViewItem::fullLayerName(QString localLayer) { return name() + "_" + localLayer; }

}
