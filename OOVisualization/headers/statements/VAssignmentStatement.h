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

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Symbol;
}

namespace OOModel {
	class AssignmentStatement;
}

namespace OOVisualization {

class OOVISUALIZATION_API VAssignmentStatement : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::AssignmentStatement>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VAssignmentStatement, OperatorSequenceStyle)

	public:
		VAssignmentStatement(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VAssignmentStatement>("default"));
		virtual ~VAssignmentStatement();

	protected:
		void determineChildren();

	private:
		Visualization::Symbol* assignmentSymbol_;
		Visualization::Item* left_;
		Visualization::Item* right_;
};

}

#endif /* VASSIGNMENTSTATEMENT_H_ */
