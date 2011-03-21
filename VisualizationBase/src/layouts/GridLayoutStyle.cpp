/***********************************************************************************************************************
 * GridLayoutStyle.cpp
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/GridLayoutStyle.h"
#include "Styles.h"

namespace Visualization {

GridLayoutStyle* GridLayoutStyle::getDefault()
{
	static GridLayoutStyle s;
	return &s;
}

void GridLayoutStyle::load()
{
	LayoutStyle::load();
	Styles::load("horizontalAlignment", (int&) horizontalAlignment_);
	Styles::load("verticalAlignment", (int&) verticalAlignment_);
	Styles::load("horizontalSpace", horizontalSpace_);
	Styles::load("verticalSpace", verticalSpace_);
}

}
