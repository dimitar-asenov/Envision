/***********************************************************************************************************************
 * VMethodCF.h
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMETHODCF_H_
#define VMETHODCF_H_

#include "../controlflowvisualization_api.h"
#include "VMethodCFStyle.h"
#include "VListCF.h"

#include "OOModel/headers/top_level/Method.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class VText;
	class VList;
	class PanelBorderLayout;
	class Static;
}

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VMethodCF : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::Method>
{
	ITEM_COMMON(VMethodCF)

	public:
		VMethodCF(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VMethodCF();

	protected:
		void determineChildren();

	private:
		Visualization::SequentialLayout* header_;
		Visualization::Static* icon_;
		Visualization::VText* name_;
		Visualization::VList* arguments_;
		Visualization::VList* content_;
		VListCF* contentCF_;
		Visualization::VList* results_;
};

}

#endif /* VMETHODCF_H_ */
