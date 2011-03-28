/***********************************************************************************************************************
 * GridLayoutStyle.cpp
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/GridLayoutStyle.h"

namespace Visualization {

void GridLayoutStyle::load(StyleLoader& sl)
{
	LayoutStyle::load(sl);
	sl.load("horizontalAlignment", (int&) horizontalAlignment_);
	sl.load("verticalAlignment", (int&) verticalAlignment_);
	sl.load("horizontalSpace", horizontalSpace_);
	sl.load("verticalSpace", verticalSpace_);
}

}
