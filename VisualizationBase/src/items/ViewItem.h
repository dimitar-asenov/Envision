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
#include "../declarative/DeclarativeItem.h"
#include "../declarative/GridLayouter.h"

namespace Visualization {

class ViewItemNode;
class VViewItemNode;

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
		ViewItem(Item* parent, QString name = {}, StyleType* style = itemStyles().get());

		static void initializeForms();

		/**
		 * Returns whether this is a valid name for a ViewItem.
		 */
		static bool isValidName(const QString& name);

		/**
		 * The purpose to use when rendering only the public interface
		 * of an item.
		 */
		static int publicInterfacePurpose();

		/**
		 * Insert an empty major element into the grid of nodes
		 */
		void insertMajor(int major);
		/**
		 * Insert the given node into the grid at the given position, rendering
		 * it with the given purpose.
		 */
		ViewItemNode* insertNode(Model::Node* node, MajorMinorIndex pos = {}, int purpose = -1);
		/**
		 * Remove the given node from the view (this does not delete the node from the model)
		 */
		void removeNode(Model::Node* node);
		/**
		 * Returns all nodes in the grid in list form.
		 */
		QList<Model::Node*> allNodes() const;
		/**
		 * Returns the position of the given node in the grid, or (-1, -1) if it doesn't exist.
		 */
		MajorMinorIndex positionOfNode(Model::Node* node) const;
		/**
		 * Returns the position of the given item's node in the grid, or (-1, -1) if the item
		 * has no node, or the node is not part of the grid.
		 */
		MajorMinorIndex positionOfItem(Item* item) const;
		/**
		 * Returns the node at the given position. If the position is invalid, returns nullptr.
		 */
		Model::Node* nodeAt(MajorMinorIndex index) const;

		/**
		 * Adds a spacing node at the given position, using the given target and parent
		 * to determine the spacing.
		 */
		void addSpacing(MajorMinorIndex index, Model::Node* spacingTarget,
						ViewItemNode* spacingParent);

		/**
		 * Adds an arrow from the first to the second node on the given arrow layer.
		 * If fromParent and toParent are set, only one arrow will be drawn. Otherwise, an arrow
		 * will be drawn between any two instances of the given nodes.
		 */
		void addArrow(Model::Node* from, Model::Node* to, QString layer,
					  ViewItemNode* fromParent = nullptr, ViewItemNode* toParent = nullptr);

		void setArrowStyle(QString layer, QString styleName);
		/**
		 * Returns the items for which an arrow should be drawn in the given arrow layer.
		 */
		QList<QPair<Item*, Item*>> arrowsForLayer(QString layer);
		/**
		 * Returns the full layer name of the local arrow layer, as used in the overlay which
		 * exists in the scene itself.
		 */
		QString fullLayerName(const QString& localLayer) const;
		/**
		 * Returns the names of all the available arrow layers.
		 */
		QStringList arrowLayers() const;

		const QString& name() const;
		/**
		 * Sets the name to the given name. Ensure that isValidName(name) holds.
		 */
		void setName(const QString& name);

		void setMajorAxis(GridLayouter::MajorAxis majorAxis);
		GridLayouter::MajorAxis majorAxis() const;

		QJsonDocument toJson() const;
		void fromJson(QJsonDocument json);

		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;

		bool zoomLabelsEnabled();
		void setZoomLabelsEnabled(bool zoomLabelsEnabled);

	private:
		friend class ViewItemManager;
		/**
		 * Notify the view about an item being removed externally (e.g. by deleting its node).
		 */
		void cleanupRemovedItem(Item* item);
		void cleanupRemovedNode(Model::Node* node);

		QVector<QVector<Model::Node*>> nodes_;
		QString name_;
		GridLayouter::MajorAxis majorAxis_{GridLayouter::ColumnMajor};

		struct ArrowToAdd {
			ViewItemNode* fromParent_{};
			Model::Node* from_{};
			ViewItemNode* toParent_{};
			Model::Node* to_{};
			QString layer_;
		};
		QList<ArrowToAdd> arrowsToAdd_;
		QHash<QString, QString> arrowStyles_;
		QHash<QString, QList<QPair<Item*, Item*>>> arrows_;
		QStringList disabledArrowLayers_;
		QJsonObject arrowToJson(QPair<Item*, Item*> arrow, QString layer) const;
		void arrowFromJson(QJsonObject json);
		void addArrowLayer(QString layer, bool enabled = true);
		void removeArrowsForItem(Item* parent);

		template <typename NodeType>
		QList<NodeType*> referencesOfType() const;

		void insertViewItemNode(ViewItemNode* node, MajorMinorIndex index);

		void ensurePositionExists(MajorMinorIndex index);
		void ensureMajorExists(int major);

		QVector<QVector<Model::Node*>> nodesGetter();

		bool zoomLabelsEnabled_{true};
};

inline const QString& ViewItem::name() const { return name_; }
inline QString ViewItem::fullLayerName(const QString& localLayer) const { return name() + "_" + localLayer; }
inline bool ViewItem::zoomLabelsEnabled() { return zoomLabelsEnabled_;}
inline void ViewItem::setZoomLabelsEnabled(bool zoomLabelsEnabled) {zoomLabelsEnabled_ = zoomLabelsEnabled;}
inline GridLayouter::MajorAxis ViewItem::majorAxis() const { return majorAxis_;}
}
