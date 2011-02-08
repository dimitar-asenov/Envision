/***********************************************************************************************************************
 * VReferenceExpressionStyle.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VReferenceExpressionStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VReferenceExpressionStyle* VReferenceExpressionStyle::getDefault()
{
	static VReferenceExpressionStyle defaultStyle;
	return &defaultStyle;
}

void VReferenceExpressionStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("container", container_);
	Visualization::Styles::load("name", name_);
	Visualization::Styles::load("separator", separator_);
}

}
