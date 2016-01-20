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

#include "GridBasedOffsetProvider.h"
#include "Cell.h"
#include "../OOInteractionException.h"


#include "VisualizationBase/src/items/LayoutProvider.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"
#include "VisualizationBase/src/declarative/DeclarativeItemBase.h"
#include "VisualizationBase/src/declarative/GridLayoutFormElement.h"

namespace OOInteraction {

QMap<int, void (*)(GridBasedOffsetProvider* provider, Visualization::Item* item)>&
		GridBasedOffsetProvider::gridConstructors()
{
	static QMap<int, void (*)(GridBasedOffsetProvider* provider, Visualization::Item* item)> map;
	return map;
}

GridBasedOffsetProvider::GridBasedOffsetProvider(Visualization::Item* vis)
	: StringOffsetProvider{vis}
{
	auto gridConstructor = gridConstructors().find(vis->typeId());
	if (gridConstructor != gridConstructors().end())
	{
		(*gridConstructor)(this, vis);
		return;
	}

	// See if this item uses a sequential layout and use a standard way to handle it
	if (auto layoutProvider = DCast<Visualization::LayoutProvider<>>(vis))
	{
		setFilterNullAndEmptyComponents();
		for (int i = 0; i < layoutProvider->layout()->length(); ++i)
			add(new Cell{i, layoutProvider->layout()->at<Visualization::Item>(i), i});

		return;
	}

	// See if this item uses a declarative grid and use a standard way to handle it
	if (auto declarativeItem = DCast<Visualization::DeclarativeItemBase>(item()))
		if (auto gridLayout = dynamic_cast<Visualization::GridLayoutFormElement*>(declarativeItem->currentForm()))
		{
			setFilterNullAndEmptyComponents();
			for (int i = 0; i < gridLayout->length(declarativeItem); ++i)
			{
				auto cellItem = gridLayout->itemAt(declarativeItem, i);
				if (cellItem)
				{
					//TODO The const_cast below is a bit fishy. Should the grid layout store mutable objects to begin with?
					add(new Cell{i, const_cast<Visualization::Item*>(cellItem), i});
				}
			}

			return;
		}

	throw OOInteractionException("Creating an unknown GridBasedOffsetProvider for a visualization of type " +
			vis->typeName());
}

GridBasedOffsetProvider::~GridBasedOffsetProvider()
{
	for (auto c : cells_) SAFE_DELETE(c);
	cells_.clear();
}

bool GridBasedOffsetProvider::hasGridConstructorfor (Visualization::Item* item)
{
	return (gridConstructors().find(item->typeId()) != gridConstructors().end());
}

void GridBasedOffsetProvider::add(Cell* cell)
{
	for (auto c: cells_)
		if (c->region().contains(cell->region()) || cell->region().contains(c->region()))
			throw OOInteractionException("Trying to add an overlapping cell to GridOffsetProvider");

	cells_.append(cell);
	if (cell->region().right()+1 > size_.width()) size_.setWidth(cell->region().right()+1);
	if (cell->region().bottom()+1 > size_.height()) size_.setHeight(cell->region().bottom()+1);
}

Cell* GridBasedOffsetProvider::findCell(const QRect& start, Direction dir) const
{
	for (auto c : cells_)
	{
		if (start== c->region()) continue;

		switch (dir)
		{
			// Note that in the code below +1 is to correct for weird definition of QRect's right and bottom methods.
			case Left: if (c->region().right()+1 != start.left()
								|| c->region().bottom()+1 <= start.top()
								|| c->region().top() >= start.bottom()+1) continue;
			break;
			case Up: if (c->region().bottom()+1 != start.top()
								|| c->region().right()+1 <= start.left()
								|| c->region().left() >= start.right()+1) continue;
			break;
			case Right: if (c->region().left() != start.right()+1
								|| c->region().bottom()+1 <= start.top()
								|| c->region().top() >= start.bottom()+1) continue;
			break;
			case Down: if (c->region().top() != start.bottom()+1
								|| c->region().right()+1 <= start.left()
								|| c->region().left() >= start.right()+1) continue;
			break;
		}

		return c;
	}

	return nullptr;
}

inline bool GridBasedOffsetProvider::isOnTop(Cell* cell) const
{
	return cell->region().top() == 0;
}

inline bool GridBasedOffsetProvider::isOnLeft(Cell* cell) const
{
	return cell->region().left() == 0;
}

inline bool GridBasedOffsetProvider::isOnBottom(Cell* cell) const
{
	return cell->region().bottom()+1 == size_.height();
}

inline bool GridBasedOffsetProvider::isOnRight(Cell* cell) const
{
	return cell->region().right()+1 == size_.width();
}

int GridBasedOffsetProvider::offset(Qt::Key key)
{
	if (!item() || !item()->itemOrChildHasFocus()) return -1;

	int result = 0;
	QStringList components = this->components();

	Cell* target = nullptr;
	bool left = false;
	bool right = false;

	// Find the offset when the cursor is within one of the cells.
	for (auto c : cells_)
	{
		if (c->item()->itemOrChildHasFocus())
		{
			int length = 0;
			result = c->offset(components, key, &length);

			if (key == Qt::Key_Backspace && result == 0)
			{
				if (isOnLeft(c)) return 0;

				target = findCell(c->region(), Left);
				if (target) right = true;
				else
				{
					target = c;
					left = true;
				}
			}
			else if (key == Qt::Key_Delete && result == length)
			{
				if (isOnRight(c)) return components.join("").length();

				target = findCell(c->region(), Right);
				if (target) left = true;
				else
				{
					target = c;
					right = true;
				}
			}
			else target = c;

		}
	}

	// None of the cells has the cursor. Try a sequential layout.
	if (!target)
	{
		const Visualization::Item* leftItem = nullptr;
		const Visualization::Item* rightItem = nullptr;

		bool foundLayout = false;

		auto layout_provider = DCast<Visualization::LayoutProvider<>>(item());
		if (layout_provider && layout_provider->scene()->mainCursor()->owner() == layout_provider->layout())
		{
			foundLayout = true;
			int index = layout_provider->layout()->correspondingSceneCursor<Visualization::LayoutCursor>()->index();

			// Handle extra cursors that fall outside the shape.
			if ( layout_provider->layout()->style()->extraCursorsOutsideShape() )
			{
				if (index == -1) return 0;
				if (index > layout_provider->layout()->length())
					return components.join("").length();
			}
			else
			{
				if (key == Qt::Key_Backspace && index == 0) return 0;
				if (key == Qt::Key_Delete && index == layout_provider->layout()->length())
					return components.join("").length();
			}

			//Find the cells corresponding to the adjacent items
			leftItem = index > 0 ? layout_provider->layout()->at<Visualization::Item>(index-1) : nullptr;
			rightItem = index < layout_provider->layout()->length() ?
					layout_provider->layout()->at<Visualization::Item>(index) : nullptr;
		}
		else if (auto declarativeItem = DCast<Visualization::DeclarativeItemBase>(item()))
		{
			auto gridLayout = dynamic_cast<Visualization::GridLayoutFormElement*>(declarativeItem->currentForm());
			if (gridLayout && item()->scene()->mainCursor()->owner() == declarativeItem)
			{
				auto cursor = dynamic_cast<Visualization::LayoutCursor*>(item()->scene()->mainCursor());
				if (cursor && cursor->ownerElement() == gridLayout)
				{
					foundLayout = true;
					int index = cursor->index();

					int length = gridLayout->length(declarativeItem);

					// Handle extra cursors that fall outside the shape.
					// TODO@Mitko: is this the right check probably not?
					if ( ! gridLayout->noBoundaryCursors(item()) )
					{
						if (index == -1) return 0;
						if (index > length)
							return components.join("").length();
					}
					else
					{
						if (key == Qt::Key_Backspace && index == 0) return 0;
						if (key == Qt::Key_Delete && index == length)
							return components.join("").length();
					}

					//Find the cells corresponding to the adjacent items
					leftItem = index > 0 ? gridLayout->itemAt(declarativeItem, index-1) : nullptr;
					rightItem = index < length ? gridLayout->itemAt(declarativeItem, index) : nullptr;
				}
			}
		}

		if (foundLayout)
		{

			Cell* leftCell = nullptr;
			Cell* rightCell = nullptr;

			for (auto c : cells_)
			{
				if (c->item() == leftItem) leftCell = c;
				if (c->item() == rightItem) rightCell = c;
			}

			if (key == Qt::Key_Backspace && leftCell) rightCell = nullptr;
			if (key == Qt::Key_Delete && rightCell) leftCell = nullptr;

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
	if (right) for (int i = target->stringComponentsStart(); i <= target->stringComponentsEnd(); ++i)
		result += components[i].size();

	for (int i = 0; i < target->stringComponentsStart(); ++i) result += components[i].size();

	return result;
}

void GridBasedOffsetProvider::setOffset(int newOffset)
{
	QStringList components = this->components();
	Q_ASSERT(0<= newOffset && newOffset <= components.join("").size());

	int offset = newOffset;
	int index = 0;

	while ( index < components.size() && offset > components[index].size())
	{
		offset -= components[index].size();
		++index;
	}

	// Find the cells corresponding to the this index and the next (if any)
	Cell* indexCell = nullptr;
	Cell* nextCell = nullptr;
	for (auto c : cells_)
	{
		if (c->stringComponentsStart() <= index && c->stringComponentsEnd() >= index) indexCell = c;
		if (c->stringComponentsStart() <= index+1 && c->stringComponentsEnd() >= index+1) nextCell = c;
	}

	if (!indexCell && !nextCell)
	{
		if (newOffset == 0)
		{
			item()->moveCursor( Visualization::Item::MoveOnLeft);
			return;
		}
		else if (index+1 == components.size() && offset == components[index].size())
		{
			item()->moveCursor( Visualization::Item::MoveOnRight);
			return;
		}
		else throw OOInteractionException("Could not find adjacent cells when setting GridBased offset.");
	}

	// TODO choose a cell in possibly a smarter way
	if (indexCell) indexCell->setOffset(offset);
	else nextCell->setOffset(0);
}

QStringList GridBasedOffsetProvider::components()
{
	QStringList components = StringOffsetProvider::components();

	if (filterNullAndEmptyComponents_)
	{
		int length = -1;
		if ( auto layoutProvider = dynamic_cast<Visualization::LayoutProvider<>*>(item()) )
			length = layoutProvider->layout()->length();
		else if (auto declarativeItem = DCast<Visualization::DeclarativeItemBase>(item()))
			if (auto gridLayout = dynamic_cast<Visualization::GridLayoutFormElement*>(declarativeItem->currentForm()))
				length = gridLayout->length(declarativeItem);

		Q_ASSERT(length >= 0);

		if (components.size() != length)
		{
			for (int i = components.size() - 1; i>=0; --i)
				if (components[i].isNull())
					components.removeAt(i);
		}
		if (components.size() != length)
			components.removeAll(QString(""));
	}

	return components;
}

}
