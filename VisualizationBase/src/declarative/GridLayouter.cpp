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

#include "GridLayouter.h"
#include "../cursor/LayoutCursor.h"

namespace Visualization {

ItemRegion GridLayouter::cursorRegion(Item* parent, FormElement* formElement, int xIndex, int yIndex,
		MajorAxis majorAxis, bool atBoundary, bool notLocationEquivalent, bool mayExpandFront, bool mayExpandBack,
		QRect area)
{
	auto horizontal = majorAxis != ColumnMajor;

	// Make sure there is at least some space for the cursor Region.
	if (horizontal && area.width() == 0) area.adjust((mayExpandFront?-1:0), 0, (mayExpandBack?1:0), 0);
	if (!horizontal && area.height() == 0 ) area.adjust(0, (mayExpandFront?-1:0), 0, (mayExpandBack?1:0));

	// Note below, that a horizontal layout, means a vertical cursor
	auto lc = new LayoutCursor(parent, horizontal ? Cursor::VerticalCursor : Cursor::HorizontalCursor);
	lc->setOwnerElement(formElement);
	lc->set2DIndex(xIndex, yIndex);
	lc->setVisualizationSize( area.size() );
	lc->setVisualizationPosition( area.topLeft() );
	lc->setRegion( area );
	lc->setIsAtBoundary(atBoundary);
	lc->setNotLocationEquivalent(notLocationEquivalent);

	auto region = ItemRegion(area);
	region.setCursor(lc);
	return region;
}

}
