/***********************************************************************************************************************
 * VStringLiteralStyle.h
 *
 *  Created on: Mar 10, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VSTRINGLITERALSTYLE_H_
#define VSTRINGLITERALSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VStringLiteralStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::TextStyle string_;
		Visualization::SymbolStyle preSymbol_;
		Visualization::SymbolStyle postSymbol_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::TextStyle& string() const;
		const Visualization::SymbolStyle& preSymbol() const;
		const Visualization::SymbolStyle& postSymbol() const;
};

inline const Visualization::SequentialLayoutStyle& VStringLiteralStyle::layout() const { return layout_; }
inline const Visualization::TextStyle& VStringLiteralStyle::string() const { return string_; }
inline const Visualization::SymbolStyle& VStringLiteralStyle::preSymbol() const { return preSymbol_; }
inline const Visualization::SymbolStyle& VStringLiteralStyle::postSymbol() const { return postSymbol_; }

}

#endif /* VSTRINGLITERALSTYLE_H_ */
