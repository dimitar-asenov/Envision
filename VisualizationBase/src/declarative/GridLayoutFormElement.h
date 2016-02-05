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

#pragma once

#include "../visualizationbase_api.h"
#include "LayoutFormElement.h"
#include "../layouts/LayoutStyle.h"

namespace Visualization {

/**
 * This class allows the arrangement of elements in a grid.
 * During its definition the grid is automatically resized to contain all the specified column and row indices.
 *
 * The grid supports
 *  - setting the alignment of all cells in the grid, a specific column/row, or only one cell
 *  - setting stretch factors for columns/rows
 *  - merging multiple cells into one
 *  - cursors, if the grid consists of exactly one row or column (not compatible with merging cells)
 */
class VISUALIZATIONBASE_API GridLayoutFormElement
		: public SuperLayoutElement<GridLayoutFormElement, LayoutFormElement>
{
	public:
		GridLayoutFormElement();
		GridLayoutFormElement(const GridLayoutFormElement& other);
		GridLayoutFormElement& operator=(const GridLayoutFormElement&) = delete;
		virtual ~GridLayoutFormElement();

		virtual GridLayoutFormElement* clone() const override;

		// Methods executable on element definition
		/**
		 * Puts the \a element at \a column and \a row in the grid.
		 *
		 * If the element is already used elsewhere (is a form root, or has a parent) it is cloned and that clone is put
		 * in the grid instead.
		 *
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* put(int column, int row, FormElement* element);
		/**
		 * Sets the horizontal and the vertical \a spacing to the same value.
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* setSpacing(int spacing);
		/**
		 * Sets the horizontal and the vertical spacing to \a spaceBetweenColumns and \a spaceBetweenRows respectively.
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* setSpacing(int spaceBetweenColumns, int spaceBetweenRows);
		/**
		 * Sets the horizontal spacing to \a spaceBetweenColumns.
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* setHorizontalSpacing(int spaceBetweenColumns);
		/**
		 * Sets the vertical spacing to \a spaceBetweenRows.
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* setVerticalSpacing(int spaceBetweenRows);
		/**
		 * Sets the horizontal alignment to \a horizontalAlignment for all the cells in the grid.
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* setHorizontalAlignment(LayoutStyle::Alignment horizontalAlignment);
		/**
		 * Sets the vertical alignment to \a verticalAlignment for all the cells in the grid.
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* setVerticalAlignment(LayoutStyle::Alignment verticalAlignment);
		/**
		 * Sets the horizontal alignment to \a horizontalAlignment for all the cells in the specified \a column.
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* setColumnHorizontalAlignment(int column, LayoutStyle::Alignment horizontalAlignment);
		/**
		 * Sets the vertical alignment to \a verticalAlignment for all the cells in the specified \a row.
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* setRowVerticalAlignment(int row, LayoutStyle::Alignment verticalAlignment);
		/**
		 * Sets the horizontal alignment to \a horizontalAlignment for the cell where the last element was added.
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* setCellHorizontalAlignment(LayoutStyle::Alignment horizontalAlignment);
		/**
		 * Sets the vertical alignment to \a verticalAlignment for the cell where the last element was added.
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* setCellVerticalAlignment(LayoutStyle::Alignment verticalAlignment);
		/**
		 * Sets the horizontal and vertical alignment to \a horizontalAlignment and \a verticalAlignment respectively for
		 * the cell where the last element was added.
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* setCellAlignment(LayoutStyle::Alignment horizontalAlignment,
														LayoutStyle::Alignment verticalAlignment);
		/**
		 * Set the cell spanning of the cell where the last element was added. This means \a columnSpan x \a rowSpan cells
		 * will be merged into a new cell, of which the column and row of the last added cell is the top left corner. A
		 * default cell has a \a columnSpan and \a rowSpan of one each.
		 * Returns a pointer to this GridLayoutElement.
		 */
		GridLayoutFormElement* setCellSpanning(int columnSpan, int rowSpan);
		/**
		 * Sets the stretch factor of this \a column to \a stretchFactor.
		 * The stretch factor determines, how much of the additionally available space this column gets if there is any.
		 * By default this is 0, meaning the column does not stretch. If the stretch factor is bigger than zero, the
		 * column gets a portion of additional space computed by (\a stretchFactor) / (sum of all column stretch factors).
		 */
		GridLayoutFormElement* setColumnStretchFactor(int column, float stretchFactor);
		/**
		 * Sets the stretch factor of all columns to \a stretchFactor.
		 * The stretch factor determines, how much of the additionally available space each column gets if there is any.
		 * By default this is 0, meaning the column does not stretch. If the stretch factor is bigger than zero, the
		 * column gets a portion of additional space computed by (\a stretchFactor) / (sum of all column stretch factors).
		 */
		GridLayoutFormElement* setColumnStretchFactors(float stretchFactor);
		/**
		 * Sets the stretch factor of this \a row to \a stretchFactor.
		 * The stretch factor determines, how much of the additionally available space this row gets if there is any.
		 * By default this is 0, meaning the row does not stretch. If the stretch factor is bigger than zero, the row
		 * gets a portion of additional space computed by (\a stretchFactor) / (sum of all row stretch factors).
		 */
		GridLayoutFormElement* setRowStretchFactor(int row, float stretchFactor);
		/**
		 * Sets the stretch factor of all rows to \a stretchFactor.
		 * The stretch factor determines, how much of the additionally available space each row gets if there is any.
		 * By default this is 0, meaning the row does not stretch. If the stretch factor is bigger than zero, the row
		 * gets a portion of additional space computed by (\a stretchFactor) / (sum of all row stretch factors).
		 */
		GridLayoutFormElement* setRowStretchFactors(float stretchFactor);
		/**
		 * Sets the stretch factor of all columns and rows to \a stretchFactor.
		 * The stretch factor determines, how much of the additionally available space each column/row gets if there is
		 * any. By default this is 0, meaning the column/row does not stretch. If the stretch factor is bigger than zero,
		 * the column/row gets a portion of additional space computed by
		 * (\a stretchFactor) / (sum of all column/row stretch factors).
		 */
		GridLayoutFormElement* setStretchFactors(float stretchFactor);
		/**
		 * Sets if the element has a cursor if it is empty to \a cursorWhenEmpty. Is false by default.
		 * Also see SequentialLayoutStyle::hasCursorWhenEmpty().
		 */

		// Methods executable when items need to be rendered
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) override;
		virtual bool sizeDependsOnParent(const Item* item) const override;
		virtual QList<ItemRegion> regions(DeclarativeItemBase* item, int parentX, int parentY) override;

