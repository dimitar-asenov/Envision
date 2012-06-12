/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * GridBasedOffsetProvider.cpp
 *
 *  Created on: Jun 11, 2012
 *      Author: Dimitar Asenov
 */

#include "GridBasedOffsetProvider.h"
#include "../OOInteractionException.h"

#include "VisualizationBase/src/items/LayoutProvider.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"

namespace OOInteraction {

GridBasedOffsetProvider::GridBasedOffsetProvider(Visualization::Item* vis)
	: StringOffsetProvider(vis)
{
}

GridBasedOffsetProvider::~GridBasedOffsetProvider()
{
	for (auto c : cells_) SAFE_DELETE(c);
	cells_.clear();
}

void GridBasedOffsetProvider::add(GridCell* cell)
{
	for(auto c: cells_)
		if (c->region().contains(cell->region()) || cell->region().contains(c->region()))
			throw OOInteractionException("Trying to add an overlapping cell to GridOffsetProvider");

	cells_.append(cell);
	if (cell->region().right()+1 > size_.width()) size_.setWidth(cell->region().right()+1);
	if (cell->region().bottom()+1 > size_.height()) size_.setHeight(cell->region().bottom()+1);
}

GridCell* GridBasedOffsetProvider::findCell(const QRect& start, Direction dir) const
{
	GridCell* result = nullptr;
	int min_distance;

	for(auto c : cells_)
	{
		if (start== c->region()) continue;

		int distance = 0;
		switch (dir)
		{
			// Note that in the code below +1 is to correct for weird definition of QRect's right and bottom methods.
			case Left: if (c->region().right()+1 > start.left()
								|| c->region().bottom()+1 <= start.top()
								|| c->region().top() >= start.bottom()+1) continue;
							else distance = start.left() - c->region().right()-1;
			break;
			case Up: if (c->region().bottom()+1 > start.top()
								|| c->region().right()+1 <= start.left()
								|| c->region().left() >= start.right()+1) continue;
							else distance = start.top() - c->region().bottom()-1;
			break;
			case Right: if (c->region().left() < start.right()+1
								|| c->region().bottom()+1 <= start.top()
								|| c->region().top() >= start.bottom()+1) continue;
							else distance = start.right()+1 - c->region().left();
			break;
			case Down: if (c->region().top() < start.bottom()+1
								|| c->region().right()+1 <= start.left()
								|| c->region().left() >= start.right()+1) continue;
							else distance = start.bottom()+1 - c->region().top();
			break;
		}

		if (!result || distance < min_distance)
		{
			result = c;
			min_distance = distance;
			if (distance == 0) break;
		}
	}

	return result;
}

inline bool GridBasedOffsetProvider::isOnTop(GridCell* cell) const
{
	return cell->region().top() == 0;
}

inline bool GridBasedOffsetProvider::isOnLeft(GridCell* cell) const
{
	return cell->region().left() == 0;
}

inline bool GridBasedOffsetProvider::isOnBottom(GridCell* cell) const
{
	return cell->region().bottom()+1 == size_.height();
}

inline bool GridBasedOffsetProvider::isOnRight(GridCell* cell) const
{
	return cell->region().right()+1 == size_.width();
}

int GridBasedOffsetProvider::offset(Qt::Key key)
{
	if (!item() || !item()->itemOrChildHasFocus()) return -1;

	int result = 0;
	QStringList components = this->components();

	GridCell* target = nullptr;
	bool left = false;
	bool right = false;

	// Find the offset when the cursor is within one of the cells.
	for (auto c : cells_)
	{
		if (c->item()->itemOrChildHasFocus())
		{
			int length = 0;
			for (int i = c->stringComponentsStart(); i <= c->stringComponentsEnd(); ++i)
				length = components[i].length();
			result = itemOffset(c->item(), length, key);

			if (key == Qt::Key_Backspace && result == 0)
			{
				if (isOnLeft(c)) return 0;

				target = findCell(c->region(), Left);
				right = true;
			}
			else if (key == Qt::Key_Delete && result == length)
			{
				if (isOnRight(c)) return components.join("").length();

				target = findCell(c->region(), Right);
				left = true;
			}
			else target = c;

		}
	}

	// None of the cells has the cursor. Try a sequential layout.
	if (!target)
	{
		auto layout_provider = dynamic_cast<Visualization::LayoutProvider<Visualization::SequentialLayout>*>(item());
		if (layout_provider && layout_provider->scene()->mainCursor()->owner() == layout_provider->layout())
		{
			int index = layout_provider->layout()->correspondingSceneCursor<Visualization::LayoutCursor>()->index();

			if (key == Qt::Key_Backspace && index == 0) return 0;
			if (key == Qt::Key_Delete && index == layout_provider->layout()->length()) return components.join("").length();

			//Find the cells corresponding to the adjacent items
			auto leftItem = index > 0 ? layout_provider->layout()->at<Visualization::Item>(index-1) : nullptr;
			auto rightItem = index < layout_provider->layout()->length() ?
					layout_provider->layout()->at<Visualization::Item>(index) : nullptr;

			if (key == Qt::Key_Backspace) rightItem = nullptr;
			if (key == Qt::Key_Delete) leftItem = nullptr;

			GridCell* leftCell = nullptr;
			GridCell* rightCell = nullptr;

			for (auto c : cells_)
			{
				if (c->item() == leftItem) leftCell = c;
				if (c->item() == rightItem) rightCell = c;
			}

			if (leftCell && !rightCell)
			{
				target = leftCell;
				right = true;
			}
			else if (rightCell && !leftCell)
			{
				target = rightCell;
				left = true;
			}
			else if (leftCell && rightCell)
			{
				//TODO choose one cell in possibly a smarter way
				target = leftCell;
				right = true;
			}
			else throw OOInteractionException("Can not compute offset of a SequentialLayout");
		}
	}

	// Process the offset
	if (!target) return -1; // This means that we should not do anything

	if (left || right) result = 0;
	if (right) for(int i = target->stringComponentsStart(); i <= target->stringComponentsEnd(); ++i)
		result += components[i].size();

	for(int i = 0; i < target->stringComponentsStart(); ++i) result += components[i].size();

	return result;
}

void GridBasedOffsetProvider::setOffset(int newOffset)
{
	QStringList components = this->components();
	Q_ASSERT(0<= newOffset && newOffset <= components.join("").size());

	int offset = newOffset;
	int index = 0;

	while( index < components.size() && offset > components[index].size())
	{
		offset -= components[index].size();
		++index;
	}

	// Find the cells corresponding to the this index and the next (if any)
	GridCell* indexCell = nullptr;
	GridCell* nextCell = nullptr;
	for(auto c : cells_)
	{
		if (c->stringComponentsStart() <= index && c->stringComponentsEnd() >= index) indexCell = c;
		if (c->stringComponentsStart() <= index+1 && c->stringComponentsEnd() >= index+1) nextCell = c;
	}

	if (!indexCell && !nextCell)
	{
		if (newOffset == 0)
		{
			item()->moveCursor( Visualization::Item::MoveOnPosition, QPoint(0,item()->height()/2));
			return;
		}
		else if (index+1 == components.size() && offset == components[index].size())
		{
			item()->moveCursor( Visualization::Item::MoveOnPosition, QPoint(item()->xEnd(), item()->height()/2));
			return;
		}
		else throw OOInteractionException("Could not find adjacent cells when setting GridBased offset.");
	}

	// TODO choose a cell in possibly a smarter way
	if (indexCell) setOffsetInItem(offset, indexCell->item());
	else setOffsetInItem(0, nextCell->item());
}

} /* namespace OOInteraction */
