/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * Layout.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/Layout.h"
#include "VisualizationException.h"
#include "ModelRenderer.h"

#include "cursor/LayoutCursor.h"

#include "Core/headers/global.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( Layout, "layout" )

Layout::Layout(Item* parent, const StyleType* style) :
	Item(parent, style)
{
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setAcceptedMouseButtons(Qt::LeftButton);
}

void Layout::setInnerSize(int width_, int height_)
{
	if (isEmpty() && !style()->drawShapeWhenEmpty()) setSize(0,0);
	else
	{
		if ( hasShape() )
		{
			getShape()->setOffset(style()->leftMargin(), style()->topMargin());
			getShape()->setInnerSize(width_, height_);
			setWidth(width() + style()->rightMargin());
			setHeight(height() + style()->bottomMargin());
		}
		else
		{
			setWidth(width_ + style()->leftMargin() + style()->rightMargin());
			setHeight(height_ + style()->topMargin() + style()->bottomMargin());
		}
	}
}

void Layout::determineChildren()
{
}

int Layout::xOffset() const
{
	if ( hasShape() ) return getShape()->contentLeft();
	else return style()->leftMargin();
}

int Layout::yOffset() const
{
	if ( hasShape() ) return getShape()->contentTop();
	else return style()->topMargin();
}

void Layout::synchronizeItem(Item*& layoutItem, Item*& externalItem, Model::Node* node)
{
	// When refactoring this method have in mind that layoutItem might point to the same item as externalItem.


	if (externalItem != layoutItem)
	{
		SAFE_DELETE_ITEM(layoutItem);
		setUpdateNeeded();
	}

	if (externalItem && externalItem->node() != node )
	{
		SAFE_DELETE_ITEM(externalItem);
		layoutItem = nullptr; // One of the safe deletes above deleted this item
		setUpdateNeeded();
	}

	if (!externalItem && node)
	{
		externalItem = renderer()->render(nullptr, node);
		externalItem->setParentItem(this);
		layoutItem = externalItem;
		setUpdateNeeded();
	}
}

QList<LayoutRegion> Layout::regions()
{
	QList<LayoutRegion> regs;

	for(auto item : childItems())
	{
		Item* child = static_cast<Item*> (item);
		QRect rect = child->boundingRect().toRect();
		rect.translate(child->pos().toPoint());
		regs.append(LayoutRegion(rect));
		regs.last().setChild(child);
	}

	return regs;
}

bool Layout::moveCursor(CursorMoveDirection dir, const QPoint& reference)
{
	// The condition below is only true if this method is called from a parent item, after it was already called for the
	// item itself. In that case simply return false as the original call returned false too.
	if (scene()->mainCursor() && scene()->mainCursor()->owner() == this &&
			(dir == MoveUpOf || dir == MoveDownOf || dir == MoveLeftOf || dir == MoveRightOf) )
	return false;

	QList<LayoutRegion> regs = regions();

	if (regs.isEmpty()) return false;

	QPoint source = reference;

	LayoutRegion* current = nullptr;
	// Handle cursor movement in a specific direction
	if (dir == MoveUp || dir == MoveDown || dir == MoveLeft || dir == MoveRight)
	{
		int index = correspondingSceneCursor<LayoutCursor>()->index();
		int x = correspondingSceneCursor<LayoutCursor>()->x();
		int y = correspondingSceneCursor<LayoutCursor>()->y();

		// Find which region corresponds to the current cursor and set the source point accordingly
		for (LayoutRegion& r : regs)
		{
			if (r.cursor() && r.cursor()->x() == x && r.cursor()->y() == y && r.cursor()->index() == index)
			{
				current = &r;
				switch(dir)
				{
					case MoveUp: source = QPoint(reference.x(), r.region().y()); break;
					case MoveDown: source = QPoint(reference.x(), r.region().y() + r.region().height()); break;
					case MoveLeft: source = QPoint(r.region().x(), reference.y()); break;
					case MoveRight: source = QPoint(r.region().x() + r.region().width(), reference.y()); break;
					default: /* Will never be the case because of the top-level if statement */ break;
				}
				break;
			}

		}
	}

	// Find the closest region that matches the constraints
	LayoutRegion::PositionConstraint constraints = LayoutRegion::NoConstraints;
	switch(dir)
	{
		case MoveUp: constraints = LayoutRegion::Above; break;
		case MoveDown: constraints = LayoutRegion::Below; break;
		case MoveLeft: constraints = LayoutRegion::LeftOf; break;
		case MoveRight: constraints = LayoutRegion::RightOf; break;
		case MoveOnPosition: constraints = LayoutRegion::NoConstraints; break;
		case MoveUpOf: constraints = LayoutRegion::Above; break;
		case MoveDownOf: constraints = LayoutRegion::Below; break;
		case MoveLeftOf: constraints = LayoutRegion::LeftOf; break;
		case MoveRightOf: constraints = LayoutRegion::RightOf; break;
	}

	LayoutRegion* best = nullptr;
	int best_distance;
	for (LayoutRegion& r : regs)
	{
		LayoutRegion::PositionConstraints satisfied = r.satisfiedPositionConstraints(source);
		if (	(&r != current)
				&& ( (( satisfied & constraints) == constraints)
						// An overlapping cursor region is preferred, even if it does not satisfy the constraints.
						// This happens in 'tight' layouts that do not leave any spacing between the elements and so the
						// Cursors are overlapping the normal items. At the borders of such containers the alternative below
						// is needed.
						|| (r.cursor() && (satisfied & LayoutRegion::Overlap)) )
				&& (!best
						|| r.distanceTo(source) < best_distance
						// Prioritize cursor regions on top of item regions.
						|| (r.cursor() && r.distanceTo(source) <= best_distance))
			)
		{
			best_distance = r.distanceTo(source);
			best = &r;
		}
	}

	// Delete the cursors of all regions but the best
	for (LayoutRegion& r : regs)
		if (&r != best) delete r.cursor();

	if (best == nullptr) return false;

	// A region was found, focus it
	if (best->cursor())
	{
		// This is a cursor region
		scene()->setMainCursor(best->cursor());
		setFocus();
		return true;
	}
	else if (best->child())
	{
		// This is a child item region

		// Only accept this as a correct focus event if this child is not already focused.
		if (best->child()->itemOrChildHasFocus()) return false;

		CursorMoveDirection childDirection;
		switch(dir)
		{
			case MoveUp: childDirection = MoveUpOf; break;
			case MoveDown: childDirection = MoveDownOf; break;
			case MoveLeft: childDirection = MoveLeftOf; break;
			case MoveRight: childDirection = MoveRightOf; break;
			case MoveOnPosition: childDirection = MoveOnPosition; break;
			case MoveUpOf: childDirection = MoveUpOf; break;
			case MoveDownOf: childDirection = MoveDownOf; break;
			case MoveLeftOf: childDirection = MoveLeftOf; break;
			case MoveRightOf: childDirection = MoveRightOf; break;
		}
		return best->child()->moveCursor(childDirection, mapToItem(best->child(), source).toPoint());
	}
	else
	{
		throw VisualizationException("Encountered a layout region with no cursor or child item.");
	}
}

}
