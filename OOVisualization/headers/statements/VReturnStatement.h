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

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Symbol;
	class VList;
}

namespace OOModel {
	class ReturnStatement;
}

namespace OOVisualization {

class OOVISUALIZATION_API VReturnStatement : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::ReturnStatement>
{
	ITEM_COMMON(VReturnStatement)

	public:
		VReturnStatement(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VReturnStatement>("default"));
		virtual ~VReturnStatement();

	protected:
		void determineChildren();

	private:
		Visualization::Symbol* symbol_;
		Visualization::VList* values_;
};

}

#endif /* VRETURNSTATEMENT_H_ */
