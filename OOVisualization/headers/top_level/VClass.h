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

#include "OOModel/headers/top_level/Class.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class VText;
	class VList;
	class PanelBorderLayout;
	class PositionLayout;
}

namespace OOVisualization {

class ClassIcon;

class OOVISUALIZATION_API VClass : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::Class>
{
	ITEM_COMMON(VClass)

	public:
		VClass(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
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
