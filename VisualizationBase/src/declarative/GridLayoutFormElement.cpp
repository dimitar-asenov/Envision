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

#include "GridLayoutFormElement.h"

#include "DeclarativeItemBase.h"
#include "../items/ItemRegion.h"
#include "../cursor/LayoutCursor.h"

namespace Visualization {

GridLayoutFormElement::GridLayoutFormElement()
{
	// initialize element grid
	elementGrid_ = QVector<QVector<FormElement*>>(numColumns_, QVector<FormElement*>(numRows_));

	// initialize span grid
	spanGrid_ = QVector<QVector<QPair<int, int>>>(numColumns_,QVector<QPair<int, int>>(numRows_, QPair<int, int>(1, 1)));

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


GridLayoutFormElement::GridLayoutFormElement(const GridLayoutFormElement& other) : LayoutFormElement{other},
	// Copy everything and then clone all child FormElements
	numColumns_{other.numColumns_},
	numRows_{other.numRows_},
	spaceBetweenColumns_{other.spaceBetweenColumns_},
	spaceBetweenRows_{other.spaceBetweenRows_},

	lastCell_{other.lastCell_},

	elementGrid_{other.elementGrid_},
	spanGrid_{other.spanGrid_},

	defaultHorizontalAlignment_{other.defaultHorizontalAlignment_},
	defaultVerticalAlignment_{other.defaultVerticalAlignment_},
	defaultRowVerticalAlignments_{other.defaultRowVerticalAlignments_},
	defaultColumnHorizontalAlignments_{other.defaultColumnHorizontalAlignments_},
	cellHorizontalAlignmentGrid_{other.cellHorizontalAlignmentGrid_},
	cellVerticalAlignmentGrid_{other.cellVerticalAlignmentGrid_},

	defaultColumnStretchFactor_{other.defaultColumnStretchFactor_},
	defaultRowStretchFactor_{other.defaultRowStretchFactor_},
	columnStretchFactors_{other.columnStretchFactors_},
	rowStretchFactors_{other.rowStretchFactors_},
	overallColumnStretchFactor_{other.overallColumnStretchFactor_},
	overallRowStretchFactor_{other.overallRowStretchFactor_}
{
	// Adjust child Elements
	for(int x = 0; x<elementGrid_.size(); ++x)
		for(int y = 0; y<elementGrid_[x].size(); ++y)
			if (elementGrid_[x][y])
				{
					auto child = elementGrid_[x][y]->clone();
					elementGrid_[x][y] = child;
					addChild(child);
				}
}

GridLayoutFormElement::~GridLayoutFormElement()
{
	// elements were deleted by Element
}

GridLayoutFormElement* GridLayoutFormElement::clone() const
{
	return new GridLayoutFormElement(*this);
}

GridLayoutFormElement* GridLayoutFormElement::put(int column, int row, FormElement* element)
{
	auto toAdd = element->cloneIfAlreadyUsed();

	adjustSize(column, row);
	lastCell_ = QPair<int, int>(column, row);

	removeChild(elementGrid_[column][row]);
	addChild(toAdd);

	SAFE_DELETE(elementGrid_[column][row]);
	elementGrid_[column][row] = toAdd;
	return this;
}

void GridLayoutFormElement::computeSize(Item* item, int availableWidth, int availableHeight)
{
	// Compute default sizes of all the elements
	// Get the widest and tallest items (without merged cells)
	QVector<int> widestInColumn(numColumns_, 0);
	QVector<int> tallestInRow(numRows_, 0);
	bool hasMultiColumn = false;
	bool hasMultiRow = false;
	for(int x=0; x<numColumns_; x++)
		for(int y=0; y<numRows_; y++)
			if (elementGrid_[x][y] != nullptr)
			{
				FormElement* element = elementGrid_[x][y];
				QPair<int, int> cellSpan = spanGrid_[x][y];
				element->computeSize(item, 0, 0); // any additional space is distributed later
				if (cellSpan.first == 1)
				{
					if (element->width(item) > widestInColumn[x]) widestInColumn[x] = element->width(item);
				}
				else hasMultiColumn = true;
				if (cellSpan.second == 1)
				{
					if (element->height(item) > tallestInRow[y]) tallestInRow[y] = element->height(item);
				}
				else hasMultiRow = true;
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

					int missingSpace = elementGrid_[x][y]->width(item) - availableSpace;
					if (missingSpace > 0)
					{
						if (availableStretchFactor == 0) // add the additional space to some column
							widestInColumn[x] += missingSpace;
						else // distribute the additional space according to the stretch factors
							for (int column=x; column<x+spanGrid_[x][y].first; column++)
								widestInColumn[column] +=
										std::ceil(missingSpace / availableStretchFactor * columnStretchFactors_[column]);
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

					int missingSpace = elementGrid_[x][y]->height(item) - availableSpace;
					if (missingSpace > 0)
					{
						if (availableStretchFactor == 0) // add the additional space to some column
							tallestInRow[y] += missingSpace;
						else // distribute the additional space according to the stretch factors
							for (int row=y; row<y+spanGrid_[x][y].second; row++)
								tallestInRow[row] +=
										std::ceil(missingSpace / availableStretchFactor * rowStretchFactors_[row]);
					}
				}
			}

	// Compute grid size
	int totalWidth = 0;
	int totalHeight = 0;

	// Compute grid width
	for (auto columnWidth : widestInColumn) totalWidth += columnWidth;
	if (numColumns_ > 0) totalWidth += leftMargin() + rightMargin();
	if (numColumns_ > 1) totalWidth += spaceBetweenColumns_ * (numColumns_ - 1);

	// Adjust widest cell in column values if there is additional space available
	int additionalWidth = availableWidth - totalWidth;
	// if availableWidth == 0, this is always false
	if (additionalWidth > 0)
	{
		if (overallColumnStretchFactor_ > 0) // distribute the additional space according to the stretch factors
			for (int x = 0; x<numColumns_; ++x)
				widestInColumn[x] += std::floor(additionalWidth / overallColumnStretchFactor_ * columnStretchFactors_[x]);
		totalWidth = availableWidth;
	}

	// Compute grid height
	for (auto rowHeight : tallestInRow) totalHeight += rowHeight;
	if (numRows_ > 0) totalHeight += topMargin() + bottomMargin();
	if (numRows_ > 1) totalHeight += spaceBetweenRows_ * (numRows_ - 1);

	// Adjust tallest cell in row values if there is additional space available
	int additionalHeight = availableHeight - totalHeight;
	// if availableHeight == 0, this is always false
	if (additionalHeight > 0)
	{
		if (overallRowStretchFactor_ > 0) // distribute the additional space according to the stretch factors
			for (int y = 0; y<numRows_; ++y)
				tallestInRow[y] += std::floor(additionalHeight / overallRowStretchFactor_ * rowStretchFactors_[y]);
		totalHeight = availableHeight;
	}

	setSize(item, QSize(totalWidth, totalHeight));

	// Recompute all the element's sizes, if their size depends on their parent's size
	for(int x=0; x<numColumns_; x++)
		for(int y=0; y<numRows_; y++)
			if (elementGrid_[x][y] != nullptr && elementGrid_[x][y]->sizeDependsOnParent(item))
			{
				FormElement* element = elementGrid_[x][y];
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

	// Set element positions
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
						xPos += (widestInColumn[x] - elementGrid_[x][y]->width(item))/2;
					else
					{
						int localAvailableWidth = 0;
						for (int column=x; column<x+spanGrid_[x][y].first; column++)
							localAvailableWidth += widestInColumn[column];
						xPos += (localAvailableWidth - elementGrid_[x][y]->width(item))/2;
					}
				}
				else if (cellHorizontalAlignmentGrid_[x][y] == LayoutStyle::Alignment::Right)
				{
					if (spanGrid_[x][y].first == 1)
						xPos += (widestInColumn[x] - elementGrid_[x][y]->width(item));
					else
					{
						int localAvailableWidth = 0;
						for (int column=x; column<x+spanGrid_[x][y].first; column++)
							localAvailableWidth += widestInColumn[column];
						xPos += (localAvailableWidth - elementGrid_[x][y]->width(item));
					}
				}

				int yPos = top;
				if (cellVerticalAlignmentGrid_[x][y] == LayoutStyle::Alignment::Center)
				{
					if (spanGrid_[x][y].second == 1)
						yPos += (tallestInRow[y] - elementGrid_[x][y]->height(item))/2;
					else
					{
						int localAvailableHeight = 0;
						for (int row=y; row<y+spanGrid_[x][y].second; row++)
							localAvailableHeight += tallestInRow[row];
						yPos += (localAvailableHeight - elementGrid_[x][y]->height(item))/2;
					}
				}
				else if (cellVerticalAlignmentGrid_[x][y] == LayoutStyle::Alignment::Bottom)
				{
					if (spanGrid_[x][y].second == 1)
						yPos += tallestInRow[y] - elementGrid_[x][y]->height(item);
					else
					{
						int localAvailableHeight = 0;
						for (int row=y; row<y+spanGrid_[x][y].second; row++)
							localAvailableHeight += tallestInRow[row];
						yPos += localAvailableHeight - elementGrid_[x][y]->height(item);
					}
				}

				elementGrid_[x][y]->setPos(item, QPoint(xPos, yPos));
			}

			top += tallestInRow[y] + spaceBetweenRows_;
		}

