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

GridLayoutElement::GridLayoutElement()
: numColumns_(1), numRows_(1), spaceBetweenColumns_{}, spaceBetweenRows_{},
  defaultHorizontalAlignment_{LayoutStyle::Alignment::Left}, defaultVerticalAlignment_{LayoutStyle::Alignment::Top},
  defaultColumnStretchFactor_{0}, defaultRowStretchFactor_{0}
{
	// initialize element grid
	elementGrid_ = QVector<QVector<Element*>>(numColumns_, QVector<Element*>(numRows_));

	// initialize span grid
	spanGrid_ = QVector<QVector<QPair<int, int>>>(numColumns_, QVector<QPair<int, int>>(numRows_));

	// initialize alignments
	defaultColumnHorizontalAlignments_ = QVector<LayoutStyle::Alignment>(numColumns_, defaultVerticalAlignment_);
	defaultRowVerticalAlignments_ = QVector<LayoutStyle::Alignment>(numRows_, defaultHorizontalAlignment_);
	cellVerticalAlignmentGrid_ = QVector<QVector<LayoutStyle::Alignment>>(numColumns_,
			QVector<LayoutStyle::Alignment>(numRows_, defaultVerticalAlignment_));
	cellHorizontalAlignmentGrid_ = QVector<QVector<LayoutStyle::Alignment>>(numColumns_,
			QVector<LayoutStyle::Alignment>(numRows_, defaultHorizontalAlignment_));

	// initialize stretch factors
	columnStretchFactors_ = QVector<float>(numColumns_, defaultColumnStretchFactor_);
	rowStretchFactors_ = QVector<float>(numRows_, defaultRowStretchFactor_);
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

GridLayoutElement* GridLayoutElement::put(int column, int row, Element* element, int columnSpan, int rowSpan)
{
	adjustSize(column + columnSpan - 1, row + rowSpan - 1);

	SAFE_DELETE(elementGrid_[column][row]);
	elementGrid_[column][row] = element;
	spanGrid_[column][row] = QPair<int, int>(columnSpan, rowSpan);
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

	// modify widest cells in columns and tallest cells in rows if there are merged columns
	if (hasMultiColumn or hasMultiRow)
		for(int x=0; x<numColumns_; x++)
			for(int y=0; y<numRows_; y++)
			{
				if (spanGrid_[x][y].first > 1)
				{
					int availableSpace = 0;
					float availableStretchFactor = 0;
					for (int column=x; column<x+spanGrid_[x][y].first; column++)
					{
						availableSpace += widestInColumn[column];
						availableStretchFactor += columnStretchFactors_[column];
					}
					availableSpace += spaceBetweenColumns_ * (spanGrid_[x][y].first - 1);

					int missingSpace = elementGrid_[x][y]->size().width() - availableSpace;
					if (missingSpace > 0)
					{
						if (availableStretchFactor == 0)
						{
							// add the additional space to some column
							widestInColumn[x] += missingSpace;
						}
						else
						{
							// distribute the additional space according to the stretch factors
							for (int column=x; column<x+spanGrid_[x][y].first; column++)
								widestInColumn[column] +=
										std::ceil(missingSpace / availableStretchFactor * columnStretchFactors_[column]);
						}
					}
				}
				if (spanGrid_[x][y].second > 1)
				{
					int availableSpace = 0;
					float availableStretchFactor = 0;
					for (int row=y; row<y+spanGrid_[x][y].second; row++)
					{
						availableSpace += tallestInRow[row];
						availableStretchFactor += rowStretchFactors_[row];
					}
					availableSpace += spaceBetweenRows_ * (spanGrid_[x][y].second - 1);

					int missingSpace = elementGrid_[x][y]->size().height() - availableSpace;
					if (missingSpace > 0)
					{
						if (availableStretchFactor == 0)
						{
							// add the additional space to some column
							tallestInRow[y] += missingSpace;
						}
						else
						{
							// distribute the additional space according to the stretch factors
							for (int row=y; row<y+spanGrid_[x][y].second; row++)
								tallestInRow[row] +=
										std::ceil(missingSpace / availableStretchFactor * rowStretchFactors_[row]);
						}
					}
				}
	}

	// Compute grid size
	int totalWidth = 0;
	int totalHeight = 0;

	// Compute grid width
	for (int x = 0; x<numColumns_; ++x) totalWidth += widestInColumn[x];
	if (numColumns_ > 0) totalWidth += leftMargin() + rightMargin();
	if (numColumns_ > 1) totalWidth += spaceBetweenColumns_ * (numColumns_ - 1);

	// Adjust widest cell in column values if there is additional space available
	int additionalWidth = availableWidth - totalWidth;
	// if availableWidth == 0, this is always false
	if (additionalWidth > 0)
	{
		if (overallColumnStretchFactor_ > 0)
		{
			// distribute the additional space according to the stretch factors
			for (int x = 0; x<numColumns_; ++x)
				widestInColumn[x] += additionalWidth / overallColumnStretchFactor_ * columnStretchFactors_[x];
		}
		totalWidth = availableWidth;
	}

	// Compute grid height
	for (int y = 0; y<numRows_; ++y) totalHeight += tallestInRow[y];
	if (numRows_ > 0) totalHeight += topMargin() + bottomMargin();
	if (numRows_ > 1) totalHeight += spaceBetweenRows_ * (numRows_ - 1);

	// Adjust tallest cell in row values if there is additional space available
	int additionalHeight = availableHeight - totalHeight;
	// if availableHeight == 0, this is always false
	if (additionalHeight > 0)
	{
		if (overallRowStretchFactor_ > 0)
		{
			// distribute the additional space according to the stretch factors
			for (int y = 0; y<numRows_; ++y)
				tallestInRow[y] += additionalHeight / overallRowStretchFactor_ * rowStretchFactors_[y];
		}
		totalHeight = availableHeight;
	}

	setSize(QSize(totalWidth, totalHeight));

	// Recompute all the element's sizes, if their size depends on their parent's size
	for(int x=0; x<numColumns_; x++)
	{
		for(int y=0; y<numRows_; y++)
			if (elementGrid_[x][y] != nullptr && elementGrid_[x][y]->sizeDependsOnParent(item))
			{
				Element* element = elementGrid_[x][y];
				QPair<int, int> cellSpan = spanGrid_[x][y];
				if (cellSpan.first == 1 && cellSpan.second == 1)
					element->computeSize(item, widestInColumn[x], tallestInRow[y]);
				else
				{
					int localAvailableWidth = 0;
					for (int column=x; column<x+spanGrid_[x][y].first; column++)
						localAvailableWidth += widestInColumn[column];

					int localAvailableHeight = 0;
					for (int row=y; row<y+spanGrid_[x][y].second; row++)
						localAvailableHeight += tallestInRow[row];
					element->computeSize(item, localAvailableWidth, localAvailableHeight);
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
				if (cellHorizontalAlignmentGrid_[x][y] == LayoutStyle::Alignment::Center)
				{
					if (spanGrid_[x][y].first == 1)
						xPos += (widestInColumn[x] - elementGrid_[x][y]->size().width())/2;
					else
					{
						int localAvailableWidth = 0;
						for (int column=x; column<x+spanGrid_[x][y].first; column++)
							localAvailableWidth += widestInColumn[column];
						xPos += (localAvailableWidth - elementGrid_[x][y]->size().width())/2;
					}
				}
				else if (cellHorizontalAlignmentGrid_[x][y] == LayoutStyle::Alignment::Right)
				{
					if (spanGrid_[x][y].first == 1)
						xPos += (widestInColumn[x] - elementGrid_[x][y]->size().width());
					else
					{
						int localAvailableWidth = 0;
						for (int column=x; column<x+spanGrid_[x][y].first; column++)
							localAvailableWidth += widestInColumn[column];
						xPos += (localAvailableWidth - elementGrid_[x][y]->size().width());
					}
				}

				int yPos = top;
				if (cellVerticalAlignmentGrid_[x][y] == LayoutStyle::Alignment::Center)
				{
					if (spanGrid_[x][y].second == 1)
						yPos += (tallestInRow[y] - elementGrid_[x][y]->size().height())/2;
					else
					{
						int localAvailableHeight = 0;
						for (int row=y; row<y+spanGrid_[x][y].second; row++)
							localAvailableHeight += tallestInRow[row];
						yPos += (localAvailableHeight - elementGrid_[x][y]->size().height())/2;
					}
				}
				else if (cellVerticalAlignmentGrid_[x][y] == LayoutStyle::Alignment::Bottom)
				{
					if (spanGrid_[x][y].second == 1)
						yPos += tallestInRow[y] - elementGrid_[x][y]->size().height();
					else
					{
						int localAvailableHeight = 0;
						for (int row=y; row<y+spanGrid_[x][y].second; row++)
							localAvailableHeight += tallestInRow[row];
						yPos += localAvailableHeight - elementGrid_[x][y]->size().height();
					}
				}

				elementGrid_[x][y]->setPos(QPoint(xPos, yPos));
			}

			top += tallestInRow[y] + spaceBetweenRows_;
		}

		left += widestInColumn[x] + spaceBetweenColumns_;
	}
}

bool GridLayoutElement::sizeDependsOnParent(const Item*) const
{
	return overallColumnStretchFactor_ > 0 || overallRowStretchFactor_ > 0;
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

void GridLayoutElement::adjustSize(int containColumn, int containRow)
{
	if (containColumn >= numColumns_ || containRow >= numRows_)
	{
		// calculate new size
		int newNumColumns = (containColumn < numColumns_) ? numColumns_ : containColumn + 1;
		int newNumRows = (containRow < numRows_) ? numRows_ : containRow + 1;

		// adjust element grid
		auto newElementGrid = QVector<QVector<Element*>>(newNumColumns, QVector<Element*>(newNumRows, nullptr));
		for (int x=0; x<numColumns_; x++)
			for (int y=0; y<numRows_; y++)
				newElementGrid[x][y] = elementGrid_[x][y];
		elementGrid_ = newElementGrid;

		// adjust span grid
		auto newSpanGrid = QVector<QVector<QPair<int, int>>>(newNumColumns, QVector<QPair<int, int>>(newNumRows));
		for (int x=0; x<numColumns_; x++)
			for (int y=0; y<numRows_; y++)
				newSpanGrid[x][y] = spanGrid_[x][y];
		spanGrid_ = newSpanGrid;

		// adjust alignments
		if (numColumns_ < newNumColumns)
		{
			auto newColumnAlignments = QVector<LayoutStyle::Alignment>(newNumColumns, defaultHorizontalAlignment_);
			for (int x=0; x<numColumns_; x++)
				newColumnAlignments[x] = defaultColumnHorizontalAlignments_[x];
			defaultColumnHorizontalAlignments_ = newColumnAlignments;
		}

		if (numRows_ < newNumRows)
		{
			auto newRowAlignments = QVector<LayoutStyle::Alignment>(newNumRows, defaultVerticalAlignment_);
			for (int y=0; y<numRows_; y++)
				newRowAlignments[y] = defaultRowVerticalAlignments_[y];
			defaultRowVerticalAlignments_ = newRowAlignments;
		}

		auto newCellHorizontalGrid = QVector<QVector<LayoutStyle::Alignment>>(newNumColumns,
				QVector<LayoutStyle::Alignment>(newNumRows));
		for (int x=0; x<newNumColumns; x++)
			for (int y=0; y<newNumRows; y++)
				if (x<numColumns_ && y<numRows_)
					newCellHorizontalGrid[x][y] = cellHorizontalAlignmentGrid_[x][y];
				else
				{
					newCellHorizontalGrid[x][y] = defaultColumnHorizontalAlignments_[x];
				}
		cellHorizontalAlignmentGrid_ = newCellHorizontalGrid;

		auto newCellVerticalGrid = QVector<QVector<LayoutStyle::Alignment>>(newNumColumns,
				QVector<LayoutStyle::Alignment>(newNumRows));
		for (int x=0; x<newNumColumns; x++)
			for (int y=0; y<newNumRows; y++)
				if (x<numColumns_ && y<numRows_)
					newCellVerticalGrid[x][y] = cellVerticalAlignmentGrid_[x][y];
				else
					newCellVerticalGrid[x][y] = defaultRowVerticalAlignments_[y];
		cellVerticalAlignmentGrid_ = newCellVerticalGrid;

		// adjust stretch factors
		if (numColumns_ < newNumColumns)
		{
			auto newColumnStretchFactors = QVector<float>(newNumColumns, defaultColumnStretchFactor_);
			for (int x=0; x<numColumns_; x++)
				newColumnStretchFactors[x] = columnStretchFactors_[x];
			columnStretchFactors_ = newColumnStretchFactors;
		}

		if (numRows_ < newNumRows)
		{
			auto newRowStretchFactors = QVector<float>(newNumRows, defaultRowStretchFactor_);
			for (int y=0; y<numRows_; y++)
				newRowStretchFactors[y] = rowStretchFactors_[y];
			rowStretchFactors_ = newRowStretchFactors;
		}

		// set new grid size
		numColumns_ = newNumColumns;
		numRows_ = newNumRows;
	}
}

} /* namespace Visualization */
