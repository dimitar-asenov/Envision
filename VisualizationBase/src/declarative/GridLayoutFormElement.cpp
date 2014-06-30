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

#include "GridLayoutFormElement.h"

#include "DeclarativeItemBase.h"
#include "GridLayouter.h"
#include "../items/ItemRegion.h"
#include "../cursor/LayoutCursor.h"

namespace Visualization {

GridLayoutFormElement::GridLayoutFormElement()
{
	// initialize element grid
	elementGrid_ = QVector<QVector<FormElement*>>(numColumns_, QVector<FormElement*>(numRows_));

	// initialize span grid
	spanGrid_ = QVector<QVector<QPair<int, int>>>(numColumns_, QVector<QPair<int, int>>(numRows_, QPair<int, int>(1, 1)));

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
	for (int x = 0; x<elementGrid_.size(); ++x)
		for (int y = 0; y<elementGrid_[x].size(); ++y)
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
	QSize finalSize = GridLayouter::computeSize<true>(availableWidth, availableHeight,
		[this](){return numRows_;},	// numRows
		[this](){return numColumns_;},	// numColumns
		[this](int x, int y){return elementGrid_[x][y];},	// has
		[this](int x, int y){return spanGrid_[x][y];},	// spanGrid
		[this, item](int x, int y){return elementGrid_[x][y]->width(item);},	// width
		[this, item](int x, int y){return elementGrid_[x][y]->height(item);},	// height
		[this, item](int x, int y, int w, int h){elementGrid_[x][y]->computeSize(item, w, h);},	// computeElementSize
		[this, item](int x, int y, int w, int h){elementGrid_[x][y]->computeSize(item, w, h);},	// changeGeometry
		[this, item](int x, int y){return elementGrid_[x][y]->sizeDependsOnParent(item);},	// isStretchable
		[this, item](int x, int y, QPoint pos){elementGrid_[x][y]->setPos(item, pos);},	// setPosition
		[this](int y){return rowStretchFactors_[y];},	// rowStretchFactors
		[this](int x){return columnStretchFactors_[x];},	// columnStretchFactors
		[this](int x, int y){return cellHorizontalAlignmentGrid_[x][y];},	// horizontalAlignment
		[this](int x, int y){return cellVerticalAlignmentGrid_[x][y];},	// verticalAlignment
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

bool GridLayoutFormElement::sizeDependsOnParent(const Item*) const
{
	return overallColumnStretchFactor_ > 0 || overallRowStretchFactor_ > 0;
}

QList<ItemRegion> GridLayoutFormElement::regions(DeclarativeItemBase* item, int parentX, int parentY)
{
	QList<ItemRegion> allRegions;

	// regions for the child elements
	for (int x=0; x<numColumns_; x++)
		for (int y=0; y<numRows_; y++)
			if (elementGrid_[x][y])
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

		for (int i = 0; i < (list_size); ++i)
		{
			ItemRegion cursorRegion;
			if (horizontal)
			{
				if (auto child = elementGrid_[i][0])
				{
					cursorRegion.setRegion(QRect(last, elementsArea.top(), thisElementPos + child->x(item) - last,
															elementsArea.height()));
					last = thisElementPos + child->xEnd(item) + offset;
				}
				else
				{
					cursorRegion.setRegion(QRect(last, elementsArea.top(), 2, elementsArea.height()));
					last += offset;
				}
			}
			else
			{
				if (auto child = elementGrid_[0][i])
				{
					cursorRegion.setRegion(QRect(elementsArea.left(), last,  elementsArea.width(),
															thisElementPos + child->y(item) - last));
					last = thisElementPos + child->yEnd(item) + offset;
				}
				else
				{
					cursorRegion.setRegion(QRect(elementsArea.left(), last,  elementsArea.width(), 2));
					last += offset;
				}
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
	for (auto stretchFactor : columnStretchFactors_) overallColumnStretchFactor_ += stretchFactor;

	overallRowStretchFactor_ = 0;
	for (auto stretchFactor : rowStretchFactors_) overallRowStretchFactor_ += stretchFactor;
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
				newNumColumns, QVector<QPair<int, int>>(newNumRows, QPair<int, int>(1, 1)));
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

inline void GridLayoutFormElement::adjustCursorRegionToAvoidZeroSize(QRect& region, bool horizontal, bool first,
																							bool last)
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
