/***********************************************************************************************************************
 * VCastExpressionStyle.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VCASTEXPRESSIONSTYLE_H_
#define VCASTEXPRESSIONSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/StaticStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VCastExpressionStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
};

inline const Visualization::SequentialLayoutStyle& VCastExpressionStyle::layout() const { return layout_; }

}

#endif /* VCASTEXPRESSIONSTYLE_H_ */
