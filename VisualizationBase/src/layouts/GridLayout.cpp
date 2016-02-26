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

#include "GridLayout.h"
#include "../shapes/Shape.h"
#include "../items/ItemWithNode.h"
#include "../renderer/ModelRenderer.h"

#include "ModelBase/src/nodes/Node.h"

namespace Visualization {

DEFINE_ITEM_COMMON( GridLayout, "layout")

GridLayout::GridLayout(Item* parent, const StyleType* style) :
Super{parent, style}, sizeX_{0}, sizeY_{0}
{
}

GridLayout::~GridLayout()
{
	for (int x = 0; x<sizeX_; x++)
		for (int y = 0; y<sizeY_; y++) SAFE_DELETE_ITEM( items_[x][y] );
}

void GridLayout::setGridSize(int sizeX, int sizeY, bool deleteExtraItems)
{
	if (sizeX < sizeX_)
	{
		for (int x = sizeX; x < sizeX_; x++)
			for (int y = 0; y < sizeY_; y++)
				if (deleteExtraItems) SAFE_DELETE_ITEM( items_[x][y] );
				else if ( items_[x][y] ) items_[x][y]->setParentItem(nullptr);
	}

	sizeX_ = sizeX;
	items_.resize(sizeX_);

	for (int x = 0; x < sizeX_; ++x)
	{
		if (sizeY < sizeY_ )
		{
			for (int y = sizeY; y<items_[x].size(); ++y)
				if (deleteExtraItems) SAFE_DELETE_ITEM( items_[x][y] );
				else if ( items_[x][y] ) items_[x][y]->setParentItem(nullptr);
		}

		items_[x].resize(sizeY);
	}

	sizeY_ = sizeY;
}

bool GridLayout::isEmpty(int x, int y) const
{
	return x>= sizeX_ || y >= sizeY_ || items_[x][y] == nullptr;
}

void GridLayout::set(Item* item, int x, int y, bool deleteOldItem)
{
	if (item) item->setParentItem(this);
	if (deleteOldItem) SAFE_DELETE_ITEM( items_[x][y]);
	else if (items_[x][y]) items_[x][y]->setParentItem(nullptr);
	items_[x][y] = item;
	setUpdateNeeded(StandardUpdate);
}

void GridLayout::swap(int x1, int y1, int x2, int y2)
{
	Item* t = items_[x1][y1];
	items_[x1][y1] = items_[x2][y2];
	items_[x2][y2] = t;
	setUpdateNeeded(StandardUpdate);
}

void GridLayout::remove(int x, int y, bool deleteItem)
{
	if (deleteItem) SAFE_DELETE_ITEM( items_[x][y]);
	else if (items_[x][y]) items_[x][y]->setParentItem(nullptr);
	items_[x][y] = nullptr;
	setUpdateNeeded(StandardUpdate);
}

void GridLayout::remove(Item* item, bool deleteItem)
{
	if (item == nullptr) return;

	for (int x = 0; x<sizeX_; ++x)
		for (int y = 0; y<sizeY_; ++y)
			if (items_[x][y] == item) items_[x][y] = nullptr;

	if (deleteItem) SAFE_DELETE_ITEM(item);
	else item->setParentItem(nullptr);
	setUpdateNeeded(StandardUpdate);
}

void GridLayout::clear(bool deleteItems)
{
	for (int x = 0; x<sizeX_; ++x)
		for (int y = 0; y<sizeY_; ++y)
		{
			if (deleteItems) SAFE_DELETE_ITEM(items_[x][y]);
			else if (items_[x][y]) items_[x][y]->setParentItem(nullptr);

			items_[x][y] = nullptr;
		}
	setUpdateNeeded(StandardUpdate);
}

void GridLayout::synchronizeWithNodes(const QList< QList<Model::Node*> >& nodes)
{
	// Remove all elements from the current grid that do not match a node
	// If a node exists record it's location
	QList<Model::Node*> nodesFound;
	QList<Item*> itemsFound;

	for (int x = 0; x < sizeX_; ++x)
		for (int y = 0; y < sizeY_; ++y)
			if (items_[x][y] != nullptr)
			{
				bool found = false;

				for (int n = 0; n < nodes.size(); ++n)
					if (nodes[n].contains(items_[x][y]->node()))
					{
						found = true;
						synchronizeItem(items_[x][y], items_[x][y]->node());
						nodesFound.append(items_[x][y]->node());
						itemsFound.append(items_[x][y]);
						break;
					}

				if (!found) SAFE_DELETE_ITEM(items_[x][y]);
				items_[x][y] = nullptr;
			}

	// Get the new size.
	int newY = nodes.size();
	int newX = 0;
	for (int y = 0; y<newY; ++y)
		if (newX < nodes[y].size()) newX = nodes[y].size();
	setGridSize(newX, newY, false);

	for (int y = 0; y<nodes.size(); ++y)
		for (int x = 0; x<nodes[y].size(); ++x)
		{
			int oldIndex = nodesFound.indexOf(nodes[y][x]);
			if (oldIndex >=0) set(itemsFound[oldIndex], x, y, false);
			else set(renderer()->render(this, nodes[y][x]), x, y, false);
		}
}

bool GridLayout::isEmpty() const
{
	for (int x = 0; x<sizeX_; ++x)
		for (int y = 0; y<sizeY_; ++y)
			if (!items_[x][y]->isEmpty()) return false;

	return true;
}

void GridLayout::updateGeometry(int, int)
{
	// Get the widest and tallest items
	QVector<int> widestInColumn(sizeX_, 0);
	QVector<int> tallestInRow(sizeY_, 0);
	for (int x=0; x<sizeX_; ++x)
	{
		for (int y=0; y<sizeY_; ++y)
			if (items_[x][y] != nullptr)
			{
				if (items_[x][y]->widthInParent() > widestInColumn[x]) widestInColumn[x] = items_[x][y]->widthInParent();
				if (items_[x][y]->heightInParent() > tallestInRow[y]) tallestInRow[y] = items_[x][y]->heightInParent();
			}
	}

	// Compute size
	int totalWidth = 0;
	for (int x = 0; x<sizeX_; ++x) totalWidth += widestInColumn[x];
	if (sizeX_ > 1) totalWidth += (sizeX_-1)*style()->horizontalSpace();

	int totalHeight = 0;
	for (int y = 0; y<sizeY_; ++y) totalHeight += tallestInRow[y];
	if (sizeY_ > 1) totalHeight += (sizeY_-1)*style()->verticalSpace();

	setInnerSize(totalWidth, totalHeight);

	// Set item positions

	int left = xOffset();
	for (int x=0; x<sizeX_; ++x)
	{
		int top = yOffset();
		for (int y=0; y<sizeY_; ++y)
		{
			if (items_[x][y] != nullptr)
			{
				int xPos = left;
				if (style()->horizontalAlignment() == LayoutStyle::Alignment::Center)
					xPos += (widestInColumn[x] - items_[x][y]->widthInParent())/2;
				else if (style()->horizontalAlignment() == LayoutStyle::Alignment::Right)
					xPos += (widestInColumn[x] - items_[x][y]->widthInParent());

				int yPos = top;
				if (style()->verticalAlignment() == LayoutStyle::Alignment::Center)
					yPos += (tallestInRow[y] - items_[x][y]->heightInParent())/2;
				else if (style()->verticalAlignment() == LayoutStyle::Alignment::Bottom)
					yPos += (tallestInRow[y] - items_[x][y]->heightInParent());

				items_[x][y]->setPos(xPos, yPos);
			}

			top += tallestInRow[y];
			top += style()->verticalSpace();
		}

		left += widestInColumn[x];
		left += style()->horizontalSpace();
	}
}

void GridLayout::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Super::paint(painter, option, widget);
	if (style()->borderPen().style() == Qt::NoPen)
		return;

