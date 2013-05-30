/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "BoxTest.h"

#include "shapes/Box.h"
#include "items/Text.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(BoxTest, "item")

BoxTest::BoxTest(Item* parent, int sub) :
Super(parent), items(this)
{
	setFlag(QGraphicsItem::ItemIsMovable);
//	style.setMargins(4);
//	style.setSpaceBetweenElements(3);
//	style.setAlignment(SequentialLayoutStyle::Alignment::Center);
//	style.setDirection(SequentialLayoutStyle::LeftToRight);
//
//	boxStyle.setCorner(BoxStyle::RoundCorner,10);
//	boxStyle.setOutline(QPen(QBrush(QColor(0x00,0x00,0xFF)),1));
//	boxStyle.setShadow(QBrush(QColor(0xFF,0xAA,0xAA)),2,1, false);
//	setShapeStyle(&boxStyle);

	for (int i = 0; i < sub; ++i)
		items.append(new BoxTest(&items, sub - 1 - i));

	if (sub == 0)
	{
		items.append(new Text(&items,"Test"));
	}
}

void BoxTest::determineChildren()
{
}

void BoxTest::updateGeometry(int, int)
{
	getShape()->setInnerSize(items.width(), items.height());
	items.setPos(getShape()->contentLeft(), getShape()->contentTop());
}

}
