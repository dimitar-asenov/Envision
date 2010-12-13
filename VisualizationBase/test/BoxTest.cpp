/***********************************************************************************************************************
 * BoxTest.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "BoxTest.h"

#include "shapes/Box.h"

namespace Visualization {

BoxTest::BoxTest(Item* parent, int sub) :
		Item(parent, new Box(this)), items(this, &style)
{
	style.setMargins(4);
	style.setSpaceBetweenElements(2);
	style.setAlignment(SequentialLayoutStyle::CenterAlignment);
	style.setDirection(SequentialLayoutStyle::BottomToTop);

	for (int i = 0; i < sub; ++i)
		items.append(new BoxTest(this, sub - 1 - i));
}

void BoxTest::determineChildren()
{
}

void BoxTest::updateState()
{
	shape()->setInnerSize(items.width(), items.height());
}

}
