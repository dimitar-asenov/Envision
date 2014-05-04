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
#include "../node_extensions/Position.h"

#include "../items/ItemWithNode.h"
#include "ArrangementAlgorithmItem.h"
#include "Layout.h"
#include "PositionLayoutStyle.h"

namespace  Visualization {

class ModelRenderer;

class VISUALIZATIONBASE_API PositionLayout : public Super<Layout>
{
	ITEM_COMMON(PositionLayout)

	public:
		PositionLayout(Item* parent, const StyleType* style = itemStyles().get());
		~PositionLayout();

		int length() const;

		/**
		 * Inserts the specified item into the Layout.
		 *
		 *	@param item
		 *					The Item to insert. This item must implement ModelItem that represents a node derived from
		 *					Model::CompositeNode. Furthermore the represented node must have the Position Extension.
		 */
		void insert(Item* item);

		void remove(int index, bool deleteItem = true);
		void removeAll(Item* item, bool deleteItem = true);
		void clear(bool deleteItems = true);
		bool containsNode(Model::Node* node);

		template <class T> T* at(int index);
		template <class T> T* at(int index) const;

		int toGrid(const int& pos) const;

		void synchronizeWithNodes(const QList<Model::Node*>& nodes, ModelRenderer* renderer);

		virtual bool isEmpty() const;

		virtual void updateGeometry(int availableWidth, int availableHeight);

		int focusedElementIndex() const;

		virtual bool itemGeometryChangesWithZoom() const override;

	protected:
		void determineChildren() override;

	private:
		/**
		 * @brief
		 * 				rearranges the items in an attempt to make them as readable as possible.
		 *
		 * @param sizeWidth
		 * 				the inner width of the PositionLayout.
		 *
		 * @param sizeHeight
		 * 				the inner height of the PositionLayout.
		 */
		void arrangeItems(int sizeWidth, int sizeHeight);

		/**
		 * @brief
		 * 				scales the item as large as possible with respect to the area available to it.
		 *
		 * @param item
		 * 				the item to be scaled.
		 *
		 * @param geometricZoomScale
		 * 				the geometric zoom scale of the scene.
		 *
		 * @return
		 * 				whether the item could reach a perceived scale (geometric zoom scale * totalScale) of 1.
		 *
		 * This function is used to find out whether an item has already enough space to be shown large enough.
		 * The geometric zoom scale is a parameter for optimization purposes.
		 */
		bool scaleItem(ArrangementAlgorithmItem* item, qreal geometricZoomScale);

		/**
		 * @brief
		 * 				handles the automatic semantic zoom level change of an item.
		 *
		 * @param item
		 * 				the item to be handled.
		 *
		 * @param allItems
		 * 				a vector containing all items.
		 *
		 * @param zoomedIn
		 * 				whether the user last geometrically zoomed in.
		 *
		 * @param zoomedOut
		 * 				whether the user last geoemtrically zoomed out.
		 *
		 * @param geometricZoomScale
		 * 				the geometric zoom scale.
		 *
		 * Geometric zoom scale, zoomedIn and zoomedOut are parameters for optimization purposes.
		 */
		void automaticSemanticZoomLevelChange(ArrangementAlgorithmItem& item, QVector<ArrangementAlgorithmItem>& allItems,
													 bool zoomedIn, bool zoomedOut, qreal geometricZoomScale);

		/**
		 * @brief
		 * 				estimates the size of the provided item when shown in full detail.
		 *
		 * @param item
		 * 				the item of which the function should guess the size in full detail.
		 *
		 * @return
		 * 				the estimated size of the provided item when shown in full detail.
		 *
		 * This function is used in the automatic semantic zoom level switch mechanism when there is no exact information
		 * available about on which level an item became less detailed.
		 */
		QSizeF estimatedItemSizeInFullDetail(Item* item);

		void calculateNodesPositionInfo();

		QVector<Item*> items;
		QVector<Position*> positions;
		bool allNodesLackPositionInfo{};

		void swap(int i, int j);
};

template <class T> T* PositionLayout::at(int index) { return static_cast<T*> (items[index]); }
template <class T> T* PositionLayout::at(int index) const { return static_cast<T*> (items[index]); }

}
