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

#include "OOModel/headers/statements/MethodCallStatement.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Text;
	class Symbol;
	class VList;
}

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API FindMethodVis : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::MethodCallStatement>
{
	ITEM_COMMON(FindMethodVis)

	public:
		FindMethodVis(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<FindMethodVis>("default"));
		virtual ~FindMethodVis();

	protected:
		void determineChildren();

	private:
		Visualization::Text* name_;
		Visualization::Symbol* separator_;
		Visualization::Item* prefix_;
		Visualization::VList* arguments_;
};

}

#endif /* FINDMETHODVIS_H_ */
