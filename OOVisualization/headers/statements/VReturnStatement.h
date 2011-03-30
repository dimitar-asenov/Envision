/***********************************************************************************************************************
 * VReturnStatement.h
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VRETURNSTATEMENT_H_
#define VRETURNSTATEMENT_H_

#include "../oovisualization_api.h"
#include "VReturnStatementStyle.h"

#include "OOModel/headers/statements/ReturnStatement.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Static;
	class VList;
}

namespace OOVisualization {

class OOVISUALIZATION_API VReturnStatement : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::ReturnStatement>
{
	ITEM_COMMON(VReturnStatement)

	public:
		VReturnStatement(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VReturnStatement();

	protected:
		void determineChildren();

	private:
		Visualization::Static* symbol_;
		Visualization::VList* values_;
};

}

#endif /* VRETURNSTATEMENT_H_ */
