/***********************************************************************************************************************
 * VContinueStatement.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VCONTINUESTATEMENT_H_
#define VCONTINUESTATEMENT_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/Static.h"

#include "OOModel/headers/statements/ContinueStatement.h"

namespace OOVisualization {

class OOVISUALIZATION_API VContinueStatement : public Visualization::ItemWithNode< Visualization::Item, OOModel::ContinueStatement>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VContinueStatement, Visualization::StaticStyle)

	public:
		VContinueStatement(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VContinueStatement();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::Static* vis_;
};

}

#endif /* VCONTINUESTATEMENT_H_ */
