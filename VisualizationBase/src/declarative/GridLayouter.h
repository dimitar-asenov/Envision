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
#pragma once

#include "../visualizationbase_api.h"
#include "../layouts/LayoutStyle.h"
#include "../items/ItemRegion.h"
#include "../items/Item.h"

namespace Visualization {

class FormElement;

class VISUALIZATIONBASE_API GridLayouter
{
	public:

		enum MajorAxis {NoMajor, ColumnMajor, RowMajor};
		template < bool mayHaveMergedCells, typename NumRows, typename NumColumns, typename HasElement, typename Width,
					  typename Height,
					  typename ComputeElementSize, typename ChangeGeometry, typename IsStretchable,
					  typename SetPosition, typename SpanGrid,
					  typename RowStretchFactors, typename ColumnStretchFactors,
					  typename HorizontalAlignment, typename VerticalAlignment,
					  typename SpaceBetweenRows, typename SpaceBetweenColumns, typename TopMargin, typename BottomMargin,
					  typename LeftMargin, typename RightMargin, typename MinWidth, typename MinHeight>
		static QSize computeSize(int availableWidth, int availableHeight, MajorAxis majorAxis,
										NumRows numRows, NumColumns numColumns,
										HasElement has, SpanGrid spanGrid, Width width, Height height,
										ComputeElementSize computeElementSize, ChangeGeometry changeGeometry,
										IsStretchable isStretchable, SetPosition setPosition,
										RowStretchFactors rowStretchFactors, ColumnStretchFactors columnStretchFactors,
										HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment,
										SpaceBetweenRows spaceBetweenRows, SpaceBetweenColumns spaceBetweenColumns,
										TopMargin topMargin, BottomMargin bottomMargin, LeftMargin leftMargin,
										RightMargin rightMargin, MinWidth minWidth, MinHeight minHeight)
		{
			Q_ASSERT(!mayHaveMergedCells || majorAxis == NoMajor);

			// Compute default sizes of all the elements
			// Get the widest and tallest items (without merged cells)
			QVector<int> widestInColumn(numColumns(), 0);
			QVector<int> tallestInRow(numRows(), 0);
			QVector<int> totalHeightInColumn(numColumns(), 0);
			QVector<int> totalWidthInRow(numRows(), 0);
			bool hasMultiColumn = false;
			bool hasMultiRow = false;
			for (int x=0; x<numColumns(); x++)
				for (int y=0; y<numRows(); y++)
					if (has(x, y))
					{
						computeElementSize(x, y, 0, 0); // any additional space is distributed later
						if (majorAxis == ColumnMajor) totalHeightInColumn[x] += height(x, y) + (y==0 ? 0 :spaceBetweenRows());
						if (majorAxis == RowMajor) totalWidthInRow[y] += width(x, y) + (x==0 ? 0 : spaceBetweenColumns());

						if (!mayHaveMergedCells || spanGrid(x, y).first == 1)
						{
							if (width(x, y) > widestInColumn[x]) widestInColumn[x] = width(x, y);
						}
						else hasMultiColumn = true;

						if (!mayHaveMergedCells || spanGrid(x, y).second == 1)
						{
							if (height(x, y) > tallestInRow[y]) tallestInRow[y] = height(x, y);
						}
						else hasMultiRow = true;
					}

			// modify widest cells in columns and tallest cells in rows if there are merged columns
			if (mayHaveMergedCells && (hasMultiColumn || hasMultiRow))
				for (int x=0; x<numColumns(); x++)
					for (int y=0; y<numRows(); y++)
					{
						if (spanGrid(x, y).first > 1)
						{
							int availableSpace = 0;
							float availableStretchFactor = 0;
							for (int column=x; column<x+spanGrid(x, y).first; column++)
							{
								availableSpace += widestInColumn[column];
								availableStretchFactor += columnStretchFactors(column);
							}
							availableSpace += spaceBetweenColumns() * (spanGrid(x, y).first - 1);

							int missingSpace = width(x, y) - availableSpace;
							if (missingSpace > 0)
							{
								if (availableStretchFactor == 0) // add the additional space to some column
									widestInColumn[x] += missingSpace;
								else // distribute the additional space according to the stretch factors
									for (int column=x; column<x+spanGrid(x, y).first; column++)
										widestInColumn[column] +=
												std::ceil(missingSpace / availableStretchFactor * columnStretchFactors(column));
							}
						}
						if (spanGrid(x, y).second > 1)
						{
							int availableSpace = 0;
							float availableStretchFactor = 0;
							for (int row=y; row<y+spanGrid(x, y).second; row++)
							{
								availableSpace += tallestInRow[row];
								availableStretchFactor += rowStretchFactors(row);
							}
							availableSpace += spaceBetweenRows() * (spanGrid(x, y).second - 1);

							int missingSpace = height(x, y) - availableSpace;
							if (missingSpace > 0)
							{
								if (availableStretchFactor == 0) // add the additional space to some column
									tallestInRow[y] += missingSpace;
								else // distribute the additional space according to the stretch factors
									for (int row=y; row<y+spanGrid(x, y).second; row++)
										tallestInRow[row] +=
												std::ceil(missingSpace / availableStretchFactor * rowStretchFactors(row));
							}
						}
					}

			// Compute grid size
			QSize finalSize{0, 0};

			// Compute grid width and overall column stretch factor
			if (numColumns() > 0) finalSize.rwidth() += leftMargin() + rightMargin();
			if (majorAxis != RowMajor)
			{
				decltype(columnStretchFactors(0)) overallColumnStretchFactor = 0;
				for (int i = 0; i< numColumns(); ++i)
				{
					finalSize.rwidth() += widestInColumn[i];
					overallColumnStretchFactor += columnStretchFactors(i);
				}
				if (numColumns() > 1) finalSize.rwidth() += spaceBetweenColumns() * (numColumns() - 1);

				// Adjust widest cell in column values if there is additional space available
				availableWidth = std::max(availableWidth, minWidth());
				int additionalWidth = availableWidth - finalSize.rwidth();
				// if availableWidth == 0, this is always false
				if (additionalWidth > 0)
				{
					if (overallColumnStretchFactor > 0) // distribute the additional space according to the stretch factors
						for (int x = 0; x<numColumns(); ++x)
							widestInColumn[x] += std::floor(additionalWidth / overallColumnStretchFactor * columnStretchFactors(x));
					finalSize.rwidth() = availableWidth;
				}
			}
			else
			{
				// No stretching

				// Already includes the spacing
				finalSize.rwidth() += totalWidthInRow.isEmpty() ? 0 :
												*std::max_element(totalWidthInRow.begin(), totalWidthInRow.end());
			}

			// Compute grid height and overall row stretch factor
			if (numRows() > 0) finalSize.rheight() += topMargin() + bottomMargin();
			if (majorAxis != ColumnMajor)
			{
				decltype(rowStretchFactors(0)) overallRowStretchFactor = 0;
				for (int i = 0; i<numRows(); ++i)
				{
					finalSize.rheight() += tallestInRow[i];
					overallRowStretchFactor += rowStretchFactors(i);
				}
				if (numRows() > 1) finalSize.rheight() += spaceBetweenRows() * (numRows() - 1);

				// Adjust tallest cell in row values if there is additional space available
				availableHeight = std::max(availableHeight, minHeight());
				int additionalHeight = availableHeight - finalSize.rheight();
				// if availableHeight == 0, this is always false
				if (additionalHeight > 0)
				{
					if (overallRowStretchFactor > 0) // distribute the additional space according to the stretch factors
						for (int y = 0; y<numRows(); ++y)
							tallestInRow[y] += std::floor(additionalHeight / overallRowStretchFactor * rowStretchFactors(y));
					finalSize.rheight() = availableHeight;
				}
			}
			else
			{
				// No stretching

				// Already includes the spacing
				finalSize.rheight() += totalHeightInColumn.isEmpty() ? 0 :
												*std::max_element(totalHeightInColumn.begin(), totalHeightInColumn.end());
			}

			// DONE With size. At this point the finalSize object contains the final size of this object.

			// Recompute all the element's sizes, if their size depends on their parent's size
			for (int x=0; x<numColumns(); x++)
				for (int y=0; y<numRows(); y++)
					if (has(x, y) && isStretchable(x, y))
					{
						int localAvailableWidth = 0;
						int localAvailableHeight = 0;

						if (!mayHaveMergedCells || (spanGrid(x, y).first == 1 && spanGrid(x, y).second == 1))
						{
							localAvailableWidth = widestInColumn[x];
							localAvailableHeight = tallestInRow[y];
						}
						else
						{
							for (int column=x; column<x+spanGrid(x, y).first; column++)
								localAvailableWidth += widestInColumn[column];

							for (int row=y; row<y+spanGrid(x, y).second; row++)
								localAvailableHeight += tallestInRow[row];
						}

						if (majorAxis == RowMajor) localAvailableWidth = 0;
						if (majorAxis == ColumnMajor) localAvailableHeight = 0;
						changeGeometry(x, y, localAvailableWidth, localAvailableHeight);
					}

			// Set element positions
			if (majorAxis == NoMajor)
			{
				int left = leftMargin();
				for (int x=0; x<numColumns(); ++x)
				{
					int top = topMargin();
					for (int y=0; y<numRows(); ++y)
					{
						if (has(x, y))
						{
							int xPos = left;
							if (horizontalAlignment(x, y) == LayoutStyle::Alignment::Center)
							{
								if (!mayHaveMergedCells || spanGrid(x, y).first == 1)
									xPos += (widestInColumn[x] - width(x, y))/2;
								else
								{
									int localAvailableWidth = 0;
									for (int column=x; column<x+spanGrid(x, y).first; column++)
										localAvailableWidth += widestInColumn[column];
									xPos += (localAvailableWidth - width(x, y))/2;
								}
							}
							else if (horizontalAlignment(x, y) == LayoutStyle::Alignment::Right)
							{
								if (!mayHaveMergedCells || spanGrid(x, y).first == 1)
									xPos += (widestInColumn[x] - width(x, y));
								else
								{
									int localAvailableWidth = 0;
									for (int column=x; column<x+spanGrid(x, y).first; column++)
										localAvailableWidth += widestInColumn[column];
									xPos += (localAvailableWidth - width(x, y));
								}
							}

							int yPos = top;
							if (verticalAlignment(x, y) == LayoutStyle::Alignment::Center)
							{
								if (!mayHaveMergedCells || spanGrid(x, y).second == 1)
									yPos += (tallestInRow[y] - height(x, y))/2;
								else
								{
									int localAvailableHeight = 0;
									for (int row=y; row<y+spanGrid(x, y).second; row++)
										localAvailableHeight += tallestInRow[row];
									yPos += (localAvailableHeight - height(x, y))/2;
								}
							}
							else if (verticalAlignment(x, y) == LayoutStyle::Alignment::Bottom)
							{
								if (!mayHaveMergedCells || spanGrid(x, y).second == 1)
									yPos += tallestInRow[y] - height(x, y);
								else
								{
									int localAvailableHeight = 0;
									for (int row=y; row<y+spanGrid(x, y).second; row++)
										localAvailableHeight += tallestInRow[row];
									yPos += localAvailableHeight - height(x, y);
								}
							}

							setPosition(x, y, QPoint{xPos, yPos});
						}

						top += tallestInRow[y] + spaceBetweenRows();
					}

					left += widestInColumn[x] + spaceBetweenColumns();
				}
			}
			else if (majorAxis == ColumnMajor)
			{
				int left = leftMargin();
				for (int x=0; x<numColumns(); ++x)
				{
					int top = topMargin();
					for (int y=0; y<numRows(); ++y)
					{
						if (has(x, y))
						{
							int xPos = left;
							if (horizontalAlignment(x, y) == LayoutStyle::Alignment::Center)
								xPos += (widestInColumn[x] - width(x, y))/2;
							else if (horizontalAlignment(x, y) == LayoutStyle::Alignment::Right)
								xPos += (widestInColumn[x] - width(x, y));

							setPosition(x, y, QPoint{xPos, top});
							top += height(x, y) + spaceBetweenRows();
						}
					}

					left += widestInColumn[x] + spaceBetweenColumns();
				}
			}
			else if (majorAxis == RowMajor)
			{
				int top = topMargin();
				for (int y=0; y<numRows(); ++y)
				{
					int left = leftMargin();
					for (int x=0; x<numColumns(); ++x)
					{
						if (has(x, y))
						{
							int yPos = top;
							if (verticalAlignment(x, y) == LayoutStyle::Alignment::Center)
								yPos += (tallestInRow[y] - height(x, y))/2;
							else if (verticalAlignment(x, y) == LayoutStyle::Alignment::Bottom)
								yPos += tallestInRow[y] - height(x, y);

							setPosition(x, y, QPoint{left, yPos});
							left += width(x, y) + spaceBetweenColumns();
						}
					}

					top += tallestInRow[y] + spaceBetweenRows();
				}
			}

			return finalSize;
		}


