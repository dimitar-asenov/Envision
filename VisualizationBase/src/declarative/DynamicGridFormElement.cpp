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

#include "DynamicGridFormElement.h"
#include "../items/Item.h"
#include "DeclarativeItemBase.h"


namespace Visualization {


DynamicGridFormElement::DynamicGridFormElement(const DynamicGridFormElement& other) : LayoutFormElement{other},
	nodesGetterFunction_{other.nodesGetterFunction_}, itemsGetterFunction_{other.itemsGetterFunction_},
	spanGetterFunction_{other.spanGetterFunction_},
	spaceBetweenColumns_{other.spaceBetweenColumns_}, spaceBetweenRows_{other.spaceBetweenRows_},
	majorAxis_{other.majorAxis_},
	horizontalAlignment_{other.horizontalAlignment_}, verticalAlignment_{other.verticalAlignment_}
{
	// There are no child memebrs to adjust
}

DynamicGridFormElement::~DynamicGridFormElement()
{
	for (auto data : itemData_)
	{
		for (auto list : data->itemGrid_)
			for (auto item : list)
				SAFE_DELETE_ITEM(item);

		SAFE_DELETE(data);
	}
	itemData_.clear();
}

DynamicGridFormElement* DynamicGridFormElement::clone() const
{
	return new DynamicGridFormElement{*this};
}

bool DynamicGridFormElement::sizeDependsOnParent(const Item*) const
{
	return false;
}

DynamicGridFormElement::ItemData&  DynamicGridFormElement::dataForItem(const Item* item) const
{
	auto dataIterator = itemData_.find(item);
	if (dataIterator != itemData_.end()) return *(dataIterator.value());

	auto data = new ItemData;
	itemData_.insert(item, data);
	return *data;
}

void DynamicGridFormElement::computeSize(Item* item, int availableWidth, int availableHeight)
{
	auto& data = dataForItem(item);
	QSize finalSize;

	if (majorAxis_ == GridLayouter::NoMajor)
		finalSize = GridLayouter::computeSize<true>(availableWidth, availableHeight, GridLayouter::NoMajor,
			[&data](){return data.numRows_;},	// numRows
			[&data](){return data.numColumns_;},	// numColumns
			[&data](int x, int y){return data.itemGrid_[x][y];},	// has
			[&data](int x, int y){return data.itemSpan_[x][y];},	// spanGrid
			[&data](int x, int y){return data.itemGrid_[x][y]->widthInParent();},	// width
			[&data](int x, int y){return data.itemGrid_[x][y]->heightInParent();},	// height
			[](int, int, int, int){},	// computeElementSize
			[&data](int x, int y, int w, int h){data.itemGrid_[x][y]->changeGeometry(w, h);},	// changeGeometry
			[&data](int x, int y){return data.itemGrid_[x][y]->sizeDependsOnParent();},	// isStretchable
			[&data](int x, int y, QPoint pos){data.itemPositionWithinLayout_[x][y] = pos;},	// setPosition
			[](int){return 0;},	// rowStretchFactors
			[](int){return 0;},	// columnStretchFactors
			[this](int, int){return horizontalAlignment_;},	// horizontalAlignment
			[this](int, int){return verticalAlignment_;},	// verticalAlignment
			[this](){return spaceBetweenRows_;},	// spaceBetweenRows
			[this](){return spaceBetweenColumns_;},	// spaceBetweenColumns
			[this](){return topMargin();},	// topMargin
			[this](){return bottomMargin();},	// bottomMargin
			[this](){return leftMargin();},	// leftMargin
			[this](){return rightMargin();},	// rightMargin
			[](){return 0;},	// minWidth
			[](){return 0;}	// minHeight
		);
	else
		finalSize = GridLayouter::computeSize<false>(availableWidth, availableHeight, majorAxis_,
				[&data](){return data.numRows_;},	// numRows
				[&data](){return data.numColumns_;},	// numColumns
				[&data](int x, int y){return data.itemGrid_[x][y];},	// has
				[](int, int){return QPair<int, int>{1, 1};},	// spanGrid
				[&data](int x, int y){return data.itemGrid_[x][y]->widthInParent();},	// width
				[&data](int x, int y){return data.itemGrid_[x][y]->heightInParent();},	// height
				[](int, int, int, int){},	// computeElementSize
				[&data](int x, int y, int w, int h){data.itemGrid_[x][y]->changeGeometry(w, h);},	// changeGeometry
				[&data](int x, int y){return data.itemGrid_[x][y]->sizeDependsOnParent();},	// isStretchable
				[&data](int x, int y, QPoint pos){data.itemPositionWithinLayout_[x][y] = pos;},	// setPosition
				[](int){return 0;},	// rowStretchFactors
				[](int){return 0;},	// columnStretchFactors
				[this](int, int){return horizontalAlignment_;},	// horizontalAlignment
				[this](int, int){return verticalAlignment_;},	// verticalAlignment
				[this](){return spaceBetweenRows_;},	// spaceBetweenRows
				[this](){return spaceBetweenColumns_;},	// spaceBetweenColumns
				[this](){return topMargin();},	// topMargin
				[this](){return bottomMargin();},	// bottomMargin
				[this](){return leftMargin();},	// leftMargin
				[this](){return rightMargin();},	// rightMargin
				[](){return 0;},	// minWidth
				[](){return 0;}	// minHeight
			);

	setSize(item, finalSize);
}

void DynamicGridFormElement::synchronizeWithItem(Item* item)
{
	auto& data = dataForItem(item);

	if (nodesGetterFunction_)
	{
		Q_ASSERT(!itemsGetterFunction_);

		auto nodes = nodesGetterFunction_(item);
		synchronizeGrids(data, nodes,
								[](Model::Node* def, Item* vis){ return def == vis->node();}, // compare
								[item](Model::Node* def) {return item->renderer()->render(item, def);}, // create
								[item](Model::Node* def, Item*& store) {item->synchronizeItem(store, def);} // sync
			);
	}
	else
	{
		Q_ASSERT(itemsGetterFunction_);

		auto items = itemsGetterFunction_(item);
		synchronizeGrids(data, items,
								[](Item* def, Item* vis){ return def == vis;}, // compare
								[item](Item* def) {def->setParentItem(item); return def;}, // create
								[item](Item* def, Item*& store) {Q_ASSERT(def->parent() == item); store = def;} // sync
			);
	}

	// Set the span if any
	if (spanGetterFunction_) data.itemSpan_ = spanGetterFunction_(item);
	else
	{
		data.itemSpan_.clear();
		data.itemSpan_.resize(data.numColumns_);
		for (int x = 0; x < data.numColumns_; ++x)
			data.itemSpan_[x] = QVector<QPair<int, int>>(data.numRows_, {1, 1});
	}
}

template <typename Definition, typename CompareFunction, typename CreateFunction, typename SyncFunction>
void DynamicGridFormElement::synchronizeGrids(ItemData& data, const Definition& def,
															 CompareFunction compare, CreateFunction create, SyncFunction sync)
{
	// Remove all elements from the current grid that do not match an entry

	// If an entry exists record it's location
	// One entry might be visualized multiple times
	QHash<decltype(def[0][0]), QList<Item*>> existingItems;

	//Keep track of the positions at which we already found visualizations for the entries
	QList<QPoint> usedPositions;

	for (int x = 0; x < data.numColumns_; ++x)
		for (int y = 0; y < data.numRows_; ++y)
			if (data.itemGrid_[x][y] != nullptr)
			{
				bool found = false;

				for (int n = 0; n < def.size(); ++n)
				{
					for (int j = 0; j < def[n].size(); ++j)
						//We must make sure not to visualize one node with the same item twice
						if (compare(def[n][j], data.itemGrid_[x][y]) && !usedPositions.contains(QPoint(n, j)))
						{
							sync(def[n][j], data.itemGrid_[x][y]);
							if (!existingItems.contains(def[n][j]))
								existingItems.insert(def[n][j], {});
							existingItems[def[n][j]].append(data.itemGrid_[x][y]);
							usedPositions.append(QPoint(n, j));
							found = true;
							break;
						}
					if (found)
						break;
				}

				if (!found) SAFE_DELETE_ITEM(data.itemGrid_[x][y]);
				data.itemGrid_[x][y] = nullptr;
			}

	// Get the new size.
	auto resizeAxes = [](const Definition& entries, int& sizeMajor, int& sizeMinor){
		sizeMajor = entries.size();
		sizeMinor = entries.isEmpty() ? 0 : std::max_element(entries.begin(), entries.end(),
				[](const decltype(def[0]) & a, const decltype(def[0]) & b){return a.size() < b.size();})->size();
	};
	if (majorAxis_ == GridLayouter::ColumnMajor) resizeAxes(def, data.numColumns_, data.numRows_);
	else resizeAxes(def, data.numRows_, data.numColumns_);

	// Set new grid size
	data.itemGrid_.resize(data.numColumns_);
	data.itemPositionWithinLayout_.resize(data.numColumns_);
	for (auto x = 0; x < data.numColumns_; ++x)
	{
		data.itemGrid_[x].resize(data.numRows_);
		data.itemPositionWithinLayout_[x].resize(data.numRows_);
	}

	for (int major = 0; major<def.size(); ++major)
		for (int minor = 0; minor<def[major].size(); ++minor)
		{
			if (def[major][minor])
			{
				//We try to find an existing visualization of our current node, and create a new one if we don't
				auto existingIterator = existingItems.find(def[major][minor]);
				Item* value;
				if (existingIterator != existingItems.end() && !existingIterator.value().isEmpty())
					value = existingIterator.value().takeFirst();
				else
					value = create(def[major][minor]);

				if (majorAxis_ == GridLayouter::ColumnMajor) data.itemGrid_[major][minor] = value;
				else data.itemGrid_[minor][major] = value;

				// Note that we don't need to update the position here since it will be overwritten anyway by
				// computeSize()
			}
		}
}

void DynamicGridFormElement::setItemPositions(Item* item, int parentX, int parentY)
{
	auto& data = dataForItem(item);

	for (int xIt = 0; xIt<data.numColumns_; ++xIt)
		for (int yIt = 0; yIt < data.numRows_; ++yIt)
			if (auto childItem = data.itemGrid_[xIt][yIt])
			{
				// Note that margins have already been accounted for.
				QPointF newPos{parentX + x(item) + data.itemPositionWithinLayout_[xIt][yIt].x(),
							parentY + y(item) + data.itemPositionWithinLayout_[xIt][yIt].y()};

				if (newPos != childItem->pos())
					childItem->setPos(newPos); // setting the position is an expensive operation so only do it if it changed
			}
}

void DynamicGridFormElement::destroyChildItems(Item* item,
										 QList<const Item* const DeclarativeItemBase::*> handledChildren)
{
	LayoutFormElement::destroyChildItems(item, handledChildren);

	auto dataIterator = itemData_.find(item);
	if (dataIterator != itemData_.end())
	{
		for (auto list : dataIterator.value()->itemGrid_)
			for (auto item : list)
				SAFE_DELETE_ITEM(item);

		SAFE_DELETE(dataIterator.value());
		itemData_.erase(dataIterator);
	}
}

QPoint DynamicGridFormElement::focusedElementIndex(Item* item) const
{
	auto& data = dataForItem(item);

	for (int x=0; x<data.numColumns_; ++x)
		for (int y=0; y<data.numRows_; ++y)
			if ( data.itemGrid_[x][y] && data.itemGrid_[x][y]->itemOrChildHasFocus()) return QPoint(x, y);

	return QPoint(-1, -1);
}

QPoint DynamicGridFormElement::indexOf(Item* item, Item* child) const
{
	Q_ASSERT(item);
	Q_ASSERT(child);

	auto directlyOwnedItem = child;
	while (directlyOwnedItem->parent() && directlyOwnedItem->parent() != item )
		directlyOwnedItem = directlyOwnedItem->parent();

	if (directlyOwnedItem->parent() != item) return {-1, -1};

	auto& data = dataForItem(item);

	for (int x=0; x<data.numColumns_; ++x)
		for (int y=0; y<data.numRows_; ++y)
			if ( data.itemGrid_[x][y] == directlyOwnedItem)
				return {x, y};

	Q_ASSERT(false);
}

bool DynamicGridFormElement::elementOrChildHasFocus(Item* item) const
{
	if (LayoutFormElement::elementOrChildHasFocus(item))
		return true;
	for (auto list : dataForItem(item).itemGrid_)
		for (auto item : list)
			if (item && item->itemOrChildHasFocus())
				return true;
	return false;
}

QList<ItemRegion> DynamicGridFormElement::regions(DeclarativeItemBase* item, int parentX, int parentY)
{
	auto& data = dataForItem(item);

	return GridLayouter::regions(item, this, parentX + x(item), parentY + y(item), majorAxis_, true, true, true,
			item->style()->extraCursorsOutsideShape(), true, true,
			[&data](){return data.numRows_;},	// numRows
			[&data](){return data.numColumns_;},	// numColumns
			[&data](int x, int y){return data.itemGrid_[x][y];},	// has
			[&data](int x, int y){return data.itemGrid_[x][y]->widthInParent();},	// width
			[&data](int x, int y){return data.itemGrid_[x][y]->heightInParent();},	// height
			[&data](int x, int y){return (int) data.itemGrid_[x][y]->x();},	// xPos
			[&data](int x, int y){return (int) data.itemGrid_[x][y]->y();},	// yPos
			[&data](int x, int y){return data.itemGrid_[x][y];},	// childItem
			[this](){return spaceBetweenRows_;},	// spaceBetweenRows
			[this](){return spaceBetweenColumns_;},	// spaceBetweenColumns
			[this](){return topMargin();},	// topMargin
			[this](){return bottomMargin();},	// bottomMargin
			[this](){return leftMargin();},	// leftMargin
			[this](){return rightMargin();}	// rightMargin
		);
}

} // namespace Visualization
