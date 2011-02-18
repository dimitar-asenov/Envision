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

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class PanelBorderLayout;
	class PositionLayout;
	class SequentialLayout;
}

namespace OOModel {
	class IfStatement;
}

namespace OOVisualization {

class OOVISUALIZATION_API VIfStatement : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::IfStatement>
{
	ITEM_COMMON(VIfStatement)

	public:
		VIfStatement(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VIfStatement>("default"));
		virtual ~VIfStatement();

	protected:
		void determineChildren();

	private:
		Visualization::SequentialLayout* header;
		Visualization::SequentialLayout* conditionBackground;
		Visualization::Item* condition;
		Visualization::SequentialLayout* content;
		Visualization::SequentialLayout* thenBranchBackground;
		Visualization::SequentialLayout* elseBranchBackground;
		Visualization::Item* thenBranch;
		Visualization::Item* elseBranch;
};

}

#endif /* VIFSTATEMENT_H_ */
