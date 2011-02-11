/***********************************************************************************************************************
 * VBooleanLiteral.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VBOOLEANLITERAL_H_
#define VBOOLEANLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/expressions/BooleanLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VBooleanLiteral : public Visualization::ModelItem
{
	ITEM_COMMON_CUSTOM_STYLENAME(VBooleanLiteral, Visualization::ModelItem, Visualization::TextStyle)

	public:
		VBooleanLiteral(Item* parent, OOModel::BooleanLiteral *literal, const Visualization::TextStyle *style = Visualization::Styles::item<VBooleanLiteral>("default"));
		virtual ~VBooleanLiteral();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::ModelItem* vis_;
};

}

#endif /* VBOOLEANLITERAL_H_ */
