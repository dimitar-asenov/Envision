/***********************************************************************************************************************
 * EmptyMethodVisStyle.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/EmptyMethodVisStyle.h"

namespace CustomMethodCall {

void EmptyMethodVisStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("icon", icon_);
	sl.load("separator", separator_);
}

}
