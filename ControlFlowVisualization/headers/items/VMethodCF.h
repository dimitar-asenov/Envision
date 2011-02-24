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

#include "OOVisualization/headers/icons/MethodIcon.h"

#include "OOModel/headers/top_level/Method.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
	class VList;
	class PanelBorderLayout;
}

namespace ControlFlowVisualization {

class OOVISUALIZATION_API VMethodCF : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::Method>
{
	ITEM_COMMON(VMethodCF)

	public:
		VMethodCF(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VMethodCF>("default"));
		virtual ~VMethodCF();

	protected:
		void determineChildren();

	private:
		Visualization::SequentialLayout* header_;
		OOVisualization::MethodIcon* icon_;
		Visualization::VText* name_;
		Visualization::VList* arguments_;
		Visualization::VList* content_;
		Visualization::VList* results_;
};

}

#endif /* VMETHODCF_H_ */
