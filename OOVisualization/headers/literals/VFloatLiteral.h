/***********************************************************************************************************************
 * VFloatLiteral.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFLOATLITERAL_H_
#define VFLOATLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/VFloat.h"

#include "OOModel/headers/expressions/FloatLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VFloatLiteral : public Visualization::VFloat
{
	ITEM_COMMON_CUSTOM_STYLENAME(VFloatLiteral, Visualization::VFloat, Visualization::TextStyle)

	public:
		VFloatLiteral(Item* parent, OOModel::FloatLiteral *literal, const Visualization::TextStyle *style = Visualization::Styles::item<VFloatLiteral>("default"));
};

}

#endif /* VFLOATLITERAL_H_ */
