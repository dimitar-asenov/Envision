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

#pragma once

#include "../visualizationbase_api.h"
#include "LayoutFormElement.h"
#include "../layouts/LayoutStyle.h"

namespace Model {
class Node;
class List;
}

namespace Visualization {

class FormElement;

/**
 * The sequential layout element can display a list of items either horizontally or vertically.
 *
 * It can directly be given a getter for a list of items, or alternatively a list of nodes or a node of type
 * Model::List, out of which the list of items then can be constructed by rendering each node with its default
 * visualization.
 */
class VISUALIZATIONBASE_API SequentialLayoutFormElement
		: public SuperLayoutElement<SequentialLayoutFormElement, LayoutFormElement>
{
	public:
		using ListNodeGetterFunction = std::function<Model::List*(Item* item)>;
		using ListOfNodesGetterFunction = std::function<QList<Model::Node*>(Item* item)>;
		using ListOfItemsGetterFunction = std::function<QList<Item*>(Item* item)>;
		using IntGetterFunction = std::function<int (Item* item)>;
		using StretchableWhenEmptyGetterFunction = std::function<bool (const Item* item)>;

		SequentialLayoutFormElement() = default;
		SequentialLayoutFormElement(const SequentialLayoutFormElement& other);
		SequentialLayoutFormElement& operator=(const SequentialLayoutFormElement&) = delete;
		virtual ~SequentialLayoutFormElement();

		virtual SequentialLayoutFormElement* clone() const override;

		// Functions executable on element definition
		/**
		 * Sets the getter (\a listNodeGetter) for getting a node of type List, from which the list of items to render in
		 * the sequential layout can be computed. The items will be rendered with their default style.
		 */
		SequentialLayoutFormElement* setListNode(ListNodeGetterFunction listNodeGetter);
		/**
		 * Sets the getter (\a nodeListGetter) for getting a list of nodes, from which the list of items to render in the
		 * sequential layout can be computed. The items will be rendered with their default style.
		 */
		SequentialLayoutFormElement* setListOfNodes(ListOfNodesGetterFunction nodeListGetter);
		/**
		 * Sets the getter (\a itemListGetter) for getting a list of items to render in the sequential layout.
		 */
		SequentialLayoutFormElement* setListOfItems(ListOfItemsGetterFunction itemListGetter);
		/**
		 * Sets what the \a space between two elements in the sequential layout should be.
		 */
		SequentialLayoutFormElement* setSpaceBetweenElements(int space);
		/**
		 * Sets a getter (\a spaceBetweenElementsGetter) for getting the appropriate space between two elements
		 * dynamically while rendering. If this getter is set, anything set with the setSpaceBetweenElements(int) will be
		 * ignored.
		 */
		SequentialLayoutFormElement* setSpaceBetweenElements(std::function<int(Item* item)> spaceBetweenElementsGetter);
		/**
		 * Sets the \a orientation of this sequential layout. Also sets the alignment to the default (bottom for
		 * horizontal, left for vertical).
		 */
		SequentialLayoutFormElement* setOrientation(Qt::Orientation orientation);
		/**
		 * Sets this sequential layout to be horizontal, also sets the alignment to the default (bottom)
		 */
		SequentialLayoutFormElement* setHorizontal();
		/**
		 * Sets this sequential layout to be vertical, also sets the alignment to the default (left)
		 */
		SequentialLayoutFormElement* setVertical();
		/**
		 * Sets the \a alignment. If it is not compatible with the layout's orientation, it will fall back to the default.
		 */
		SequentialLayoutFormElement* setAlignment(LayoutStyle::Alignment alignment);
		/**
		 * Defines if this layout should be displayed forwards (true) or backwards (false).
		 * Example:
		 * forwards (default): |0|1|2|3|
		 * backwards:          |3|2|1|0|
		 */
		SequentialLayoutFormElement* setForward(bool forward);
		/**
		 * Sets the minimum width to \a minWidth.
		 */
		SequentialLayoutFormElement* setMinWidthGetter(IntGetterFunction minWidthGetter);
		/**
		 * Sets the minimum height to \a minHeight.
		 */
		SequentialLayoutFormElement* setMinHeightGetter(IntGetterFunction minHeightGetter);

		/**
		 * Sets whether this form should be stretchable if it has no elements.
		 */
		SequentialLayoutFormElement* setStretchableWhenEmpty(StretchableWhenEmptyGetterFunction stretchableWhenEmpty);

		// Methods executable when items need to be rendered
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) override;
		virtual void setItemPositions(Item* item, int parentX, int parentY) override;
		virtual void synchronizeWithItem(Item* item) override;
		virtual bool sizeDependsOnParent(const Item* item) const override;
		virtual QList<ItemRegion> regions(DeclarativeItemBase* item, int parentX, int parentY) override;

		bool isEmpty(const Item* item) const override;
		bool elementOrChildHasFocus(Item* item) const override;
		/**
		 * Returns the index of the focused element for the specified \a item. If no element is focused, returns -1. This
		 * method is very similar to the focsedElementIndex of SequentialLayout (layouts folder).
		 */
		int focusedElementIndex(const Item* item) const;
		/**
		 * Returns the length of the displayed list of items for this item.
		 */
		int length(const Item* item) const;
		/**
		 * Returns the item at \a itemIndex for this \a item.
		 */
		template <typename T> T* itemAt(const Item* item, int itemIndex) const;

