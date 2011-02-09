/***********************************************************************************************************************
 * VModule.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMODULE_H_
#define VMODULE_H_

#include "../../oovisualization_api.h"
#include "VModuleStyle.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
	class PanelBorderLayout;
	class PositionLayout;
	class SequentialLayout;
}

namespace OOModel {
	class Module;
}

namespace OOVisualization {

class OOVISUALIZATION_API VModule : public Visualization::ModelItem
{
	ITEM_COMMON(VModule, Visualization::ModelItem)

	public:
		VModule(Item* parent, OOModel::Module* node, const VModuleStyle* style = Visualization::Styles::item<VModule>("default"));
		virtual ~VModule();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::PanelBorderLayout* layout;
		Visualization::SequentialLayout* header;
		Visualization::VText* name;
		Visualization::PositionLayout* content;
};

}

#endif /* VMODULE_H_ */
