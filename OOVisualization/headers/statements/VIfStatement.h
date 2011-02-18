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

class OOVISUALIZATION_API VIfStatement : public Visualization::ModelItem
{
	ITEM_COMMON(VIfStatement)

	public:
		VIfStatement(Item* parent, OOModel::IfStatement* node, const VIfStatementStyle* style = Visualization::Styles::item<VIfStatement>("default"));
		virtual ~VIfStatement();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::PanelBorderLayout* layout;
		Visualization::SequentialLayout* header;
		Visualization::SequentialLayout* conditionBackground;
		Visualization::ModelItem* condition;
		Visualization::SequentialLayout* content;
		Visualization::SequentialLayout* thenBranchBackground;
		Visualization::SequentialLayout* elseBranchBackground;
		Visualization::ModelItem* thenBranch;
		Visualization::ModelItem* elseBranch;
};

}

#endif /* VIFSTATEMENT_H_ */
