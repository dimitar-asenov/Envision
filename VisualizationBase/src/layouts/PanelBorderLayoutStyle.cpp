/***********************************************************************************************************************
 * PanelBorderLayoutStyle.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/PanelBorderLayoutStyle.h"

namespace Visualization {

PanelBorderLayoutStyle::PanelBorderLayoutStyle() :
	leftInnerMargin_(5), rightInnerMargin_(5), topInnerMargin_(5), bottomInnerMargin_(5)
{
}

PanelBorderLayoutStyle* PanelBorderLayoutStyle::getDefault()
{
	static PanelBorderLayoutStyle defaultStyle;
	return &defaultStyle;
}

}