		left += widestInColumn[x] + spaceBetweenColumns_;
	}
}

bool GridLayoutFormElement::sizeDependsOnParent(const Item*) const
{
	return overallColumnStretchFactor_ > 0 || overallRowStretchFactor_ > 0;
}

QList<ItemRegion> GridLayoutFormElement::regions(DeclarativeItemBase* item, int parentX, int parentY)
{
	QList<ItemRegion> allRegions;

	// regions for the child elements
	for(int x=0; x<numColumns_; x++)
		for(int y=0; y<numRows_; y++)
			if(elementGrid_[x][y])
				allRegions.append(elementGrid_[x][y]->regions(item, this->x(item) + parentX, this->y(item) + parentY));

	// if grid consists of exactly one row or one column, add more cursor regions
	if (numColumns_ == 1 || numRows_ == 1)
	{
		QRect wholeArea = QRect(QPoint(x(item) + parentX, y(item) + parentY), size(item));
		QRect elementsArea = QRect(QPoint(wholeArea.left() + leftMargin(), wholeArea.top() + topMargin()),
											QPoint(wholeArea.right() - rightMargin(), wholeArea.bottom() - bottomMargin()));

		// This is the rectangle half-way between the bounding box of the layout and elementsArea.
		QRect midArea = QRect(QPoint(wholeArea.left() + leftMargin()/2, wholeArea.top() + topMargin()/2),
											QPoint(wholeArea.right() - rightMargin()/2, wholeArea.bottom() - bottomMargin()/2));

		// if there is exactly one cell, the orientation is vertical
		bool horizontal = (numColumns_ != 1);

		int offset;
		if (horizontal)
			offset = (spaceBetweenColumns_ > 0) ? spaceBetweenColumns_/2 : 1;
		else
			offset = (spaceBetweenRows_ > 0) ? spaceBetweenRows_/2 : 1;

		int last = horizontal ? midArea.left() : midArea.top();
		int list_size = horizontal ? numColumns_ : numRows_;

		int thisElementPos = horizontal ? parentX + x(item) : parentY + y(item);

		for(int i = 0; i < (list_size); ++i)
		{
			ItemRegion cursorRegion;
			if (horizontal)
			{
				auto child = elementGrid_[i][0];
				cursorRegion.setRegion(QRect(last, elementsArea.top(), thisElementPos + child->x(item) - last,
														elementsArea.height()));
				last = thisElementPos + child->xEnd(item) + offset;
			}
			else
			{
				auto child = elementGrid_[0][i];
				cursorRegion.setRegion(QRect(elementsArea.left(), last,  elementsArea.width(),
														thisElementPos + child->y(item) - last));
				last = thisElementPos + child->yEnd(item) + offset;
			}

			adjustCursorRegionToAvoidZeroSize(cursorRegion.region(), horizontal, i==0, false);

			// Note below, that a horizontal layout, means a vertical cursor
			auto lc = new LayoutCursor(item, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
			lc->setOwnerElement(this);
			cursorRegion.setCursor(lc);
			lc->setIndex(i);
			lc->setVisualizationPosition(cursorRegion.region().topLeft());
			lc->setVisualizationSize(horizontal ? QSize(2, height(item)) : QSize(width(item), 2));
			lc->setOwnerElement(this);
			if (i==0) lc->setIsAtBoundary(true);

			cursorRegion.cursor()->setRegion(cursorRegion.region());
			if (notLocationEquivalentCursors(item)) lc->setNotLocationEquivalent(true);

			// Skip cursor?
			if (!((i == 0) && noBoundaryCursors(item)) && !((i > 0) && noInnerCursors(item)))
				allRegions.append(cursorRegion);
		}

		// Add trailing cursor region if not omitted
		if (!noBoundaryCursors(item))
		{
			QRect trailing;
			if (horizontal)
				trailing.setRect(last, elementsArea.top(), midArea.right() + 1 - last, elementsArea.height());
			else
				trailing.setRect(elementsArea.left(), last,  elementsArea.width(), midArea.bottom() + 1 - last);

			adjustCursorRegionToAvoidZeroSize(trailing, horizontal, false, true);

			allRegions.append(ItemRegion(trailing));
			// Note below, that a horizontal layout, means a vertical cursor
			auto lc = new LayoutCursor(item, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
			lc->setOwnerElement(this);
			allRegions.last().setCursor(lc);
			lc->setIndex(list_size);
			lc->setVisualizationPosition(allRegions.last().region().topLeft());
			lc->setVisualizationSize(horizontal ? QSize(2, height(item)) : QSize(width(item), 2));
			lc->setRegion(trailing);
			lc->setIsAtBoundary(true);
			if (notLocationEquivalentCursors(item)) lc->setNotLocationEquivalent(true);
		}
	}
	return allRegions;
}

void GridLayoutFormElement::computeOverallStretchFactors()
{
	overallColumnStretchFactor_ = 0;
	for(auto stretchFactor : columnStretchFactors_) overallColumnStretchFactor_ += stretchFactor;

	overallRowStretchFactor_ = 0;
	for(auto stretchFactor : rowStretchFactors_) overallRowStretchFactor_ += stretchFactor;
}

void GridLayoutFormElement::adjustSize(int containColumn, int containRow)
{
	if (containColumn >= numColumns_ || containRow >= numRows_)
	{
		// calculate new size
		int newNumColumns = (containColumn < numColumns_) ? numColumns_ : containColumn + 1;
		int newNumRows = (containRow < numRows_) ? numRows_ : containRow + 1;

		// adjust element grid
		auto newElementGrid = QVector<QVector<FormElement*>>(newNumColumns, QVector<FormElement*>(newNumRows, nullptr));
		for (int x=0; x<numColumns_; x++)
			for (int y=0; y<numRows_; y++)
				newElementGrid[x][y] = elementGrid_[x][y];
		elementGrid_ = newElementGrid;

		// adjust span grid
		auto newSpanGrid = QVector<QVector<QPair<int, int>>>(
				newNumColumns,QVector<QPair<int, int>>(newNumRows, QPair<int, int>(1, 1)));
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
					newCellHorizontalGrid[x][y] = defaultColumnHorizontalAlignments_[x];
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

		computeOverallStretchFactors();

		// set new grid size
		numColumns_ = newNumColumns;
		numRows_ = newNumRows;
	}
}

inline void GridLayoutFormElement::adjustCursorRegionToAvoidZeroSize(QRect& region, bool horizontal, bool first, bool last)
{
	// Make sure there is at least some space for the cursor Region.
	if (horizontal && region.width() == 0) region.adjust((first?0:-1), 0, (last?0:1), 0);
	if (!horizontal && region.height() == 0 ) region.adjust(0, (first?0:-1), 0, (last?0:1));
}

int GridLayoutFormElement::focusedElementIndex(Item* item) const
{
	if (numColumns_ == 1)
		for (int y=0; y<numRows_; ++y)
			if (elementGrid_[0][y]->elementOrChildHasFocus(item)) return y;
	if (numRows_ == 1)
		for (int x=0; x<numColumns_; ++x)
			if (elementGrid_[x][0]->elementOrChildHasFocus(item)) return x;

	return -1;
}

} /* namespace Visualization */
