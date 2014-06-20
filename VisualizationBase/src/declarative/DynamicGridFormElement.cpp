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
#include "GridLayouter.h"

namespace Visualization {


DynamicGridFormElement::DynamicGridFormElement(const DynamicGridFormElement& other) : LayoutFormElement{other},
	nodesGetterFunction_{other.nodesGetterFunction_}, spanGetterFunction_{other.spanGetterFunction_},
	spaceBetweenColumns_{other.spaceBetweenColumns_}, spaceBetweenRows_{other.spaceBetweenRows_},
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
	return new DynamicGridFormElement(*this);
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
	QSize finalSize = GridLayouter::computeSize<true>(availableWidth, availableHeight,
		[&data](){return data.numRows_;},	// numRows
		[&data](){return data.numColumns_;},	// numColumns
		[&data](int x, int y){return data.itemGrid_[x][y];},	// has
		[&data](int x, int y){return data.itemSpan_[x][y];},	// spanGrid
		[&data](int x, int y){return data.itemGrid_[x][y]->widthInParent();},	// width
		[&data](int x, int y){return data.itemGrid_[x][y]->heightInParent();},	// height
		[](int, int, int, int){},	// computeElementSize
		[&data](int x, int y, int w, int h){data.itemGrid_[x][y]->changeGeometry(w, h);},	// changeGeometry
		[&data](int x, int y){return data.itemGrid_[x][y]->sizeDependsOnParent();},	// isStretchable
		[&data](int x, int y, QPoint pos){data.itemGrid_[x][y]->setPos(pos);},	// setPosition
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
	auto nodes = nodesGetterFunction_(item);
	auto& data = dataForItem(item);

	// Remove all elements from the current grid that do not match a node
	// If a node exists record it's location
	QList<Model::Node*> nodesFound;
	QList<Item*> itemsFound;

	for (int x = 0; x < data.numColumns_; ++x)
		for (int y = 0; y < data.numRows_; ++y)
			if (data.itemGrid_[x][y] != nullptr)
			{
				bool found = false;

				for (int n = 0; n < nodes.size(); ++n)
					if (nodes[n].contains(data.itemGrid_[x][y]->node()))
					{
						found = true;
						nodesFound.append(data.itemGrid_[x][y]->node());
						itemsFound.append(data.itemGrid_[x][y]);
						item->synchronizeItem(data.itemGrid_[x][y], data.itemGrid_[x][y]->node());
						break;
					}

				if (!found) SAFE_DELETE_ITEM(data.itemGrid_[x][y]);
				data.itemGrid_[x][y] = nullptr;
			}

	// Get the new size.
	int newY = nodes.size();
	int newX = 0;
	for (int y = 0; y<newY; ++y)
		if (newX < nodes[y].size()) newX = nodes[y].size();
	setGridSize(data, newX, newY);

	for (int y = 0; y<nodes.size(); ++y)
		for (int x = 0; x<nodes[y].size(); ++x)
		{
			int oldIndex = nodesFound.indexOf(nodes[y][x]);
			if (oldIndex >=0) data.itemGrid_[x][y] = itemsFound[oldIndex];
			else data.itemGrid_[x][y] = item->renderer()->render(item, nodes[y][x]);
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

	item->setUpdateNeeded(Item::StandardUpdate);
}

void DynamicGridFormElement::setGridSize(ItemData& data, int sizeX, int sizeY)
{
	if (sizeX < data.numColumns_)
	{
		for (int x = sizeX; x < data.numColumns_; x++)
			for (int y = 0; y < data.numRows_; y++)
				if ( data.itemGrid_[x][y] )
					data.itemGrid_[x][y]->setParentItem(nullptr);
	}

	data.numColumns_ = sizeX;
	data.itemGrid_.resize(data.numColumns_);

	for (int x = 0; x < data.numColumns_; ++x)
	{
		if (sizeY < data.numRows_ )
		{
			for (int y = sizeY; y<data.itemGrid_[x].size(); ++y)
				if ( data.itemGrid_[x][y] )
					data.itemGrid_[x][y]->setParentItem(nullptr);
		}

		data.itemGrid_[x].resize(sizeY);
	}

	data.numRows_ = sizeY;
}

void DynamicGridFormElement::setItemPositions(Item* item, int parentX, int parentY)
{
	for (auto list : dataForItem(item).itemGrid_)
		for (auto i : list)
			if (i) i->setPos(parentX + x(item) + i->x() + leftMargin(), parentY + y(item) + i->y() + topMargin());
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
		itemData_.remove(item);
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

} // namespace Visualization
