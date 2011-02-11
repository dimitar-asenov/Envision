/***********************************************************************************************************************
 * VVariableDeclarationStyle.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VVariableDeclarationStyle.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

VVariableDeclarationStyle* VVariableDeclarationStyle::getDefault()
{
	static VVariableDeclarationStyle defaultStyle;
	return &defaultStyle;
}

void VVariableDeclarationStyle::load()
{
	ItemStyle::load();

	Visualization::Styles::load("container", container_);
	Visualization::Styles::load("name", name_);
	Visualization::Styles::load("assignmentStyle", assignmentStyle_);
	Visualization::Styles::load("assignmentSymbol", assignmentSymbol_);
}

}
