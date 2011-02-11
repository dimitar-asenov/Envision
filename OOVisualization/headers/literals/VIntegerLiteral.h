/***********************************************************************************************************************
 * VIntegerLiteral.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VINTEGERLITERAL_H_
#define VINTEGERLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/VInteger.h"

#include "OOModel/headers/expressions/IntegerLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VIntegerLiteral : public Visualization::VInteger
{
	ITEM_COMMON_CUSTOM_STYLENAME(VIntegerLiteral, Visualization::VInteger, Visualization::TextStyle)

	public:
		VIntegerLiteral(Item* parent, OOModel::IntegerLiteral *literal, const Visualization::TextStyle *style = Visualization::Styles::item<VIntegerLiteral>("default"));
};

}

#endif /* VINTEGERLITERAL_H_ */
