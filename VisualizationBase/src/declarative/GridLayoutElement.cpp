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
	computeOverallStretchFactors();
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
	// Compute default sizes of all the elements
	// Get the widest and tallest items (without merged cells)
	QVector<int> widestInColumn(numColumns_, 0);
	QVector<int> tallestInRow(numRows_, 0);
	bool hasMultiColumn = false;
	bool hasMultiRow = false;
	for(int x=0; x<numColumns_; x++)
	{
		for(int y=0; y<numRows_; y++)
			if (elementGrid_[x][y] != nullptr)
			{
				Element* element = elementGrid_[x][y];
				QPair<int, int> cellSpan = spanGrid_[x][y];
				element->computeSize(item, 0, 0); // any additional space is distributed later
				if (cellSpan.first == 1)
				{
					if (element->size().width() > widestInColumn[x]) widestInColumn[x] = element->size().width();
				}
				else
					hasMultiColumn = true;
				if (cellSpan.second == 1)
				{
					if (element->size().height() > tallestInRow[y]) tallestInRow[y] = element->size().height();
				}
				else
					hasMultiRow = true;
			}
	}

	// Compute grid size
	int totalWidth = 0;
	int totalHeight = 0;

	// No merged cells on X-axis
	if (!hasMultiColumn)
	{
		for (int x = 0; x<numColumns_; ++x) totalWidth += widestInColumn[x];
		if (numColumns_ > 0) totalWidth += leftMargin() + rightMargin();
		if (numColumns_ > 1) totalWidth += spaceBetweenColumns_ * (numColumns_ - 1);

		int additionalSpace = availableWidth - totalWidth;
		// if availableWidth == 0, this is always false
		if (additionalSpace > 0)
		{
			if (overallColumnStretchFactor_ == 0)
			{
				// add the additional space to some column
				widestInColumn[numColumns_ - 1] += additionalSpace;
			}
			else
			{
				// distribute the additional space according to the stretch factors
				for (int x = 0; x<numColumns_; ++x)
					widestInColumn[x] += additionalSpace / overallColumnStretchFactor_ * columnStretchFactors_[x];
			}
			totalWidth = availableWidth;
		}
	}
	// TODO: else

	// No merged cells on Y-axis
	if (!hasMultiRow)
	{
		for (int y = 0; y<numRows_; ++y) totalHeight += tallestInRow[y];
		if (numRows_ > 0) totalHeight += topMargin() + bottomMargin();
		if (numRows_ > 1) totalHeight += spaceBetweenRows_ * (numRows_ - 1);

		int additionalSpace = availableHeight - totalHeight;
		// if availableHeight == 0, this is always false
		if (additionalSpace > 0)
		{
			if (overallRowStretchFactor_ == 0)
			{
				// add the additional space to some row
				tallestInRow[numRows_ - 1] += additionalSpace;
			}
			else
			{
				// distribute the additional space according to the stretch factors
				for (int y = 0; y<numRows_; ++y)
					tallestInRow[y] += additionalSpace / overallRowStretchFactor_ * rowStretchFactors_[y];
			}
			totalHeight = availableHeight;
		}
	}
	// TODO: else

	setSize(QSize(totalWidth, totalHeight));

	// recompute all the sizes, if they matter now
	// TODO: is this assumption correct, or do I need to recompute in every case?
	if (availableWidth > 0 or availableHeight > 0)
	{
		for(int x=0; x<numColumns_; x++)
		{
			for(int y=0; y<numRows_; y++)
				if (elementGrid_[x][y] != nullptr)
				{
					Element* element = elementGrid_[x][y];
					QPair<int, int> cellSpan = spanGrid_[x][y];
					if (cellSpan.first == 1 and cellSpan.second == 1)
						element->computeSize(item, widestInColumn[x], tallestInRow[y]);
					// TODO: else
				}
		}
	}

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

void GridLayoutElement::computeOverallStretchFactors()
{
	overallColumnStretchFactor_ = 0;
	for(int x=0; x<numColumns_; ++x)
	{
		overallColumnStretchFactor_ += columnStretchFactors_[x];
	}
	overallRowStretchFactor_ = 0;
	for(int y=0; y<numRows_; ++y)
	{
		overallRowStretchFactor_ += rowStretchFactors_[y];
	}
}

} /* namespace Visualization */
