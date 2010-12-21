/***********************************************************************************************************************
 * BoxTest.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "BoxTest.h"

#include "shapes/Box.h"
#include "items/Text.h"

namespace Visualization {

BoxTest::BoxTest(Item* parent, int sub) :
		Item(parent), items(this)
{
	setFlag(QGraphicsItem::ItemIsMovable);
//	style.setMargins(4);
//	style.setSpaceBetweenElements(3);
//	style.setAlignment(SequentialLayoutStyle::CenterAlignment);
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

void BoxTest::updateState()
{
	getShape()->setInnerSize(items.width(), items.height());
	items.setPos(getShape()->contentLeft(), getShape()->contentTop());
}

}
