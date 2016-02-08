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

#include "LayoutCursor.h"
#include "CursorShapeItem.h"
#include "../layouts/Layout.h"

namespace Visualization {

LayoutCursor::LayoutCursor(Item* owner, CursorType type)
	: Cursor{owner, type, new CursorShapeItem{this}}, x_{0}, y_{0}, index_{0}, isAtBoundary_{false},
	  ownerElement_{nullptr}
{
}

LayoutCursor* LayoutCursor::clone() const
{
	return new LayoutCursor{*this};
}

void LayoutCursor::update()
{
	for (auto& r : owner()->regions())
	{
		if (isSame(r.cursor()))
		{
			auto lc = static_cast<LayoutCursor*>(r.cursor());
			auto vis = static_cast<CursorShapeItem*>(lc->visualization());
			setVisualizationSize(vis->cursorSize());
			setVisualizationPosition(lc->position());
		}

		if (r.cursor()) delete r.cursor();
	}
}

void LayoutCursor::setVisualizationSize(const QSize& size)
{
	CursorShapeItem* ci = static_cast<CursorShapeItem*> (visualization());
	ci->setCursorSize(size);
}

void LayoutCursor::setVisualizationPosition(const QPoint& pos)
{
	setPosition(pos);
	CursorShapeItem* ci = static_cast<CursorShapeItem*> (visualization());
	ci->setCursorTopLeft(position());
}

bool LayoutCursor::isSame(Cursor* c)
{
	auto lc = dynamic_cast<LayoutCursor*>(c);
	if (lc)
		return lc->owner() == owner() && lc->x() == x() && lc->y() == y() && lc->index() == index()
				&& lc->ownerElement() == ownerElement();

	return false;
}

bool LayoutCursor::isAtBoundary() const
{
	return isAtBoundary_;
}

}
