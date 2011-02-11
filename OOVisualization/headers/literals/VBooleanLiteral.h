/***********************************************************************************************************************
 * VBooleanLiteral.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VBOOLEANLITERAL_H_
#define VBOOLEANLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/VBoolean.h"

#include "OOModel/headers/expressions/BooleanLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VBooleanLiteral : public Visualization::VBoolean
{
	ITEM_COMMON_CUSTOM_STYLENAME(VBooleanLiteral, Visualization::VBoolean, Visualization::TextStyle)

	public:
		VBooleanLiteral(Item* parent, OOModel::BooleanLiteral *literal, const Visualization::TextStyle *style = Visualization::Styles::item<VBooleanLiteral>("default"));
};

}

#endif /* VBOOLEANLITERAL_H_ */
