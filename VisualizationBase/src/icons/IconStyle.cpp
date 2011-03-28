/***********************************************************************************************************************
 * IconStyle.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "icons/IconStyle.h"

namespace Visualization {

void IconStyle::load(StyleLoader& sl)
{
	ItemStyle::load(sl);
	sl.load("outline", outline_);
}

}
