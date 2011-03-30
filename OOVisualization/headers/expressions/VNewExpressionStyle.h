/***********************************************************************************************************************
 * VNewExpressionStyle.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VNEWEXPRESSIONSTYLE_H_
#define VNEWEXPRESSIONSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/StaticStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VNewExpressionStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::StaticStyle newSymbol_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::StaticStyle& newSymbol() const;
};

inline const Visualization::SequentialLayoutStyle& VNewExpressionStyle::layout() const { return layout_; }
inline const Visualization::StaticStyle& VNewExpressionStyle::newSymbol() const { return newSymbol_; }

}

#endif /* VNEWEXPRESSIONSTYLE_H_ */
