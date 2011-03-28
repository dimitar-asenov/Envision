/***********************************************************************************************************************
 * VProject.h
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VPROJECT_H_
#define VPROJECT_H_

#include "../oovisualization_api.h"
#include "VProjectStyle.h"

#include "OOModel/headers/top_level/Project.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class VText;
	class PanelBorderLayout;
	class PositionLayout;
	class SequentialLayout;
}

namespace OOVisualization {

class OOVISUALIZATION_API VProject : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::Project>
{
	ITEM_COMMON(VProject)

	public:
		VProject(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VProject();

	protected:
		void determineChildren();

	private:
		Visualization::SequentialLayout* header;
		Visualization::VText* name;
		Visualization::PositionLayout* content;
};

}

#endif /* VPROJECT_H_ */
