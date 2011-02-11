/***********************************************************************************************************************
 * VStringLiteral.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VSTRINGLITERAL_H_
#define VSTRINGLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/VText.h"

#include "OOModel/headers/expressions/StringLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VStringLiteral : public Visualization::VText
{
	ITEM_COMMON_CUSTOM_STYLENAME(VStringLiteral, Visualization::VText, Visualization::TextStyle)

	public:
		VStringLiteral(Item* parent, OOModel::StringLiteral *literal, const Visualization::TextStyle *style = Visualization::Styles::item<VStringLiteral>("default"));
};

}

#endif /* VSTRINGLITERAL_H_ */
