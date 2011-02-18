/***********************************************************************************************************************
 * VIntegerLiteral.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VINTEGERLITERAL_H_
#define VINTEGERLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/expressions/IntegerLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VIntegerLiteral : public Visualization::ItemWithNode< Visualization::Item, OOModel::IntegerLiteral>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VIntegerLiteral, Visualization::TextStyle)

	public:
		VIntegerLiteral(Item* parent, NodeType *literal, const StyleType *style = Visualization::Styles::item<VIntegerLiteral>("default"));
		virtual ~VIntegerLiteral();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::Item* vis_;
};

}

#endif /* VINTEGERLITERAL_H_ */
