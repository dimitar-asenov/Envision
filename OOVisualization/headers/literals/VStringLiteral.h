/***********************************************************************************************************************
 * VStringLiteral.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VSTRINGLITERAL_H_
#define VSTRINGLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/items/VText.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/expressions/StringLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VStringLiteral : public Visualization::ItemWithNode< Visualization::Item, OOModel::StringLiteral>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VStringLiteral, Visualization::TextStyle)

	public:
		VStringLiteral(Item* parent, NodeType *literal, const StyleType *style = Visualization::Styles::item<VStringLiteral>("default"));
		virtual ~VStringLiteral();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::VText* vis_;
};

}

#endif /* VSTRINGLITERAL_H_ */
