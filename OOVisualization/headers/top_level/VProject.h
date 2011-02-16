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

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
	class PanelBorderLayout;
	class PositionLayout;
	class SequentialLayout;
}

namespace OOModel {
	class Project;
}

namespace OOVisualization {

class OOVISUALIZATION_API VProject : public Visualization::ModelItem
{
	ITEM_COMMON(VProject, Visualization::ModelItem)

	public:
		VProject(Item* parent, OOModel::Project* node, const VProjectStyle* style = Visualization::Styles::item<VProject>("default"));
		virtual ~VProject();

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

#endif /* VPROJECT_H_ */
