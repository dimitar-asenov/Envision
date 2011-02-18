/***********************************************************************************************************************
 * VBooleanLiteral.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VBOOLEANLITERAL_H_
#define VBOOLEANLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/expressions/BooleanLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VBooleanLiteral : public Visualization::ItemWithNode< Visualization::Item, OOModel::BooleanLiteral>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VBooleanLiteral, Visualization::TextStyle)

	public:
		VBooleanLiteral(Item* parent, NodeType *literal, const StyleType *style = Visualization::Styles::item<VBooleanLiteral>("default"));
		virtual ~VBooleanLiteral();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::Item* vis_;
};

}

#endif /* VBOOLEANLITERAL_H_ */
