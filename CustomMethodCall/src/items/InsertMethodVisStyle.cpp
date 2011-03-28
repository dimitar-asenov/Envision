/***********************************************************************************************************************
 * InsertMethodVisStyle.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/InsertMethodVisStyle.h"

namespace CustomMethodCall {

void InsertMethodVisStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("icon", icon_);
	sl.load("separator", separator_);
	sl.load("arguments", arguments_);
}

}
