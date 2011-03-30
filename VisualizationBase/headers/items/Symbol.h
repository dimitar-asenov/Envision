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
		Symbol(Item* parent, const StyleType *style = itemStyles().get());
		virtual bool setText(const QString& newText);
		virtual bool isEmpty() const;

	protected:
		virtual QString currentText();
};

}

#endif /* SYMBOL_H_ */
