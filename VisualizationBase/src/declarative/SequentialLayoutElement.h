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
#include "LayoutElement.h"
#include "../layouts/LayoutStyle.h"

namespace Model {
class Node;
class List;
}

namespace Visualization {

class Element;

/**
 * The sequential layout element can display a list of items either horizontally or vertically.
 *
 * It can directly be given a getter for a list of items, or alternatively a list of nodes or a node of type
 * Model::List, out of which the list of items then can be constructed by rendering each node with its default
 * visualization.
 */
class VISUALIZATIONBASE_API SequentialLayoutElement : public LayoutElement
{
		FLUENT_ELEMENT_INTERFACE(SequentialLayoutElement);

	public:
		using ListNodeGetterFunction = std::function<Model::List*(Item* item)>;
		using ListOfNodesGetterFunction = std::function<QList<Model::Node*>(Item* item)>;
		using ListOfItemsGetterFunction = std::function<QList<Item*>(Item* item)>;

		SequentialLayoutElement();
		virtual ~SequentialLayoutElement();

		// Functions executable on element definition
		/**
		 * Sets the getter (\a listNodeGetter) for getting a node of type List, from which the list of items to render in
		 * the sequential layout can be computed. The items will be rendered with their default style.
		 */
		SequentialLayoutElement* setListNode(ListNodeGetterFunction listNodeGetter);
		/**
		 * Sets the getter (\a nodeListGetter) for getting a list of nodes, from which the list of items to render in the
		 * sequential layout can be computed. The items will be rendered with their default style.
		 */
		SequentialLayoutElement* setListOfNodes(ListOfNodesGetterFunction nodeListGetter);
		/**
		 * Sets the getter (\a itemListGetter) for getting a list of items to render in the sequential layout.
		 */
		SequentialLayoutElement* setListOfItems(ListOfItemsGetterFunction itemListGetter);
		/**
		 * Sets what the \a space between two elements in the sequential layout should be.
		 */
		SequentialLayoutElement* setSpaceBetweenElements(int space);
		/**
		 * Sets a getter (\a spaceBetweenElementsGetter) for getting the appropriate space between two elements
		 * dynamically while rendering. If this getter is set, anything set with the setSpaceBetweenElements(int) will be
		 * ignored.
		 */
		SequentialLayoutElement* setSpaceBetweenElements(std::function<int()> spaceBetweenElementsGetter);
		/**
		 * Sets the \a orientation of this sequential layout. Also sets the alignment to the default (bottom for
		 * horizontal, left for vertical).
		 */
		SequentialLayoutElement* setOrientation(Qt::Orientation orientation);
		/**
		 * Sets this sequential layout to be horizontal, also sets the alignment to the default (bottom)
		 */
		SequentialLayoutElement* setHorizontal();
		/**
		 * Sets this sequential layout to be vertical, also sets the alignment to the default (left)
		 */
		SequentialLayoutElement* setVertical();
		/**
		 * Sets the \a alignment. If it is not compatible with the layout's orientation, it will fall back to the default.
		 */
		SequentialLayoutElement* setAlignment(LayoutStyle::Alignment alignment);
		/**
		 * Defines if this layout should be displayed forwards (true) or backwards (false).
		 * Example:
		 * forwards (default): |0|1|2|3|
		 * backwards:          |3|2|1|0|
		 */
		SequentialLayoutElement* setForward(bool forward);
		/**
		 * Sets the minimum width to \a minWidth.
		 */
		SequentialLayoutElement* setMinWidth(int minWidth);
		/**
		 * Sets the minimum height to \a minHeight.
		 */
		SequentialLayoutElement* setMinHeight(int minHeight);
		/**
		 * Sets if the element has a cursor if it is empty to \a cursorWhenEmpty. Is false by default.
		 * Also see SequentialLayoutStyle::hasCursorWhenEmpty().
		 */
		SequentialLayoutElement* setHasCursorWhenEmpty(bool cursorWhenEmpty);
		/**
		 * Sets if the element does not have location equivalent cursors to \a notLocationEquivalent. Is false by
		 * default.
		 * Also see SequentialLayoutStyle::notLocationEquivalentCursors().
		 */
		SequentialLayoutElement* setNotLocationEquivalentCursors(bool notLocationEquivalent);
		/**
		 * Sets if the element has no boundary cursors to \a noBoundaryCursors. Is false by default.
		 * Also see SequentialLayoutStyle::noBoundaryCursorsInsideShape().
		 */
		SequentialLayoutElement* setNoBoudaryCursors(bool noBoundaryCursors);
		/**
		 * Sets if the element has no inner cursors to \a noInnerCursors. Is false by default.
		 * Also see SequentialLayoutStyle::noInnerCursors().
		 */
		SequentialLayoutElement* setNoInnerCursors(bool noInnerCursors);

