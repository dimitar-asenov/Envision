/***********************************************************************************************************************
 * IconStyle.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "icons/IconStyle.h"
#include "Styles.h"

namespace Visualization {

void IconStyle::load()
{
	ItemStyle::load();
	Styles::load("outline", outline_);
}

}
