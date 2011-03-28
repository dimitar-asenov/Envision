/***********************************************************************************************************************
 * VArrayTypeStyle.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VARRAYTYPESTYLE_H_
#define VARRAYTYPESTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VArrayTypeStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::SymbolStyle symbol_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::SymbolStyle& symbol() const;
};

inline const Visualization::SequentialLayoutStyle& VArrayTypeStyle::layout() const { return layout_; }
inline const Visualization::SymbolStyle& VArrayTypeStyle::symbol() const { return symbol_; }

}

#endif /* VARRAYTYPESTYLE_H_ */
