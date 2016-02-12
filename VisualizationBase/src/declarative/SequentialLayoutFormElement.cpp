/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "SequentialLayoutFormElement.h"

#include "DeclarativeItemBase.h"
#include "GridLayouter.h"
#include "../renderer/ModelRenderer.h"
#include "../items/Item.h"
#include "../items/ItemRegion.h"
#include "../cursor/LayoutCursor.h"

#include "ModelBase/src/nodes/List.h"

namespace Visualization {

SequentialLayoutFormElement::SequentialLayoutFormElement(const SequentialLayoutFormElement& other)
:	SuperLayoutElement<SequentialLayoutFormElement, LayoutFormElement>{other},
	listNodeGetter_{other.listNodeGetter_}, nodeListGetter_{other.nodeListGetter_},
 	itemListGetter_{other.itemListGetter_}, spaceBetweenElementsGetter_{other.spaceBetweenElementsGetter_},
 	defaultSpaceBetweenElements_{other.defaultSpaceBetweenElements_}, orientation_{other.orientation_},
	alignment_{other.alignment_}, forward_{other.forward_}, minWidthGetter_{other.minWidthGetter_},
	minHeightGetter_{other.minHeightGetter_}
{
}

SequentialLayoutFormElement::~SequentialLayoutFormElement()
{
	for (auto data : itemData_)
	{
		for (auto item : data->items_)
			SAFE_DELETE_ITEM(item);
		SAFE_DELETE(data);
	}
}


SequentialLayoutFormElement* SequentialLayoutFormElement::clone() const
{
	return new SequentialLayoutFormElement{*this};
}

void SequentialLayoutFormElement::computeSize(Item* item, int availableWidth, int availableHeight)
{
	auto& itemData = dataForItem(item);
	auto& itemList = itemData.items_;
	auto& itemPositions = itemData.itemPositionWithinLayout_;
	bool horizontal = orientation_ == Qt::Horizontal;
	int spacing = spaceBetweenElements(item);
	int size = itemList.size();

	int minWidth = minWidthGetter_ ? minWidthGetter_(item) : (orientation_ == Qt::Horizontal ? 3 : 10);
	int minHeight = minHeightGetter_ ? minHeightGetter_(item) : (orientation_ == Qt::Horizontal ? 10 : 3);
	int minorStretchFactor = sizeDependsOnParent(item) ? 1 : 0;

	QSize finalSize;

	if (forward_)
	{
		if (horizontal)
		{
			finalSize = GridLayouter::computeSize<false>(availableWidth, availableHeight, GridLayouter::NoMajor,
				[](){return 1;},	// numRows
				[size](){return size;},	// numColumns
				[](int, int){return true;},	// has
				[](int, int){return QPair<int, int>{1, 1};},	// spanGrid
				[&itemList](int x, int){return itemList[x]->widthInParent();},	// width
				[&itemList](int x, int){return itemList[x]->heightInParent();},	// height
				[](int, int, int, int){},	// computeElementSize
				[&itemList](int x, int, int w, int h){itemList[x]->changeGeometry(w, h);},	// changeGeometry
				[&itemList](int x, int){return itemList[x]->sizeDependsOnParent();},	// isStretchable
				[&itemPositions](int x, int, QPoint pos){itemPositions[x] = pos;},	// setPosition
				[minorStretchFactor](int){return minorStretchFactor;},	// rowStretchFactors
				[&itemList](int x){return itemList[x]->sizeDependsOnParent() ? 1 : 0;},	// columnStretchFactors
				[](int, int){return LayoutStyle::Alignment::Left;},	// horizontalAlignment
				[this](int, int){return alignment_;},	// verticalAlignment
				[](){return 0;},	// spaceBetweenRows
				[spacing](){return spacing;},	// spaceBetweenColumns
				[this](){return topMargin();},	// topMargin
				[this](){return bottomMargin();},	// bottomMargin
				[this](){return leftMargin();},	// leftMargin
				[this](){return rightMargin();},	// rightMargin
				[minWidth](){return minWidth;},	// minWidth
				[minHeight](){return minHeight;}	// minHeight
			);
		}
		else
		{
			finalSize = GridLayouter::computeSize<false>(availableWidth, availableHeight, GridLayouter::NoMajor,
				[size](){return size;},	// numRows
				[](){return 1;},	// numColumns
				[](int, int){return true;},	// has
				[](int, int){return QPair<int, int>{1, 1};},	// spanGrid
				[&itemList](int, int y){return itemList[y]->widthInParent();},	// width
				[&itemList](int, int y){return itemList[y]->heightInParent();},	// height
				[](int, int, int, int){},	// computeElementSize
				[&itemList](int, int y, int w, int h){itemList[y]->changeGeometry(w, h);},	// changeGeometry
				[&itemList](int, int y){return itemList[y]->sizeDependsOnParent();},	// isStretchable
				[&itemPositions](int, int y, QPoint pos){itemPositions[y] = pos;},	// setPosition
				[&itemList](int y){return itemList[y]->sizeDependsOnParent() ? 1 : 0;},	// rowStretchFactors
				[minorStretchFactor](int){return minorStretchFactor;},	// columnStretchFactors
				[this](int, int){return alignment_;},	// horizontalAlignment
				[](int, int){return LayoutStyle::Alignment::Top;},	// verticalAlignment
				[spacing](){return spacing;},	// spaceBetweenRows
				[](){return 0;},	// spaceBetweenColumns
				[this](){return topMargin();},	// topMargin
				[this](){return bottomMargin();},	// bottomMargin
				[this](){return leftMargin();},	// leftMargin
				[this](){return rightMargin();},	// rightMargin
				[minWidth](){return minWidth;},	// minWidth
				[minHeight](){return minHeight;}	// minHeight
			);
		}
	}
	else // Reversed order
	{
		auto invert = [size](int i){return size-1-i;};
		if (horizontal)
		{
			finalSize = GridLayouter::computeSize<false>(availableWidth, availableHeight, GridLayouter::NoMajor,
				[](){return 1;},	// numRows
				[size](){return size;},	// numColumns
				[](int, int){return true;},	// has
				[](int, int){return QPair<int, int>{1, 1};},	// spanGrid
				[&itemList, invert](int x, int){return itemList[invert(x)]->widthInParent();},	// width
				[&itemList, invert](int x, int){return itemList[invert(x)]->heightInParent();},	// height
				[](int, int, int, int){},	// computeElementSize
				[&itemList, invert](int x, int, int w, int h){itemList[invert(x)]->changeGeometry(w, h);},	// changeGeom.
				[&itemList, invert](int x, int){return itemList[invert(x)]->sizeDependsOnParent();},	// isStretchable
				[&itemPositions, invert](int x, int, QPoint pos){itemPositions[invert(x)] = pos;},	// setPosition
				[minorStretchFactor](int){return minorStretchFactor;},	// rowStretchFactors
				[&itemList, invert](int x){return itemList[invert(x)]->sizeDependsOnParent() ? 1 : 0;},	// columnStretchF.
				[](int, int){return LayoutStyle::Alignment::Left;},	// horizontalAlignment
				[this](int, int){return alignment_;},	// verticalAlignment
				[](){return 0;},	// spaceBetweenRows
				[spacing](){return spacing;},	// spaceBetweenColumns
				[this](){return topMargin();},	// topMargin
				[this](){return bottomMargin();},	// bottomMargin
				[this](){return leftMargin();},	// leftMargin
				[this](){return rightMargin();},	// rightMargin
				[minWidth](){return minWidth;},	// minWidth
				[minHeight](){return minHeight;}	// minHeight
			);
		}
		else
		{
			finalSize = GridLayouter::computeSize<false>(availableWidth, availableHeight, GridLayouter::NoMajor,
				[size](){return size;},	// numRows
				[](){return 1;},	// numColumns
				[](int, int){return true;},	// has
				[](int, int){return QPair<int, int>{1, 1};},	// spanGrid
				[&itemList, invert](int, int y){return itemList[invert(y)]->widthInParent();},	// width
				[&itemList, invert](int, int y){return itemList[invert(y)]->heightInParent();},	// height
				[](int, int, int, int){},	// computeElementSize
				[&itemList, invert](int, int y, int w, int h){itemList[invert(y)]->changeGeometry(w, h);},	// changeGeom.
				[&itemList, invert](int, int y){return itemList[invert(y)]->sizeDependsOnParent();},	// isStretchable
				[&itemPositions, invert](int, int y, QPoint pos){itemPositions[invert(y)] = pos;},	// setPosition
				[&itemList, invert](int y){return itemList[invert(y)]->sizeDependsOnParent() ? 1 : 0;},	// rowStretchF.
				[minorStretchFactor](int){return minorStretchFactor;},	// columnStretchFactors
				[this](int, int){return alignment_;},	// horizontalAlignment
				[](int, int){return LayoutStyle::Alignment::Top;},	// verticalAlignment
				[spacing](){return spacing;},	// spaceBetweenRows
				[](){return 0;},	// spaceBetweenColumns
				[this](){return topMargin();},	// topMargin
				[this](){return bottomMargin();},	// bottomMargin
				[this](){return leftMargin();},	// leftMargin
				[this](){return rightMargin();},	// rightMargin
				[minWidth](){return minWidth;},	// minWidth
				[minHeight](){return minHeight;}	// minHeight
			);
		}
	}

	setSize(item, finalSize);
}

void SequentialLayoutFormElement::setItemPositions(Item* item, int parentX, int parentY)
{
	auto& data = dataForItem(item);
	for (int i = 0; i<data.items_.size(); ++i)
	{
		// Note that margins have already been accounted for.
		QPointF newPos{parentX + x(item) + data.itemPositionWithinLayout_[i].x(),
					parentY + y(item) + data.itemPositionWithinLayout_[i].y()};

		if (newPos != data.items_[i]->pos())
			data.items_[i]->setPos(newPos); // setting the position is an expensive operation so only do it if it changed
	}
}

void SequentialLayoutFormElement::synchronizeWithItem(Item* item)
{
	if (listNodeGetter_)
	{
		Model::List* listNode = listNodeGetter_(item);
		synchronizeWithNodes(item, listNode->children());
	}
	else if (nodeListGetter_)
		synchronizeWithNodes(item, nodeListGetter_(item));
	else
	{
		Q_ASSERT(itemListGetter_);
		synchronizeWithItems(item, itemListGetter_(item));
	}

	auto& itemData = dataForItem(item);
	itemData.itemPositionWithinLayout_.resize(itemData.items_.size());
	//Note that we do not need to initialize the positions in any way since they will be overwritten in computeSize()

	for (auto i : itemData.items_) i->setParentItem(item);
}

bool SequentialLayoutFormElement::sizeDependsOnParent(const Item* item) const
{
	for (auto i : dataForItem(item).items_)
		if (i->sizeDependsOnParent())
			return true;
	return false;
}

void SequentialLayoutFormElement::destroyChildItems(Item* item,
		QList<const Item* const DeclarativeItemBase::*> handledChildren)
{
	LayoutFormElement::destroyChildItems(item, handledChildren);

	auto dataIterator = itemData_.find(item);
	if (dataIterator != itemData_.end())
	{
		for (auto item : dataIterator.value()->items_)
			SAFE_DELETE_ITEM(item);

		SAFE_DELETE(dataIterator.value());
		itemData_.erase(dataIterator);
	}
}

SequentialLayoutFormElement::ItemData& SequentialLayoutFormElement::dataForItem(const Item* item) const
{
	auto dataIterator = itemData_.find(item);
	if (dataIterator != itemData_.end()) return *(dataIterator.value());

	auto data = new ItemData;
	itemData_.insert(item, data);
	return *data;
}

int SequentialLayoutFormElement::spaceBetweenElements(Item* item)
{
	if (spaceBetweenElementsGetter_)
		return spaceBetweenElementsGetter_(item);
	return defaultSpaceBetweenElements_;
}

void SequentialLayoutFormElement::synchronizeWithNodes(Item* item, const QList<Model::Node*>& nodes)
{
	Visualization::Item::synchronizeCollections(item, nodes, dataForItem(item).items_,
		[](Model::Node* node, Item* item){return item->node() == node;},
		[](Item* parent, Model::Node* node){return parent->renderer()->render(parent, node);},
		[](Item* parent, Model::Node* node, Item*& item){return parent->renderer()->sync(item, parent, node);});
}

void SequentialLayoutFormElement::synchronizeWithItems(Item* item, const QList<Item*>& items)
{
	Visualization::Item::synchronizeCollections(item, items, dataForItem(item).items_,
		[](Item* newItem, Item* oldItem){return newItem == oldItem;},
		[](Item*, Item* newItem){return newItem;},
		[](Item*, Item*, Item*&){return false;});
}

QList<ItemRegion> SequentialLayoutFormElement::regions(DeclarativeItemBase* item, int parentX, int parentY)
{
	QList<ItemRegion> regs;

	auto& itemData = dataForItem(item);
	auto& itemList = itemData.items_;

	// If this layout is not visible return no regions
	if (itemList.isEmpty() && !hasCursorWhenEmpty(item))
		return regs;

	bool horizontal = orientation_ == Qt::Horizontal;
	int spacing = spaceBetweenElements(item);
	int size = itemList.size();


	if (forward_)
		{
			if (horizontal)
			{
				regs = GridLayouter::regions(item, this, parentX + x(item), parentY + y(item),
						GridLayouter::RowMajor,
						hasCursorWhenEmpty(item), !noInnerCursors(item), !noBoundaryCursors(item),
						item->style()->extraCursorsOutsideShape(), false, notLocationEquivalentCursors(item),
						[](){return 1;},	// numRows
						[size](){return size;},	// numColumns
						[](int, int){return true;},	// has
						[&itemList](int x, int){return itemList[x]->widthInParent();},	// width
						[&itemList](int x, int){return itemList[x]->heightInParent();},	// height
						[&itemList](int x, int){return (int) itemList[x]->x();},	// xPos
						[&itemList](int x, int){return (int) itemList[x]->y();},	// yPos
						[&itemList](int x, int){return itemList[x];},	// childItem
						[spacing](){return spacing;},	// spaceBetweenRows
						[spacing](){return spacing;},	// spaceBetweenColumns
						[this](){return topMargin();},	// topMargin
						[this](){return bottomMargin();},	// bottomMargin
						[this](){return leftMargin();},	// leftMargin
						[this](){return rightMargin();}	// rightMargin
					);
			}
			else
			{
				regs = GridLayouter::regions(item, this, parentX + x(item), parentY + y(item),
						GridLayouter::ColumnMajor,
						hasCursorWhenEmpty(item), !noInnerCursors(item), !noBoundaryCursors(item),
						item->style()->extraCursorsOutsideShape(), false, notLocationEquivalentCursors(item),
						[size](){return size;},	// numRows
						[](){return 1;},	// numColumns
						[](int, int){return true;},	// has
						[&itemList](int, int y){return itemList[y]->widthInParent();},	// width
						[&itemList](int, int y){return itemList[y]->heightInParent();},	// height
						[&itemList](int, int y){return (int) itemList[y]->x();},	// xPos
						[&itemList](int, int y){return (int) itemList[y]->y();},	// yPos
						[&itemList](int, int y){return itemList[y];},	// childItem
						[spacing](){return spacing;},	// spaceBetweenRows
						[spacing](){return spacing;},	// spaceBetweenColumns
						[this](){return topMargin();},	// topMargin
						[this](){return bottomMargin();},	// bottomMargin
						[this](){return leftMargin();},	// leftMargin
						[this](){return rightMargin();}	// rightMargin
					);
			}
		}
		else // Reversed order
		{
			auto invert = [size](int i){return size-1-i;};
			if (horizontal)
			{
				regs = GridLayouter::regions(item, this, parentX + x(item), parentY + y(item),
						GridLayouter::RowMajor,
						hasCursorWhenEmpty(item), !noInnerCursors(item), !noBoundaryCursors(item),
						item->style()->extraCursorsOutsideShape(), false, notLocationEquivalentCursors(item),
						[](){return 1;},	// numRows
						[size](){return size;},	// numColumns
						[](int, int){return true;},	// has
						[&itemList, invert](int x, int){return itemList[invert(x)]->widthInParent();},	// width
						[&itemList, invert](int x, int){return itemList[invert(x)]->heightInParent();},	// height
						[&itemList, invert](int x, int){return (int) itemList[invert(x)]->x();},	// xPos
						[&itemList, invert](int x, int){return (int) itemList[invert(x)]->y();},	// yPos
						[&itemList, invert](int x, int){return itemList[invert(x)];},	// childItem
						[spacing](){return spacing;},	// spaceBetweenRows
						[spacing](){return spacing;},	// spaceBetweenColumns
						[this](){return topMargin();},	// topMargin
						[this](){return bottomMargin();},	// bottomMargin
						[this](){return leftMargin();},	// leftMargin
						[this](){return rightMargin();}	// rightMargin
					);
			}
			else
			{
				regs = GridLayouter::regions(item, this, parentX + x(item), parentY + y(item),
						GridLayouter::ColumnMajor,
						hasCursorWhenEmpty(item), !noInnerCursors(item), !noBoundaryCursors(item),
						item->style()->extraCursorsOutsideShape(), false, notLocationEquivalentCursors(item),
						[size](){return size;},	// numRows
						[](){return 1;},	// numColumns
						[](int, int){return true;},	// has
						[&itemList, invert](int, int y){return itemList[invert(y)]->widthInParent();},	// width
						[&itemList, invert](int, int y){return itemList[invert(y)]->heightInParent();},	// height
						[&itemList, invert](int, int y){return (int) itemList[invert(y)]->x();},	// xPos
						[&itemList, invert](int, int y){return (int) itemList[invert(y)]->y();},	// yPos
						[&itemList, invert](int, int y){return itemList[invert(y)];},	// childItem
						[spacing](){return spacing;},	// spaceBetweenRows
						[spacing](){return spacing;},	// spaceBetweenColumns
						[this](){return topMargin();},	// topMargin
						[this](){return bottomMargin();},	// bottomMargin
						[this](){return leftMargin();},	// leftMargin
						[this](){return rightMargin();}	// rightMargin
					);
			}
		}

	return regs;
}

bool SequentialLayoutFormElement::isEmpty(const Item* item) const
{
	for (auto i : dataForItem(item).items_)
		if (!i->isEmpty())
			return false;
	return true;
}

bool SequentialLayoutFormElement::elementOrChildHasFocus(Item* item) const
{
	if (LayoutFormElement::elementOrChildHasFocus(item))
		return true;
	for (auto i : dataForItem(item).items_)
		if (i->itemOrChildHasFocus())
			return true;
	return false;
}

int SequentialLayoutFormElement::focusedElementIndex(const Item* item) const
{
	auto& itemList = dataForItem(item).items_;

	for (int i = 0; i<itemList.size(); ++i)
		if (itemList[i]->itemOrChildHasFocus()) return i;

	return -1;
}

int SequentialLayoutFormElement::length(const Item* item) const
{
	return dataForItem(item).items_.size();
}

}
