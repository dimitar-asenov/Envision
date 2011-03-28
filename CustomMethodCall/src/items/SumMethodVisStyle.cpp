/***********************************************************************************************************************
 * SumMethodVisStyle.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/SumMethodVisStyle.h"

namespace CustomMethodCall {

void SumMethodVisStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("name", name_);
	sl.load("arguments", arguments_);
}

}
