/***********************************************************************************************************************
 * VContinueStatement.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VCONTINUESTATEMENT_H_
#define VCONTINUESTATEMENT_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/Symbol.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/statements/ContinueStatement.h"

namespace OOVisualization {

class OOVISUALIZATION_API VContinueStatement : public Visualization::ItemWithNode< Visualization::Item, OOModel::ContinueStatement>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VContinueStatement, Visualization::SymbolStyle)

	public:
		VContinueStatement(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VContinueStatement>("default"));
		virtual ~VContinueStatement();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::Symbol* vis_;
};

}

#endif /* VCONTINUESTATEMENT_H_ */
