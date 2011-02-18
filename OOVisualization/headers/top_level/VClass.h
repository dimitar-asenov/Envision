/***********************************************************************************************************************
 * VClass.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VCLASS_H_
#define VCLASS_H_

#include "../oovisualization_api.h"
#include "VClassStyle.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
	class VList;
	class PanelBorderLayout;
	class PositionLayout;
}

namespace OOModel {
	class Class;
}

namespace OOVisualization {

class ClassIcon;

class OOVISUALIZATION_API VClass : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::Class>
{
	ITEM_COMMON(VClass)

	public:
		VClass(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VClass>("default"));
		virtual ~VClass();

	protected:
		void determineChildren();

	private:
		Visualization::SequentialLayout* header_;
		ClassIcon* icon_;
		Visualization::VText* name_;
		Visualization::VList* baseClasses_;
		Visualization::PositionLayout* content_;
		Visualization::SequentialLayout* fieldContainer_;
		Visualization::SequentialLayout* publicFieldArea_;
		Visualization::SequentialLayout* privateFieldArea_;
		Visualization::SequentialLayout* protectedFieldArea_;
		Visualization::SequentialLayout* defaultFieldArea_;
};

}

#endif /* VCLASS_H_ */
