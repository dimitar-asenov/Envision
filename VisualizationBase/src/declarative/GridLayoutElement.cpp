/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "GridLayoutElement.h"

namespace Visualization {

GridLayoutElement::GridLayoutElement(int numHorizontalCells, int numVerticalCells)
: numHorizontalCells_(numHorizontalCells), numVerticalCells_(numVerticalCells),
  horizontalSpacing_{}, verticalSpacing_{}
{
	elementGrid_ = QVector<QVector<Element*>>(numHorizontalCells_, QVector<Element*>(numVerticalCells_));
}

GridLayoutElement::~GridLayoutElement()
{
	for(int x=0; x<numHorizontalCells_; x++)
		for(int y=0; y<numVerticalCells_; y++)
			SAFE_DELETE(elementGrid_[x][y]);
}

void GridLayoutElement::destroyChildItems(Item* item)
{
	for(int x=0; x<numHorizontalCells_; x++)
		for(int y=0; y<numVerticalCells_; y++)
			elementGrid_[x][y]->destroyChildItems(item);
}

void GridLayoutElement::add(int cellX, int cellY, Element* element)
{
	if (elementGrid_[cellX][cellY] != nullptr)
		SAFE_DELETE(elementGrid_[cellX][cellY]);
	elementGrid_[cellX][cellY] = element;
}

void GridLayoutElement::synchronizeWithItem(Item* item)
{
	for(int x=0; x<numHorizontalCells_; x++)
	{
		for(int y=0; y<numVerticalCells_; y++)
			if (elementGrid_[x][y] != nullptr)
			{
				elementGrid_[x][y]->synchronizeWithItem(item);
			}
	}
}

void GridLayoutElement::setItemPositions(Item* item, int parentX, int parentY)
{
	for(int x=0; x<numHorizontalCells_; x++)
	{
		for(int y=0; y<numVerticalCells_; y++)
			if (elementGrid_[x][y] != nullptr)
			{
				Element* element = elementGrid_[x][y];
				element->setItemPositions(item, parentX + pos().x(), parentY + pos().y());
			}
	}
}

void GridLayoutElement::computeSize(Item* item, int availableWidth, int availableHeight)
{
	// Get the widest and tallest items
	QVector<int> widestInColumn(numHorizontalCells_, 0);
	QVector<int> tallestInRow(numVerticalCells_, 0);
	for(int x=0; x<numHorizontalCells_; x++)
	{
		for(int y=0; y<numVerticalCells_; y++)
			if (elementGrid_[x][y] != nullptr)
			{
				Element* element = elementGrid_[x][y];
				// TODO: do something better with the available width/height
				element->computeSize(item, availableWidth / numHorizontalCells_, availableHeight / numVerticalCells_);
				QSize size = element->size();
				if (size.width() > widestInColumn[x]) widestInColumn[x] = size.width();
				if (size.height() > tallestInRow[y]) tallestInRow[y] = size.height();
			}
	}

	// Compute size
	int totalWidth = 0;
	for (int x = 0; x<numHorizontalCells_; ++x) totalWidth += widestInColumn[x];
	if (numHorizontalCells_ > 0) totalWidth += leftMargin() + rightMargin();
	if (numHorizontalCells_ > 1) totalWidth += horizontalSpacing_ * (numHorizontalCells_ - 1);

	int totalHeight = 0;
	for (int y = 0; y<numVerticalCells_; ++y) totalHeight += tallestInRow[y];
	if (numVerticalCells_ > 0) totalHeight += topMargin() + bottomMargin();
	if (numVerticalCells_ > 1) totalHeight += verticalSpacing_ * (verticalSpacing_ - 1);

	setSize(QSize(totalWidth, totalHeight));

	// Set item positions

	int left = leftMargin();
	for(int x=0; x<numHorizontalCells_; ++x)
	{
		int top = topMargin();
		for(int y=0; y<numVerticalCells_; ++y)
		{
			if (elementGrid_[x][y] != nullptr)
			{
				// assume alignment is top-left
				// TODO: generalize alignment
				int xPos = left;
//				if (style()->horizontalAlignment() == LayoutStyle::Alignment::Center) xPos += (widestInColumn[x] - items_[x][y]->width())/2;
//				else if (style()->horizontalAlignment() == LayoutStyle::Alignment::Right) xPos += (widestInColumn[x] - items_[x][y]->width());

				int yPos = top;
//				if (style()->verticalAlignment() == LayoutStyle::Alignment::Center) yPos += (tallestInRow[y] - items_[x][y]->height())/2;
//				else if (style()->verticalAlignment() == LayoutStyle::Alignment::Bottom) yPos += (tallestInRow[y] - items_[x][y]->height());

				elementGrid_[x][y]->setPos(QPoint(xPos, yPos));
			}

			top += tallestInRow[y] + verticalSpacing_;
		}

		left += widestInColumn[x] + horizontalSpacing_;
	}
}

} /* namespace Visualization */
