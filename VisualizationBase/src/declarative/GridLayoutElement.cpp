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

GridLayoutElement::GridLayoutElement(int numColumns, int numRows)
: numColumns_(numColumns), numRows_(numRows), spaceBetweenColumns_{}, spaceBetweenRows_{},
  horizontalAlignment_{LayoutStyle::Alignment::Left}, verticalAlignment_{LayoutStyle::Alignment::Top}
{
	elementGrid_ = QVector<QVector<Element*>>(numColumns_, QVector<Element*>(numRows_));
	spanGrid_ = QVector<QVector<QPair<int, int>>>(numColumns_, QVector<QPair<int, int>>(numRows_));
	columnStretchFactors_ = QVector<float>(numColumns_, 0);
	rowStretchFactors_ = QVector<float>(numRows_, 0);
}

GridLayoutElement::~GridLayoutElement()
{
	for(int x=0; x<numColumns_; x++)
		for(int y=0; y<numRows_; y++)
			SAFE_DELETE(elementGrid_[x][y]);
}

void GridLayoutElement::destroyChildItems(Item* item)
{
	for(int x=0; x<numColumns_; x++)
		for(int y=0; y<numRows_; y++)
			elementGrid_[x][y]->destroyChildItems(item);
}

GridLayoutElement* GridLayoutElement::addElement(Element* element, int column, int row, int columnSpan, int rowSpan)
{
	SAFE_DELETE(elementGrid_[column][row]);
	elementGrid_[column][row] = element;
	spanGrid_[column][row] = QPair<int, int>(columnSpan, rowSpan);
	// TODO: How can I make sure that row and column span are valid? Can I use assert?
	return this;
}

void GridLayoutElement::synchronizeWithItem(Item* item)
{
	for(int x=0; x<numColumns_; x++)
	{
		for(int y=0; y<numRows_; y++)
			if (elementGrid_[x][y] != nullptr)
			{
				elementGrid_[x][y]->synchronizeWithItem(item);
			}
	}
}

void GridLayoutElement::setItemPositions(Item* item, int parentX, int parentY)
{
	for(int x=0; x<numColumns_; x++)
	{
		for(int y=0; y<numRows_; y++)
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
	QVector<int> widestInColumn(numColumns_, 0);
	QVector<int> tallestInRow(numRows_, 0);
	for(int x=0; x<numColumns_; x++)
	{
		for(int y=0; y<numRows_; y++)
			if (elementGrid_[x][y] != nullptr)
			{
				Element* element = elementGrid_[x][y];
				// TODO: do something better with the available width/height
				element->computeSize(item, availableWidth / numColumns_, availableHeight / numRows_);
				QSize size = element->size();
				if (size.width() > widestInColumn[x]) widestInColumn[x] = size.width();
				if (size.height() > tallestInRow[y]) tallestInRow[y] = size.height();
			}
	}

	// Compute size
	int totalWidth = 0;
	for (int x = 0; x<numColumns_; ++x) totalWidth += widestInColumn[x];
	if (numColumns_ > 0) totalWidth += leftMargin() + rightMargin();
	if (numColumns_ > 1) totalWidth += spaceBetweenColumns_ * (numColumns_ - 1);

	int totalHeight = 0;
	for (int y = 0; y<numRows_; ++y) totalHeight += tallestInRow[y];
	if (numRows_ > 0) totalHeight += topMargin() + bottomMargin();
	if (numRows_ > 1) totalHeight += spaceBetweenRows_ * (numRows_ - 1);

	setSize(QSize(totalWidth, totalHeight));

	// Set item positions

	int left = leftMargin();
	for(int x=0; x<numColumns_; ++x)
	{
		int top = topMargin();
		for(int y=0; y<numRows_; ++y)
		{
			if (elementGrid_[x][y] != nullptr)
			{
				int xPos = left;
				if (horizontalAlignment_ == LayoutStyle::Alignment::Center)
					xPos += (widestInColumn[x] - elementGrid_[x][y]->size().width())/2;
				else if (horizontalAlignment_ == LayoutStyle::Alignment::Right)
					xPos += (widestInColumn[x] - elementGrid_[x][y]->size().width());

				int yPos = top;
				if (verticalAlignment_ == LayoutStyle::Alignment::Center)
					yPos += (tallestInRow[y] - elementGrid_[x][y]->size().height())/2;
				else if (verticalAlignment_ == LayoutStyle::Alignment::Bottom)
					yPos += (tallestInRow[y] - elementGrid_[x][y]->size().height());

				elementGrid_[x][y]->setPos(QPoint(xPos, yPos));
			}

			top += tallestInRow[y] + spaceBetweenRows_;
		}

		left += widestInColumn[x] + spaceBetweenColumns_;
	}
}

} /* namespace Visualization */
