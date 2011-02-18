/***********************************************************************************************************************
 * VFormalArgument.h
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFORMALARGUMENT_H_
#define VFORMALARGUMENT_H_

#include "../oovisualization_api.h"
#include "VFormalArgumentStyle.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
}

namespace OOModel {
	class FormalArgument;
}

namespace OOVisualization {

class OOVISUALIZATION_API VFormalArgument : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::FormalArgument>
{
	ITEM_COMMON(VFormalArgument)

	public:
		VFormalArgument(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VFormalArgument>("default"));
		virtual ~VFormalArgument();

	protected:
		void determineChildren();

	private:
		Visualization::VText* name_;
		Visualization::Item* type_;
};

}

#endif /* VFORMALARGUMENT_H_ */
