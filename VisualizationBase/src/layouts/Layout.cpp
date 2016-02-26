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

#include "Layout.h"
#include "../VisualizationException.h"
#include "../renderer/ModelRenderer.h"

#include "../cursor/LayoutCursor.h"

namespace Visualization {

DEFINE_ITEM_COMMON( Layout, "layout" )

Layout::Layout(Item* parent, const StyleType* style) :
Super{parent, style}
{
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setAcceptedMouseButtons(Qt::LeftButton);
}

void Layout::setInnerSize(int width, int height)
{
	if (isEmpty() && !style()->drawShapeWhenEmpty()) setSize(0, 0);
	else
	{
		if (hasShape())
		{
			getShape()->setOffset(style()->leftMargin(), style()->topMargin());
			getShape()->setInnerSize(width, height);
			setWidth(widthInLocal() + style()->rightMargin());
			setHeight(heightInLocal() + style()->bottomMargin());
		}
		else
		{
			setWidth(width + style()->leftMargin() + style()->rightMargin());
			setHeight(height + style()->topMargin() + style()->bottomMargin());
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

}
