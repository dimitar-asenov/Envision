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
	nodesGetterFunction_{other.nodesGetterFunction_}, spanGetterFunction_{other.spanGetterFunction_},
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
	auto nodes = nodesGetterFunction_(item);
	auto& data = dataForItem(item);

	// Remove all elements from the current grid that do not match a node
	// If a node exists record it's location
	QHash<Model::Node*, Item*> existingItems;

	for (int x = 0; x < data.numColumns_; ++x)
		for (int y = 0; y < data.numRows_; ++y)
			if (data.itemGrid_[x][y] != nullptr)
			{
				bool found = false;

				for (int n = 0; n < nodes.size(); ++n)
					if (nodes[n].contains(data.itemGrid_[x][y]->node()))
					{
						found = true;
						item->synchronizeItem(data.itemGrid_[x][y], data.itemGrid_[x][y]->node());
						existingItems.insert(data.itemGrid_[x][y]->node(), data.itemGrid_[x][y]);
						break;
					}

				if (!found) SAFE_DELETE_ITEM(data.itemGrid_[x][y]);
				data.itemGrid_[x][y] = nullptr;
			}

	// Get the new size.
	auto resizeAxes = [](const QVector<QVector<Model::Node*>>& nodes, int& sizeMajor, int& sizeMinor){
		sizeMajor = nodes.size();
		sizeMinor = nodes.isEmpty() ? 0 : std::max_element(nodes.begin(), nodes.end(),
				[](const QVector<Model::Node*>& a, const QVector<Model::Node*>& b){return a.size() < b.size();})->size();
	};
	if (majorAxis_ == GridLayouter::ColumnMajor) resizeAxes(nodes, data.numColumns_, data.numRows_);
	else resizeAxes(nodes, data.numRows_, data.numColumns_);

	// Set new grid size
	data.itemGrid_.resize(data.numColumns_);
	data.itemPositionWithinLayout_.resize(data.numColumns_);
	for (auto x = 0; x < data.numColumns_; ++x)
	{
		data.itemGrid_[x].resize(data.numRows_);
		data.itemPositionWithinLayout_[x].resize(data.numRows_);
	}

	for (int major = 0; major<nodes.size(); ++major)
		for (int minor = 0; minor<nodes[major].size(); ++minor)
		{
			if (nodes[major][minor])
			{
				auto existingIterator = existingItems.find(nodes[major][minor]);
				auto value = existingIterator != existingItems.end()
						? existingIterator.value()
						: item->renderer()->render(item, nodes[major][minor]);

				if (majorAxis_ == GridLayouter::ColumnMajor) data.itemGrid_[major][minor] = value;
				else data.itemGrid_[minor][major] = value;

				// Note that we don't need to update the position here since it will be overwritten anyway by
				// computeSize()
			}
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

	return GridLayouter::regions(item, this, parentX + x(item), parentY + y(item), majorAxis_, true, false,
			item->style()->extraCursorsOutsideShape(), true, true,
			[&data](){return data.numRows_;},	// numRows
			[&data](){return data.numColumns_;},	// numColumns
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
