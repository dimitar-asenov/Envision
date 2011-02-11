/***********************************************************************************************************************
 * VStringLiteral.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VSTRINGLITERAL_H_
#define VSTRINGLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/expressions/StringLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VStringLiteral : public Visualization::ModelItem
{
	ITEM_COMMON_CUSTOM_STYLENAME(VStringLiteral, Visualization::ModelItem, Visualization::TextStyle)

	public:
		VStringLiteral(Item* parent, OOModel::StringLiteral *literal, const Visualization::TextStyle *style = Visualization::Styles::item<VStringLiteral>("default"));
		virtual ~VStringLiteral();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::ModelItem* vis_;
};

}

#endif /* VSTRINGLITERAL_H_ */
