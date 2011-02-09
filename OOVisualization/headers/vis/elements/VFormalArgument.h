/***********************************************************************************************************************
 * VFormalArgument.h
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFORMALARGUMENT_H_
#define VFORMALARGUMENT_H_

#include "../../oovisualization_api.h"
#include "VFormalArgumentStyle.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
	class SequentialLayout;
}

namespace OOModel {
	class FormalArgument;
}

namespace OOVisualization {

class OOVISUALIZATION_API VFormalArgument : public Visualization::ModelItem
{
	ITEM_COMMON(VFormalArgument, Visualization::ModelItem)

	public:
		VFormalArgument(Item* parent, OOModel::FormalArgument* node, const VFormalArgumentStyle* style = Visualization::Styles::item<VFormalArgument>("default"));
		virtual ~VFormalArgument();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::VText* name_;
		Visualization::ModelItem* type_;
};

}

#endif /* VFORMALARGUMENT_H_ */
