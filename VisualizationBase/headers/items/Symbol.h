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

class VISUALIZATIONBASE_API Symbol : public TextRenderer
{
	ITEM_COMMON(Symbol)

	public:
		Symbol(Item* parent, const StyleType *style);
		virtual bool setText(const QString& newText);
};

}

#endif /* SYMBOL_H_ */
