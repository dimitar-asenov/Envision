/***********************************************************************************************************************
 * InsertMethodVis.h
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef INSERTMETHODVIS_H_
#define INSERTMETHODVIS_H_

#include "../custommethodcall_api.h"
#include "InsertMethodVisStyle.h"

#include "OOModel/headers/statements/MethodCallStatement.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Static;
	class VList;
}

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API InsertMethodVis : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::MethodCallStatement>
{
	ITEM_COMMON(InsertMethodVis)

	public:
		InsertMethodVis(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~InsertMethodVis();

	protected:
		void determineChildren();

	private:
		Visualization::Static* icon_;
		Visualization::Static* separator_;
		Visualization::Item* prefix_;
		Visualization::VList* arguments_;
};

}

#endif /* INSERTMETHODVIS_H_ */
