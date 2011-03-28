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

#include "OOModel/headers/statements/AssignmentStatement.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Symbol;
}

namespace OOVisualization {

class OOVISUALIZATION_API VAssignmentStatement : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::AssignmentStatement>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VAssignmentStatement, OperatorSequenceStyle)

	public:
		VAssignmentStatement(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
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
