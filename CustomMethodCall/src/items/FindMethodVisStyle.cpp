/***********************************************************************************************************************
 * FindMethodVisStyle.cpp
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/FindMethodVisStyle.h"

namespace CustomMethodCall {

void FindMethodVisStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("name", name_);
	sl.load("separator", separator_);
	sl.load("arguments", arguments_);
}

}
