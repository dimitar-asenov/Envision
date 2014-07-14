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

namespace Visualization {

class VISUALIZATIONBASE_API GridLayouter
{
	public:

		enum MajorAxis {NoMajor, ColumnMajor, RowMajor};
		template < bool mayHaveMergedCells, class NumRows, class NumColumns, class HasElement, class Width, class Height,
					 class ComputeElementSize, class ChangeGeometry, class IsStretchable, class SetPosition, class SpanGrid,
					 class RowStretchFactors, class ColumnStretchFactors, class HorizontalAlignment, class VerticalAlignment,
					 class SpaceBetweenRows, class SpaceBetweenColumns, class TopMargin, class BottomMargin,
					 class LeftMargin, class RightMargin, class MinWidth, class MinHeight>
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

							setPosition(x, y, QPoint(xPos, yPos));
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

							setPosition(x, y, QPoint(xPos, top));
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

							setPosition(x, y, QPoint(left, yPos));
							left += width(x, y) + spaceBetweenColumns();
						}
					}

					top += tallestInRow[y] + spaceBetweenRows();
				}
			}

			return finalSize;
		}
};

} // namespace Visualization
