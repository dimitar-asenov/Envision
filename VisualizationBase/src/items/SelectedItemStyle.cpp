/***********************************************************************************************************************
 * SelectedItemStyle.cpp
 *
 *  Created on: Jan 20, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/SelectedItemStyle.h"

namespace Visualization {

SelectedItemStyle* SelectedItemStyle::getDefault()
{
	static SelectedItemStyle defaultStyle;
	return &defaultStyle;
}

void SelectedItemStyle::load()
{
	ItemStyle::load();

}

}
