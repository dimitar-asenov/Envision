/***********************************************************************************************************************
 * EmptyMethodVis.h
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef EMPTYMETHODVIS_H_
#define EMPTYMETHODVIS_H_

#include "../custommethodcall_api.h"
#include "EmptyMethodVisStyle.h"

#include "OOModel/headers/expressions/MethodCallExpression.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Symbol;
	class SVGIcon;
}

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API EmptyMethodVis : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::MethodCallExpression>
{
	ITEM_COMMON(EmptyMethodVis)

	public:
		EmptyMethodVis(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<EmptyMethodVis>("default"));
		virtual ~EmptyMethodVis();

	protected:
		void determineChildren();

	private:
		Visualization::SVGIcon* icon_;
		Visualization::Symbol* separator_;
		Visualization::Item* prefix_;
};

}

#endif /* EMPTYMETHODVIS_H_ */
