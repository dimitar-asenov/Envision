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
 * PanelBorderLayoutStyle.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PanelBorderLayoutStyle.h"

namespace Visualization {

PanelBorderLayoutStyle::PanelBorderLayoutStyle() :
	leftInnerMargin_(5), rightInnerMargin_(5), topInnerMargin_(5), bottomInnerMargin_(5)
{
}

void PanelBorderLayoutStyle::setInnerMargins(int marginSize)
{
	if ( marginSize >= 0 )
	{
		leftInnerMargin_ = marginSize;
		rightInnerMargin_ = marginSize;
		topInnerMargin_ = marginSize;
		bottomInnerMargin_ = marginSize;
	}
}

void PanelBorderLayoutStyle::setInnerMargins(int left, int right, int top, int bottom)
{
	if ( left >= 0 ) leftInnerMargin_ = left;
	if ( right >= 0 ) rightInnerMargin_ = right;
	if ( top >= 0 ) topInnerMargin_ = top;
	if ( bottom >= 0 ) bottomInnerMargin_ = bottom;
}

void PanelBorderLayoutStyle::load(StyleLoader& sl)
{
	LayoutStyle::load(sl);
	sl.load("topPanelStyle", topStyle_);
	sl.load("leftPanelStyle", leftStyle_);
	sl.load("bottomPanelStyle", bottomStyle_);
	sl.load("rightPanelStyle", rightStyle_);

	sl.load("leftInnerMargin", leftInnerMargin_);
	sl.load("rightInnerMargin", rightInnerMargin_);
	sl.load("topInnerMargin", topInnerMargin_);
	sl.load("bottomInnerMargin", bottomInnerMargin_);

	sl.load("isLeftProtrusionFixed", leftProtrusionFixed_);
	if (leftProtrusionFixed_) sl.load("leftProtrusion", leftProtrusion_);

	sl.load("shapeOnlyOnContent", shapeOnlyOnContent_);
}

}
