/***********************************************************************************************************************
 * VLoopStatement.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLOOPSTATEMENT_H_
#define VLOOPSTATEMENT_H_

#include "../oovisualization_api.h"
#include "VLoopStatementStyle.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class PanelBorderLayout;
	class PositionLayout;
	class SequentialLayout;
}

namespace OOModel {
	class LoopStatement;
}

namespace OOVisualization {

class OOVISUALIZATION_API VLoopStatement : public Visualization::ModelItem
{
	ITEM_COMMON(VLoopStatement, Visualization::ModelItem)

	public:
		VLoopStatement(Item* parent, OOModel::LoopStatement* node, const VLoopStatementStyle* style = Visualization::Styles::item<VLoopStatement>("default"));
		virtual ~VLoopStatement();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::PanelBorderLayout* layout;
		Visualization::SequentialLayout* header;

		Visualization::SequentialLayout* conditionBackground;
		Visualization::SequentialLayout* initStepBackground;
		Visualization::SequentialLayout* updateStepBackground;

		Visualization::ModelItem* condition;
		Visualization::ModelItem* initStep;
		Visualization::ModelItem* updateStep;
		Visualization::ModelItem* body;
};

}

#endif /* VLOOPSTATEMENT_H_ */
