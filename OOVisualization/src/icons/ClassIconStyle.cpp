/***********************************************************************************************************************
 * ClassIconStyle.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "icons/ClassIconStyle.h"

namespace OOVisualization {

void ClassIconStyle::load(Visualization::StyleLoader& sl)
{
	IconStyle::load(sl);
	sl.load("width", width_);
	sl.load("rectWidth", rectWidth_);
	sl.load("rectHeight", rectHeight_);
}

}
