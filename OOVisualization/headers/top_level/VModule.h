/***********************************************************************************************************************
 * VModule.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMODULE_H_
#define VMODULE_H_

#include "../oovisualization_api.h"
#include "VModuleStyle.h"

#include "OOModel/headers/top_level/Module.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
	class PanelBorderLayout;
	class PositionLayout;
}

namespace OOVisualization {

class OOVISUALIZATION_API VModule : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::Module>
{
	ITEM_COMMON(VModule)

	public:
		VModule(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VModule>("default"));
		virtual ~VModule();

	protected:
		void determineChildren();

	private:
		Visualization::SequentialLayout* header;
		Visualization::VText* name;
		Visualization::PositionLayout* content;
};

}

#endif /* VMODULE_H_ */
