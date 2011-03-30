/***********************************************************************************************************************
 * FindMethodVis.h
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef FINDMETHODVIS_H_
#define FINDMETHODVIS_H_

#include "../custommethodcall_api.h"
#include "FindMethodVisStyle.h"

#include "OOModel/headers/expressions/MethodCallExpression.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Text;
	class Static;
	class VList;
}

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API FindMethodVis : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::MethodCallExpression>
{
	ITEM_COMMON(FindMethodVis)

	public:
		FindMethodVis(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~FindMethodVis();

	protected:
		void determineChildren();

	private:
		Visualization::Text* name_;
		Visualization::Static* separator_;
		Visualization::Item* prefix_;
		Visualization::VList* arguments_;
};

}

#endif /* FINDMETHODVIS_H_ */
