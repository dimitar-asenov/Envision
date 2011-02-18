/***********************************************************************************************************************
 * VVariableAccess.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VVARIABLEACCESS_H_
#define VVARIABLEACCESS_H_

#include "../oovisualization_api.h"
#include "VVariableAccessStyle.h"

#include "OOModel/headers/expressions/VariableAccess.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Text;
	class Symbol;
}

namespace OOVisualization {

class OOVISUALIZATION_API VVariableAccess : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::VariableAccess>
{
	ITEM_COMMON(VVariableAccess)

	public:
		VVariableAccess(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VVariableAccess>("default"));
		virtual ~VVariableAccess();

	protected:
		void determineChildren();

	private:
		Visualization::Text* name_;
		Visualization::Symbol* separator_;
		Visualization::Item* prefix_;
};

}


#endif /* VVARIABLEACCESS_H_ */
