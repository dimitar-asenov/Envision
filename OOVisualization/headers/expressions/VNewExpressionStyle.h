/***********************************************************************************************************************
 * VNewExpressionStyle.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VNEWEXPRESSIONSTYLE_H_
#define VNEWEXPRESSIONSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VNewExpressionStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle container_;
		Visualization::SymbolStyle amountSymbol_;
		Visualization::SymbolStyle newSymbol_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& container() const;
		const Visualization::SymbolStyle& amountSymbol() const;
		const Visualization::SymbolStyle& newSymbol() const;

		static VNewExpressionStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& VNewExpressionStyle::container() const { return container_; }
inline const Visualization::SymbolStyle& VNewExpressionStyle::amountSymbol() const { return amountSymbol_; }
inline const Visualization::SymbolStyle& VNewExpressionStyle::newSymbol() const { return newSymbol_; }

}

#endif /* VNEWEXPRESSIONSTYLE_H_ */
