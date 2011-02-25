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

#include "OOModel/headers/statements/LoopStatement.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class PanelBorderLayout;
	class SequentialLayout;
	class VList;
}

namespace OOVisualization {

class OOVISUALIZATION_API VLoopStatement : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::LoopStatement>
{
	ITEM_COMMON(VLoopStatement)

	public:
		VLoopStatement(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VLoopStatement>("default"));
		virtual ~VLoopStatement();

	protected:
		virtual void determineChildren();

	private:
		Visualization::SequentialLayout* header;

		Visualization::SequentialLayout* conditionBackground;
		Visualization::SequentialLayout* initStepBackground;
		Visualization::SequentialLayout* updateStepBackground;

		Visualization::Item* condition;
		Visualization::Item* initStep;
		Visualization::Item* updateStep;
		Visualization::VList* body;
};

}

#endif /* VLOOPSTATEMENT_H_ */