		// Methods executable when items need to be rendered
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) override;
		virtual void setItemPositions(Item* item, int parentX, int parentY) override;
		virtual void synchronizeWithItem(Item* item) override;
		virtual bool sizeDependsOnParent(const Item* item) const override;
		virtual QList<ItemRegion> regions(Item* item, int parentX, int parentY) override;

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
		template <class T> T* itemAt(const Item* item, int itemIndex) const;

		// Recursive item destruction
		virtual void destroyChildItems(Item* item) override;

	private:
		ListNodeGetterFunction listNodeGetter_{};
		ListOfNodesGetterFunction nodeListGetter_{};
		ListOfItemsGetterFunction itemListGetter_{};

		std::function<int()> spaceBetweenElementsGetter_;

		int defaultSpaceBetweenElements_{0};
		Qt::Orientation orientation_{Qt::Horizontal};
		LayoutStyle::Alignment alignment_{LayoutStyle::Alignment::Bottom};
		bool forward_{true};
		int minWidth_{};
		int minHeight_{};
		bool hasCursorWhenEmpty_{false};
		bool notLocationEquivalentCursors_{false};
		bool noBoundaryCursors_{false};
		bool noInnerCursors_{false};

		mutable QHash<const Item*, QList<Item*>*> itemListMap_{};

		/**
		 * Returns the cached item list for this item.
		 */
		QList<Item*>& listForItem(const Item* item) const;
		/**
		 * Computes the space between elements from the getter and the previously set space.
		 */
		int spaceBetweenElements();
		/**
		 * Synchronizes the old list of items for this \a item with the new list of \a nodes.
		 */
		void synchronizeWithNodes(Item* item, const QList<Model::Node*>& nodes);
		/**
		 * Synchronizes the old list of items for this \a item with the new list of \a items.
		 */
		void synchronizeWithItems(Item* item, const QList<Item*>& items);
		/**
		 * Swaps the items at positions \a i and \a j in the list of items for \a item.
		 */
		void swap(Item* item, int i, int j);
		void adjustCursorRegionToAvoidZeroSize(QRect& region, bool horizontal, bool first, bool last);
		/**
		 * Removes the item at \a index from the itemList of \a item. If \a deleteItem is set, the item also gets deleted.
		 */
		void removeFromItemList(Item* item, int index, bool deleteItem);
};

inline SequentialLayoutElement* SequentialLayoutElement::setListNode(ListNodeGetterFunction listNodeGetter)
{
	Q_ASSERT(!nodeListGetter_ && !itemListGetter_);
	listNodeGetter_ = listNodeGetter;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setListOfNodes(ListOfNodesGetterFunction nodeListGetter)
{
	Q_ASSERT(!listNodeGetter_ && !itemListGetter_);
	nodeListGetter_ = nodeListGetter;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setListOfItems(ListOfItemsGetterFunction itemListGetter)
{
	Q_ASSERT(!nodeListGetter_ && !listNodeGetter_);
	itemListGetter_ = itemListGetter;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setSpaceBetweenElements(int space)
{
	defaultSpaceBetweenElements_ = space;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setSpaceBetweenElements(
		std::function<int()> spaceBetweenElementsGetter)
{
	spaceBetweenElementsGetter_ = spaceBetweenElementsGetter;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setOrientation(Qt::Orientation orientation)
{
	orientation_ = orientation;
	if (orientation_ == Qt::Horizontal)
		alignment_ = LayoutStyle::Alignment::Bottom;
	else
		alignment_ = LayoutStyle::Alignment::Left;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setHorizontal()
{
	orientation_ = Qt::Horizontal;
	alignment_ = LayoutStyle::Alignment::Bottom;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setVertical()
{
	orientation_ = Qt::Vertical;
	alignment_ = LayoutStyle::Alignment::Left;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setAlignment(LayoutStyle::Alignment alignment)
{
	alignment_ = alignment;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setForward(bool forward)
{
	forward_ = forward;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setMinWidth(int minWidht)
{
	minWidth_ = minWidht;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setMinHeight(int minHeight)
{
	minHeight_ = minHeight;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setHasCursorWhenEmpty(bool cursorWhenEmpty)
{
	hasCursorWhenEmpty_ = cursorWhenEmpty;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setNotLocationEquivalentCursors(
		bool notLocationEquivalentCursors)
{
	notLocationEquivalentCursors_ = notLocationEquivalentCursors;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setNoBoudaryCursors(bool noBoundaryCursors)
{
	noBoundaryCursors_ = noBoundaryCursors;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setNoInnerCursors(bool noInnerCursors)
{
	noInnerCursors_ = noInnerCursors;
	return this;
}

template <class T> T* SequentialLayoutElement::itemAt(const Item* item, int itemIndex) const
{
	auto& itemList = listForItem(item);
	return static_cast<T*>(itemList.at(itemIndex));
}

} /* namespace Visualization */
