/***********************************************************************************************************************
 * SymbolStyle.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/SymbolStyle.h"
#include "Styles.h"

namespace Visualization {

SymbolStyle* SymbolStyle::getDefault()
{
	static SymbolStyle defaultStyle;
	return &defaultStyle;
}

void SymbolStyle::load()
{
	ItemStyle::load();

	Styles::load("symbol", symbol_);
}

}