		// Recursive item destruction
		virtual void destroyChildItems(Item* item,
				QList<const Item* const DeclarativeItemBase::*> handledChildren) override;

	private:

		// Do not forget to update the copy constructor if adding new members.
		ListNodeGetterFunction listNodeGetter_{};
		ListOfNodesGetterFunction nodeListGetter_{};
		ListOfItemsGetterFunction itemListGetter_{};

		std::function<int(Item* item)> spaceBetweenElementsGetter_{};

		int defaultSpaceBetweenElements_{0};
		Qt::Orientation orientation_{Qt::Horizontal};
		LayoutStyle::Alignment alignment_{LayoutStyle::Alignment::Bottom};
		bool forward_{true};
		IntGetterFunction minWidthGetter_{};
		IntGetterFunction minHeightGetter_{};
		StretchableWhenEmptyGetterFunction stretchableWhenEmpty_{};

		struct ItemData
		{
			QVector<Item*> items_;
			QVector<QPointF> itemPositionWithinLayout_;
		};

		mutable QHash<const Item*, ItemData*> itemData_{};

		/**
		 * Returns the cached item list for this item.
		 */
		ItemData& dataForItem(const Item* item) const;
		/**
		 * Computes the space between elements from the getter and the previously set space.
		 */
		int spaceBetweenElements(Item* item);
		/**
		 * Synchronizes the old list of items for this \a item with the new list of \a nodes.
		 */
		void synchronizeWithNodes(Item* item, const QList<Model::Node*>& nodes);
		/**
		 * Synchronizes the old list of items for this \a item with the new list of \a items.
		 */
		void synchronizeWithItems(Item* item, const QList<Item*>& items);
};

inline SequentialLayoutFormElement* SequentialLayoutFormElement::setListNode(ListNodeGetterFunction listNodeGetter)
{
	Q_ASSERT(!nodeListGetter_ && !itemListGetter_);
	listNodeGetter_ = listNodeGetter;
	return this;
}
inline SequentialLayoutFormElement* SequentialLayoutFormElement::setListOfNodes(
		ListOfNodesGetterFunction nodeListGetter)
{
	Q_ASSERT(!listNodeGetter_ && !itemListGetter_);
	nodeListGetter_ = nodeListGetter;
	return this;
}
inline SequentialLayoutFormElement* SequentialLayoutFormElement::setListOfItems(
		ListOfItemsGetterFunction itemListGetter)
{
	Q_ASSERT(!nodeListGetter_ && !listNodeGetter_);
	itemListGetter_ = itemListGetter;
	return this;
}
inline SequentialLayoutFormElement* SequentialLayoutFormElement::setSpaceBetweenElements(int space)
{
	defaultSpaceBetweenElements_ = space;
	return this;
}
inline SequentialLayoutFormElement* SequentialLayoutFormElement::setSpaceBetweenElements(
		std::function<int(Item* item)> spaceBetweenElementsGetter)
{
	spaceBetweenElementsGetter_ = spaceBetweenElementsGetter;
	return this;
}
inline SequentialLayoutFormElement* SequentialLayoutFormElement::setOrientation(Qt::Orientation orientation)
{
	orientation_ = orientation;
	if (orientation_ == Qt::Horizontal)
		alignment_ = LayoutStyle::Alignment::Bottom;
	else
		alignment_ = LayoutStyle::Alignment::Left;
	return this;
}
inline SequentialLayoutFormElement* SequentialLayoutFormElement::setHorizontal()
{
	orientation_ = Qt::Horizontal;
	alignment_ = LayoutStyle::Alignment::Bottom;
	return this;
}
inline SequentialLayoutFormElement* SequentialLayoutFormElement::setVertical()
{
	orientation_ = Qt::Vertical;
	alignment_ = LayoutStyle::Alignment::Left;
	return this;
}
inline SequentialLayoutFormElement* SequentialLayoutFormElement::setAlignment(LayoutStyle::Alignment alignment)
{
	alignment_ = alignment;
	return this;
}
inline SequentialLayoutFormElement* SequentialLayoutFormElement::setForward(bool forward)
{
	forward_ = forward;
	return this;
}
inline SequentialLayoutFormElement* SequentialLayoutFormElement::setMinWidthGetter(IntGetterFunction minWidthGetter)
{
	minWidthGetter_ = minWidthGetter;
	return this;
}
inline SequentialLayoutFormElement* SequentialLayoutFormElement::setMinHeightGetter(IntGetterFunction minHeightGetter)
{
	minHeightGetter_ = minHeightGetter;
	return this;
}

inline SequentialLayoutFormElement* SequentialLayoutFormElement::setStretchableWhenEmpty(
		StretchableWhenEmptyGetterFunction stretchableWhenEmpty)
{
	stretchableWhenEmpty_ = stretchableWhenEmpty;
	return this;
}

template <typename T> T* SequentialLayoutFormElement::itemAt(const Item* item, int itemIndex) const
{
	return static_cast<T*>(dataForItem(item).items_.at(itemIndex));
}

}
