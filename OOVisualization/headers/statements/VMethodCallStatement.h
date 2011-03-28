/***********************************************************************************************************************
 * VMethodCallStatement.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMETHODCALLSTATEMENT_H_
#define VMETHODCALLSTATEMENT_H_

#include "../oovisualization_api.h"
#include "VMethodCallStatementStyle.h"

#include "OOModel/headers/statements/MethodCallStatement.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Text;
	class Symbol;
	class VList;
}

namespace OOVisualization {

class OOVISUALIZATION_API VMethodCallStatement : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::MethodCallStatement>
{
	ITEM_COMMON(VMethodCallStatement)

	public:
		VMethodCallStatement(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VMethodCallStatement();

	protected:
		void determineChildren();

	private:
		Visualization::Text* name_;
		Visualization::Symbol* separator_;
		Visualization::Item* prefix_;
		Visualization::VList* arguments_;
};

}

#endif /* VMETHODCALLSTATEMENT_H_ */