		/**
		 * Returns the index of the focused element for the specified \a item. If no element is focused, returns -1. This
		 * method is very similar to the focsedElementIndex of SequentialLayout (layouts folder).
		 */
		int focusedElementIndex(Item* item) const;

		/**
		 * Returns the 2D index of the focused element for the specified \a item. If no element is focused, returns -1
		 * for both coordinates.
		 */
		QPoint focusedElement2DIndex(Item* item) const;

		/**
		 * Asserts that this is a linear grid and returns the length.
		 */
		int length(Item* item) const;

		/**
		 * Asserts that this is a linear grid and returns the item at \a index.
		 * If there is not an item but some compound FormElement at that place, that is also an assertion violation!
		 */
		const Visualization::Item* itemAt(DeclarativeItemBase* item, int index) const;

	private:

		// Do not forget to update the copy constructor if adding new members.
		int numColumns_{1};
		int numRows_{1};
		int spaceBetweenColumns_{};
		int spaceBetweenRows_{};

		QPair<int, int> lastCell_{QPair<int, int>(0, 0)};

		QVector<QVector<FormElement*>> elementGrid_{};
		QVector<QVector<QPair<int, int>>> spanGrid_{};

		LayoutStyle::Alignment defaultHorizontalAlignment_{LayoutStyle::Alignment::Left};
		LayoutStyle::Alignment defaultVerticalAlignment_{LayoutStyle::Alignment::Top};
		QVector<LayoutStyle::Alignment> defaultRowVerticalAlignments_{};
		QVector<LayoutStyle::Alignment> defaultColumnHorizontalAlignments_{};
		QVector<QVector<LayoutStyle::Alignment>> cellHorizontalAlignmentGrid_{};
		QVector<QVector<LayoutStyle::Alignment>> cellVerticalAlignmentGrid_{};

		float defaultColumnStretchFactor_{};
		float defaultRowStretchFactor_{};
		QVector<float> columnStretchFactors_{};
		QVector<float> rowStretchFactors_{};
		float overallColumnStretchFactor_{};
		float overallRowStretchFactor_{};

		/**
		 * Recomputes and caches the sum of the column stretch factors and the row stretch factors respectively.
		 */
		void computeOverallStretchFactors();
		// TODO: rename to grid dimensions
		/**
		 * Adjusts the dimensions of the grid, such that it contains the column with index \a containColumn and the row
		 * with index \a containColumn. The grid dimensions can only grow.
		 */
		void adjustSize(int containColumn, int containRow);
};