		/**
		 * Does not consider spans.
		 */
		template <typename NumRows, typename NumColumns, typename HasElement, typename Width, typename Height,
					 typename XPos, typename YPos, typename ChildItem,
					 typename SpaceBetweenRows, typename SpaceBetweenColumns, typename TopMargin, typename BottomMargin,
					 typename LeftMargin, typename RightMargin>
		static QList<ItemRegion> regions(Item* parent, FormElement* formElement, int xOffset, int yOffset,
										MajorAxis majorAxis, bool showCursorWhenEmpty, bool showInnerCursors,
										bool showBoundaryCursors, bool extraCursorsAroundParentShape, bool showMajorCursors,
										bool notLocationEquivalentCursors,

										NumRows numRows, NumColumns numColumns,
										HasElement has, Width width, Height height, XPos xPos, YPos yPos,
										ChildItem childItem,
										SpaceBetweenRows spaceBetweenRows, SpaceBetweenColumns spaceBetweenColumns,
										TopMargin topMargin, BottomMargin bottomMargin, LeftMargin leftMargin,
										RightMargin rightMargin)
		{
			QList<ItemRegion> regs;

			auto isEmpty = numColumns() == 0 || numRows() == 0;
			if ( isEmpty && !showCursorWhenEmpty) return regs;

			extraCursorsAroundParentShape = extraCursorsAroundParentShape && parent->hasShape();

			// Get the widest and tallest items and the precise position of different columns
			QVector<int> columnLeft(numColumns(), std::numeric_limits<int>::max());
			QVector<int> columnRight(numColumns(), 0);
			QVector<int> rowTop(numRows(), std::numeric_limits<int>::max());
			QVector<int> rowBottom(numRows(), 0);
			QVector<int> lastChildIndexInColumn(numColumns(), -1);
			QVector<int> lastChildIndexInRow(numRows(), -1);
			int rightMostPoint = 0;
			int bottomMostPoint = 0;
			for (int x=0; x<numColumns(); x++)
				for (int y=0; y<numRows(); y++)
					if (has(x, y))
					{
						lastChildIndexInColumn[x] = y;
						lastChildIndexInRow[y] = x;
						QRect elementRect{xPos(x, y), yPos(x, y), width(x, y), height(x, y)};
						if (elementRect.left() < columnLeft[x]) columnLeft[x] = elementRect.left();
						if (elementRect.right() > columnRight[x]) columnRight[x] = elementRect.right();

						if (elementRect.top() < rowTop[y]) rowTop[y] = elementRect.top();
						if (elementRect.bottom() > rowBottom[y]) rowBottom[y] = elementRect.bottom();

						if (elementRect.right() > rightMostPoint) rightMostPoint = elementRect.right();
						if (elementRect.bottom() > bottomMostPoint) bottomMostPoint = elementRect.bottom();
					}

			// If a row or a column is completely empty it still contains std::numeric_limits<int>::max().
			// We need to correctly set the elements in that case.
			// To make the cursors more visible, we use the available margins/space, halfway on both sides
			int leftSoFar = xOffset + leftMargin();
			int leftPrevSpace = leftMargin();
			int topSoFar = yOffset + topMargin();
			int topPrevSpace = topMargin();
			for (int x = 0; x<numColumns(); ++x)
			{
				int nextSpace = (x+1 == numColumns() ? rightMargin() : spaceBetweenColumns() );
				if (columnLeft[x] == std::numeric_limits<int>::max())
				{
					columnLeft[x] = leftSoFar - leftPrevSpace/2;
					columnRight[x] = leftSoFar + nextSpace/2;
					leftSoFar += nextSpace;
				}
				else leftSoFar = columnRight[x] + nextSpace;

				leftPrevSpace = nextSpace;
			}
			for (int y = 0; y<numRows(); ++y)
			{
				int nextSpace = (y+1 == numRows() ? bottomMargin() : spaceBetweenRows() );
				if (rowTop[y] == std::numeric_limits<int>::max())
				{
					rowTop[y] = topSoFar - topPrevSpace/2;
					rowBottom[y] = topSoFar + nextSpace/2;
					topSoFar += nextSpace;
				}
				topSoFar = rowBottom[y] + nextSpace;

				topPrevSpace = nextSpace;
			}

			// Set Child item regions and remember rects
			QVector< QVector<QRect> > itemAreas(numColumns(), QVector<QRect>(numRows(), QRect{}));
			QVector<int> columnNextTop(numColumns(), numRows() > 0 ?  rowTop[0] : 0);
			QVector<int> rowNextLeft(numRows(), numColumns() > 0 ? columnLeft[0] : 0);
			for (int x=0; x<numColumns(); x++)
				for (int y=0; y<numRows(); y++)
					if (has(x, y))
					{
						QRect itemArea;

						// Set vertical position and dimensions
						if (majorAxis == ColumnMajor)
						{
							itemArea.setTop(columnNextTop[x]);
							itemArea.setHeight( height(x, y) );
							columnNextTop[x] += itemArea.height() + spaceBetweenRows();
						}
						else
						{
							itemArea.setTop(rowTop[y]);
							itemArea.setBottom( rowBottom[y] );
						}

						// Set horizontal position and dimensions
						if (majorAxis == RowMajor)
						{
							itemArea.setLeft(rowNextLeft[y]);
							itemArea.setWidth( width(x, y) );
							rowNextLeft[y] += itemArea.width() + spaceBetweenColumns();
						}
						else
						{
							itemArea.setLeft(columnLeft[x]);
							itemArea.setRight( columnRight[x] );
						}

						itemAreas[x][y] = itemArea;

						if (auto child = childItem(x, y))
						{
							regs.append( ItemRegion{} );
							regs.last().setItem( child );
							regs.last().setRegion(itemArea);
						}
					}
					else
					{
						// Add a dummy area rect to make putting cursors easier
						if (majorAxis == NoMajor)
						{
							itemAreas[x][y].setTopLeft( QPoint{columnLeft[x], rowTop[y]} );
							itemAreas[x][y].setBottomRight( QPoint{columnRight[x], rowBottom[y]} );
						}
					}

			// Set front and inner cursor regions.
			// Inner cursors span the spacing between items.
			// Front and Back cursors, if present, are in the margins.
			// Cursors outside shape, if present, are outside the margins.
			int frontCursorLeft = numColumns() > 0 ? columnLeft[0] - leftMargin() : 0;
			int frontCursorTop = numRows() > 0 ? rowTop[0] - topMargin() : 0;
			int totalWidth = rightMostPoint - frontCursorLeft;
			int totalHeight = bottomMostPoint - frontCursorTop;

			for (int x=0; x<numColumns(); x++)
				for (int y=0; y<numRows(); y++)
				{
					int columnWidth = columnRight[x]-columnLeft[x]+1;
					int rowHeight = rowBottom[y]-rowTop[y]+1;

					// Front minor cursor. Always there if requested.
					if (showBoundaryCursors && ((x == 0 && majorAxis != ColumnMajor) || (y==0 && majorAxis == ColumnMajor)))
					{
						regs.append( cursorRegion(parent, formElement, x, y, majorAxis == ColumnMajor,
								!extraCursorsAroundParentShape,
								notLocationEquivalentCursors, !extraCursorsAroundParentShape, true,
								majorAxis == ColumnMajor
								? QRect{columnLeft[x], frontCursorTop, columnWidth, topMargin()}
								: QRect{frontCursorLeft, rowTop[y], leftMargin(), rowHeight} ));
					}

					// Inner major cursor after the current major axis, if requested
					if (showMajorCursors &&
							(		(majorAxis == ColumnMajor && x+1<numColumns() && y==0)
							 ||	(majorAxis != ColumnMajor && y+1<numRows() && x==0)))
					{
						regs.append( cursorRegion(parent, formElement,
							majorAxis == ColumnMajor ? x+1 : -1, majorAxis == ColumnMajor ? -1 : y+1,
							majorAxis != ColumnMajor, false, true, true, true,
							majorAxis == ColumnMajor
							? QRect{columnRight[x]+1, frontCursorTop, leftMargin(), totalHeight}
							: QRect{frontCursorLeft, rowBottom[y]+1, totalWidth, topMargin()} ));
					}

					// Inner cursor to next element, if any
					if (showInnerCursors && !itemAreas[x][y].isNull()
							&& (	(majorAxis == ColumnMajor && y < lastChildIndexInColumn[x])
								||
									(majorAxis != ColumnMajor && x < lastChildIndexInRow[y])))
					{
						regs.append( cursorRegion(parent, formElement,
								majorAxis == ColumnMajor ? x : x+1,
								majorAxis == ColumnMajor ? y+1 : y, majorAxis == ColumnMajor, false,
								notLocationEquivalentCursors, true, true,
								majorAxis == ColumnMajor
								? QRect{columnLeft[x], itemAreas[x][y].bottom()+1, columnWidth, spaceBetweenRows()}
								: QRect{itemAreas[x][y].right()+1, rowTop[y], spaceBetweenColumns(), rowHeight} ));
					}

					// Back cursor, if requested, and if there is at least one element
					if (showBoundaryCursors && !itemAreas[x][y].isNull()
							&& (	(majorAxis == ColumnMajor && y == lastChildIndexInColumn[x])
								||
									(majorAxis != ColumnMajor && x == lastChildIndexInRow[y])))
					{
						regs.append( cursorRegion(parent, formElement,
								majorAxis == ColumnMajor ? x : x+1,
								majorAxis == ColumnMajor ? y+1 : y, majorAxis == ColumnMajor, !extraCursorsAroundParentShape,
								notLocationEquivalentCursors, true, !extraCursorsAroundParentShape,
								majorAxis == ColumnMajor
								? QRect{columnLeft[x], itemAreas[x][y].bottom()+1, columnWidth, bottomMargin()}
								: QRect{itemAreas[x][y].right()+1, rowTop[y], rightMargin(), rowHeight} ));
					}

				}

			// If Empty but a cursor is requested add a single fron-back cursor
			if (isEmpty && showCursorWhenEmpty)
			{
				// Assume a symmetric shape
				auto left = xOffset + leftMargin();
				auto right = parent->widthInLocal() - left - xOffset - rightMargin();
				if (right < left) right = left+1;
				auto top = yOffset + topMargin();
				auto bottom = parent->heightInLocal() - top - yOffset - bottomMargin();
				if (bottom < top) bottom = top+1;

				regs.append( cursorRegion(parent, formElement, 0, 0, majorAxis == ColumnMajor,
						!extraCursorsAroundParentShape,
						notLocationEquivalentCursors, !extraCursorsAroundParentShape, !extraCursorsAroundParentShape,
						QRect{QPoint{top, left}, QPoint{bottom, right}} ));
			}


			// Front and back major cursor. Always there if requested.
			if (showMajorCursors)
			{
				// Front
				regs.append( cursorRegion(parent, formElement,
						majorAxis == ColumnMajor ? 0 : -1, majorAxis == ColumnMajor ? -1 : 0,
						majorAxis != ColumnMajor, false, true, false, true,
						majorAxis == ColumnMajor
						? QRect{frontCursorLeft, frontCursorTop, leftMargin(), totalHeight}
						: QRect{frontCursorLeft, frontCursorTop, totalWidth, topMargin()} ));

				//Back
				regs.append( cursorRegion(parent, formElement,
						majorAxis == ColumnMajor ? numColumns() : -1, majorAxis == ColumnMajor ? -1 : numRows(),
						majorAxis != ColumnMajor, false, true, true, false,
						majorAxis == ColumnMajor
						? QRect{numColumns()>0 ? columnRight[numColumns()-1]+1 : 0, frontCursorTop, leftMargin(), totalHeight}
						: QRect{frontCursorLeft, numRows()>0 ? rowBottom[numRows()-1]+1 : 0, totalWidth, topMargin()} ));
			}

			// Cursors outside shape
			constexpr int CURSOR_SIZE = 2;
			if (extraCursorsAroundParentShape)
			{
				// Front
				regs.append(cursorRegion(parent, formElement, -1, -1, majorAxis == ColumnMajor, true,
								notLocationEquivalentCursors, false, true,
								majorAxis == ColumnMajor
								? QRect{0, 0, parent->widthInLocal(), CURSOR_SIZE}
								: QRect{0, 0, CURSOR_SIZE, parent->heightInLocal()} ));

				// Back
				regs.append(cursorRegion(parent, formElement, numColumns()+1, numRows()+1, majorAxis == ColumnMajor, true,
								notLocationEquivalentCursors, true, false,
								majorAxis == ColumnMajor
								? QRect{0, parent->heightInLocal() - CURSOR_SIZE - 1, parent->widthInLocal(), CURSOR_SIZE}
								: QRect{parent->widthInLocal() - CURSOR_SIZE - 1, 0, CURSOR_SIZE, parent->heightInLocal()} ));
			}

			return regs;
		}


		static QVector< QVector<Model::Node*>> arrange(QVector<Model::Node*> nodes, MajorAxis majorAxis);
		static void setPositionInGrid(QVector<Model::Node*> nodes, int x, int y, Model::Node* node, MajorAxis majorAxis);
		static void removeFromGrid(QVector<Model::Node*> nodes, Model::Node* node, MajorAxis majorAxis);
		static void normalizeGridIndices(QVector<Model::Node*> nodes, MajorAxis majorAxis);
	//*******************************************************************************************************************
	// Helpers
	//*******************************************************************************************************************
	private:
		static ItemRegion cursorRegion(Item* parent, FormElement* formElement, int xIndex, int yIndex,
				bool horizontal, bool atBoundary, bool notLocationEquivalent, bool mayExpandFront, bool mayExpandBack,
				QRect area);

		template<typename Container, typename Value>
		static void resizeReplace(Container& container, int majorIndex, int minorIndex, Value value);

		static void pushNodes(QVector<Model::Node*> nodes, int x, int y, int pushAmount, MajorAxis majorAxis,
				bool pushAllMajor);
};

}
