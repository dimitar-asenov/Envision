/***********************************************************************************************************************
 * SymbolStyle.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SYMBOLSTYLE_H_
#define SYMBOLSTYLE_H_

#include "../visualizationbase_api.h"

#include "TextStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API SymbolStyle : public TextStyle
{
	private:
		QString symbol_;

	public:
		void load(StyleLoader& sl);

		const QString& symbol() const;
};

inline const QString& SymbolStyle::symbol() const { return symbol_; }


}

#endif /* SYMBOLSTYLE_H_ */
