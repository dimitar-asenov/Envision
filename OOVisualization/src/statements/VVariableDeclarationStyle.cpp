/***********************************************************************************************************************
 * VVariableDeclarationStyle.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VVariableDeclarationStyle.h"

namespace OOVisualization {

void VVariableDeclarationStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("layout", layout_);
	sl.load("name", name_);
	sl.load("assignmentSymbol", assignmentSymbol_);
}

}
