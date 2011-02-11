/***********************************************************************************************************************
 * Symbol.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include "../visualizationbase_api.h"

#include "TextRenderer.h"
#include "SymbolStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API Symbol : public TextRenderer<Item>
{
	ITEM_COMMON(Symbol, TextRenderer<Item>)

	public:
		Symbol(Item* parent, const SymbolStyle *style);
		virtual bool setText(const QString& newText);
};

}

#endif /* SYMBOL_H_ */
