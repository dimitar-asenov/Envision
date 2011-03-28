/***********************************************************************************************************************
 * SumMethodVis.h
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SUMMETHODVIS_H_
#define SUMMETHODVIS_H_

#include "../custommethodcall_api.h"
#include "SumMethodVisStyle.h"

#include "OOModel/headers/expressions/MethodCallExpression.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Symbol;
}

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API SumMethodVis : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::MethodCallExpression>
{
	ITEM_COMMON(SumMethodVis)

	public:
		SumMethodVis(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~SumMethodVis();

	protected:
		void determineChildren();

	private:
		Visualization::Symbol* name_;
		Visualization::Item* prefix_;
		Visualization::Item* from_;
		Visualization::Item* to_;
		Visualization::SequentialLayout* arguments_;
};

}

#endif /* SUMMETHODVIS_H_ */