inline GridLayoutFormElement* GridLayoutFormElement::setSpacing(int spacing)
{
	spaceBetweenRows_ = spacing;
	spaceBetweenColumns_ = spacing;
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setSpacing(int spaceBetweenColumns, int spaceBetweenRows)
{
	spaceBetweenColumns_ = spaceBetweenColumns;
	spaceBetweenRows_ = spaceBetweenRows;
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setHorizontalSpacing(int spaceBetweenColumns)
{
	spaceBetweenColumns_ = spaceBetweenColumns;
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setVerticalSpacing(int spaceBetweenRows)
{
	spaceBetweenRows_ = spaceBetweenRows;
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setHorizontalAlignment(LayoutStyle::Alignment horizontalAlignment)
{
	defaultHorizontalAlignment_ = horizontalAlignment;
	for (int x = 0; x < numColumns_; x++)
	{
		defaultColumnHorizontalAlignments_[x] = horizontalAlignment;
		for (int y = 0; y < numRows_; y++)
			cellHorizontalAlignmentGrid_[x][y] = horizontalAlignment;
	}

	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setVerticalAlignment(LayoutStyle::Alignment verticalAlignment)
{
	defaultVerticalAlignment_ = verticalAlignment;
	for (int y = 0; y < numRows_; y++)
	{
		defaultRowVerticalAlignments_[y] = verticalAlignment;
		for (int x = 0; x < numColumns_; x++)
			cellVerticalAlignmentGrid_[x][y] = verticalAlignment;
	}
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setColumnHorizontalAlignment(int column,
		LayoutStyle::Alignment horizontalAlignment)
{
	adjustSize(column, 0);

	defaultColumnHorizontalAlignments_[column] = horizontalAlignment;
	for (int y = 0; y < numRows_; y++)
		cellHorizontalAlignmentGrid_[column][y] = horizontalAlignment;

	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setRowVerticalAlignment(
		int row, LayoutStyle::Alignment verticalAlignment)
{
	adjustSize(0, row);

	defaultRowVerticalAlignments_[row] = verticalAlignment;
	for (int x = 0; x < numColumns_; x++)
		cellVerticalAlignmentGrid_[x][row] = verticalAlignment;
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setCellHorizontalAlignment(
		LayoutStyle::Alignment horizontalAlignment)
{
	int column = lastCell_.first;
	int row = lastCell_.second;
	cellHorizontalAlignmentGrid_[column][row] = horizontalAlignment;
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setCellVerticalAlignment(LayoutStyle::Alignment verticalAlignment)
{
	int column = lastCell_.first;
	int row = lastCell_.second;
	cellVerticalAlignmentGrid_[column][row] = verticalAlignment;
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setCellAlignment(LayoutStyle::Alignment horizontalAlignment,
																					LayoutStyle::Alignment verticalAlignment)
{
	int column = lastCell_.first;
	int row = lastCell_.second;
	cellHorizontalAlignmentGrid_[column][row] = horizontalAlignment;
	cellVerticalAlignmentGrid_[column][row] = verticalAlignment;
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setCellSpanning(int columnSpan, int rowSpan)
{
	int column = lastCell_.first;
	int row = lastCell_.second;
	adjustSize(column + columnSpan - 1, row + rowSpan - 1);
	spanGrid_[column][row] = QPair<int, int>(columnSpan, rowSpan);
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setColumnStretchFactor(int column, float stretchFactor)
{
	adjustSize(column, 0);
	columnStretchFactors_[column] = stretchFactor;
	computeOverallStretchFactors();
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setColumnStretchFactors(float stretchFactor)
{
	defaultColumnStretchFactor_ = stretchFactor;
	columnStretchFactors_ = QVector<float>(numColumns_, stretchFactor);
	computeOverallStretchFactors();
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setRowStretchFactor(int row, float stretchFactor)
{
	adjustSize(0, row);
	rowStretchFactors_[row] = stretchFactor;
	computeOverallStretchFactors();
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setRowStretchFactors(float stretchFactor)
{
	defaultRowStretchFactor_ = stretchFactor;
	rowStretchFactors_ = QVector<float>(numRows_, stretchFactor);
	computeOverallStretchFactors();
	return this;
}
inline GridLayoutFormElement* GridLayoutFormElement::setStretchFactors(float stretchFactor)
{
	defaultColumnStretchFactor_ = stretchFactor;
	defaultRowStretchFactor_ = stretchFactor;
	columnStretchFactors_ = QVector<float>(numColumns_, stretchFactor);
	rowStretchFactors_ = QVector<float>(numRows_, stretchFactor);
	computeOverallStretchFactors();
	return this;
}

}
