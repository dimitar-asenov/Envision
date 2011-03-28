/***********************************************************************************************************************
 * VIfStatement.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VIFSTATEMENT_H_
#define VIFSTATEMENT_H_

#include "../oovisualization_api.h"
#include "VIfStatementStyle.h"

#include "OOModel/headers/statements/IfStatement.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class PanelBorderLayout;
	class PositionLayout;
	class SequentialLayout;
	class VList;
}

namespace OOVisualization {

class OOVISUALIZATION_API VIfStatement : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::IfStatement>
{
	ITEM_COMMON(VIfStatement)

	public:
		VIfStatement(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VIfStatement();

	protected:
		void determineChildren();

	private:
		Visualization::SequentialLayout* header;
		Visualization::SequentialLayout* conditionBackground;
		Visualization::Item* condition;
		Visualization::SequentialLayout* content;
		Visualization::VList* thenBranch;
		Visualization::VList* elseBranch;
};

}

#endif /* VIFSTATEMENT_H_ */
