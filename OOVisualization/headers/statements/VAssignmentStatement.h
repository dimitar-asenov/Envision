/***********************************************************************************************************************
 * VAssignmentStatement.h
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VASSIGNMENTSTATEMENT_H_
#define VASSIGNMENTSTATEMENT_H_

#include "../oovisualization_api.h"
#include "../expressions/OperatorStyle.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Symbol;
	class SequentialLayout;
}

namespace OOModel {
	class AssignmentStatement;
}

namespace OOVisualization {

class OOVISUALIZATION_API VAssignmentStatement : public Visualization::ModelItem
{
	ITEM_COMMON_CUSTOM_STYLENAME(VAssignmentStatement, Visualization::ModelItem, OperatorSequenceStyle)

	public:
		VAssignmentStatement(Item* parent, OOModel::AssignmentStatement* node, const OperatorSequenceStyle* style = Visualization::Styles::item<VAssignmentStatement>("default"));
		virtual ~VAssignmentStatement();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::Symbol* assignmentSymbol_;
		Visualization::ModelItem* left_;
		Visualization::ModelItem* right_;
};

}

#endif /* VASSIGNMENTSTATEMENT_H_ */