	// Get the widest and tallest items
	QVector<int> widestInColumn(sizeX_, 0);
	QVector<int> tallestInRow(sizeY_, 0);
	for (int x=0; x<sizeX_; ++x)
	{
		for (int y=0; y<sizeY_; ++y)
			if (items_[x][y] != nullptr)
			{
				if (items_[x][y]->widthInParent() > widestInColumn[x]) widestInColumn[x] = items_[x][y]->widthInParent();
				if (items_[x][y]->heightInParent() > tallestInRow[y]) tallestInRow[y] = items_[x][y]->heightInParent();
			}
	}

	painter->setPen(style()->borderPen());

	int sum = 0;
	for (int i = 0; i < widestInColumn.size()-1; i++)
	{
		sum += widestInColumn.at(i);
		painter->drawLine(sum+((style()->horizontalSpace())/2), 0,
								sum+((style()->horizontalSpace())/2), heightInParent());
		sum += style()->horizontalSpace();
	}

	sum = 0;
	for (int i = 0; i < tallestInRow.size()-1; i++)
	{
		sum += tallestInRow.at(i);
		painter->drawLine(0, sum+((style()->verticalSpace())/2), widthInParent(), sum+((style()->verticalSpace())/2));
		sum += style()->verticalSpace();
	}

}

QPoint GridLayout::focusedElementIndex() const
{
	for (int x=0; x<sizeX_; ++x)
		for (int y=0; y<sizeY_; ++y)
			if ( items_[x][y] && items_[x][y]->itemOrChildHasFocus()) return QPoint{x, y};

	return QPoint{-1, -1};
}

Item* GridLayout::findFirstVertical(bool startFromTop)
{
	for (int y = 0; y<sizeY_; ++y)
		for (int x = 0; x<sizeX_; ++x)
		{
			int yIndex = startFromTop ? y : (sizeY_-y-1);
			if (items_[x][yIndex] != nullptr) return items_[x][yIndex];
		}

	return nullptr;
}

Item* GridLayout::findFirstHorizontal(bool startFromLeft)
{
	for (int x = 0; x<sizeX_; ++x)
		for (int y = 0; y<sizeY_; ++y)
		{
			int xIndex = startFromLeft ? x : (sizeX_-x-1);
			if (items_[xIndex][y] != nullptr) return items_[xIndex][y];
		}

	return nullptr;
}

Item* GridLayout::findNext(int dx, int dy)
{
	QPoint current = focusedElementIndex();
	if (current.x() < 0 || current.y() < 0) return nullptr;

	int x = current.x() + dx;
	int y = current.y() + dy;

	while (x>= 0 && x < sizeX_ && y>=0 && y < sizeY_)
	{
		if (items_[x][y] != nullptr) return items_[x][y];

		x+= dx;
		y+= dy;
	}

	return nullptr;
}

}
