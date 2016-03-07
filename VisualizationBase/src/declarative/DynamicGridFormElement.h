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
#include "LayoutFormElement.h"
#include "../layouts/LayoutStyle.h"
#include "GridLayouter.h"

namespace Model {
	class Node;
}

namespace Visualization {

class VISUALIZATIONBASE_API DynamicGridFormElement
		: public SuperLayoutElement<DynamicGridFormElement, LayoutFormElement>
{
	public:
		using NodesGetterFunction = std::function<QVector<QVector<Model::Node*>>(Item* item)>;
		using ItemsGetterFunction = std::function<QVector<QVector<Item*>>(Item* item)>;
		using SpanGetterFunction = std::function<QVector<QVector<QPair<int, int>>>(Item* item)>;

		DynamicGridFormElement() = default;
		DynamicGridFormElement(const DynamicGridFormElement& other);
		DynamicGridFormElement& operator=(const DynamicGridFormElement&) = delete;
		virtual ~DynamicGridFormElement() override;

		virtual DynamicGridFormElement* clone() const override;

		DynamicGridFormElement* setNodesGetter(NodesGetterFunction nodeGetter);
		DynamicGridFormElement* setItemsGetter(ItemsGetterFunction itemGetter);
		DynamicGridFormElement* setSpanGetter(SpanGetterFunction spanGetter);

		// Methods executable on element definition
		/**
		 * Sets the horizontal and the vertical spacing to \a spaceBetweenColumns and \a spaceBetweenRows respectively.
		 * Returns a pointer to this DynamicGridFormElement.
		 */
		DynamicGridFormElement* setSpacing(int spaceBetweenColumns, int spaceBetweenRows);
		/**
		 * Sets the horizontal alignment to \a alignment for all the cells in the grid.
		 * Returns a pointer to this DynamicGridFormElement.
		 */
		DynamicGridFormElement* setHorizontalAlignment(LayoutStyle::Alignment alignment);
		/**
		 * Sets the vertical alignment to \a alignment for all the cells in the grid.
		 * Returns a pointer to this DynamicGridFormElement.
		 */
		DynamicGridFormElement* setVerticalAlignment(LayoutStyle::Alignment alignment);

		/**
		 * Sets the major axis of this layout.
		 * Returns a pointer to this DynamicGridFormElement.
		 *
		 * If a major axis is set, the input grid is assumed to be a vector of [MajorAxis][MinorAxis].
		 */
		DynamicGridFormElement* setMajorAxis(GridLayouter::MajorAxis majorAxis);


		// Methods executable when items need to be rendered
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) override;
		virtual void setItemPositions(Item* item, int parentX, int parentY) override;
		virtual void synchronizeWithItem(Item* item) override;
		virtual bool sizeDependsOnParent(const Item* item) const override;
		virtual void destroyChildItems(Item* item,
												 QList<const Item* const DeclarativeItemBase::*> handledChildren) override;

		/**
		 * Returns the index of the focused element for the specified \a item. If no element is focused, returns {-1,-1}.
		 */
		QPoint focusedElementIndex(Item* item) const;

		/**
		 * Returns the index of \a child or {-1, -1} if this layout does not manage \a child.
		 *
		 * \a child could be a direct or an indirect child of the layout.
		 */
		QPoint indexOf(Item* item, Item* child) const;

		QSize gridSize(const Item* item) const;
		virtual bool elementOrChildHasFocus(Item* item) const override;
		virtual bool isEmpty(const Item* item) const override;
		virtual QList<ItemRegion> regions(DeclarativeItemBase* item, int parentX, int parentY) override;

	private:

		// Do not forget to update the copy constructor if adding new members.
		NodesGetterFunction nodesGetterFunction_{}; // Expects a list of ROWS (result[y][x])
		ItemsGetterFunction itemsGetterFunction_{}; // Expects a list of ROWS (result[y][x])
		SpanGetterFunction spanGetterFunction_{}; // Expects a list of ROWS (result[y][x])

		int spaceBetweenColumns_{};
		int spaceBetweenRows_{};
		GridLayouter::MajorAxis majorAxis_{};

		LayoutStyle::Alignment horizontalAlignment_{LayoutStyle::Alignment::Left};
		LayoutStyle::Alignment verticalAlignment_{LayoutStyle::Alignment::Top};

		struct ItemData
		{
			int numRows_{};
			int numColumns_{};
			QVector<QVector<Item*>> itemGrid_;
			QVector<QVector<QPair<int, int>>> itemSpan_;
			QVector<QVector<QPointF>> itemPositionWithinLayout_;
		};

		mutable QHash<const Item*, ItemData*> itemData_{};

		ItemData& dataForItem(const Item* item) const;

		template <typename Definition, typename CompareFunction, typename CreateFunction, typename SyncFunction>
		void synchronizeGrids(ItemData& data, const Definition& def, CompareFunction compare,
										 CreateFunction create, SyncFunction sync);
};

inline DynamicGridFormElement* DynamicGridFormElement::setNodesGetter(NodesGetterFunction nodeGetter)
{
	Q_ASSERT(!itemsGetterFunction_);
	nodesGetterFunction_ = nodeGetter;
	return this;
}

inline DynamicGridFormElement* DynamicGridFormElement::setItemsGetter(ItemsGetterFunction itemGetter)
{
	Q_ASSERT(!nodesGetterFunction_);
	itemsGetterFunction_ = itemGetter;
	return this;
}

inline DynamicGridFormElement* DynamicGridFormElement::setSpanGetter(SpanGetterFunction spanGetter)
{
	spanGetterFunction_ = spanGetter;
	return this;
}

inline DynamicGridFormElement* DynamicGridFormElement::setSpacing(int spaceBetweenColumns, int spaceBetweenRows)
{
	spaceBetweenColumns_ = spaceBetweenColumns;
	spaceBetweenRows_ = spaceBetweenRows;
	return this;
}

inline DynamicGridFormElement* DynamicGridFormElement::setMajorAxis(GridLayouter::MajorAxis majorAxis)
{
	majorAxis_ = majorAxis;
	return this;
}

inline DynamicGridFormElement* DynamicGridFormElement::setHorizontalAlignment(LayoutStyle::Alignment alignment)
{
	horizontalAlignment_ = alignment;
	return this;
}

inline DynamicGridFormElement* DynamicGridFormElement::setVerticalAlignment(LayoutStyle::Alignment alignment)
{
	verticalAlignment_ = alignment;
	return this;
}

inline QSize DynamicGridFormElement::gridSize(const Item* item) const
{
	auto& data = dataForItem(item);
	return QSize{data.numColumns_, data.numRows_};
}

inline bool DynamicGridFormElement::isEmpty(const Item* item) const { return gridSize(item).isEmpty(); }

}
