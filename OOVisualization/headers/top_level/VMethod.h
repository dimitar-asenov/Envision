/***********************************************************************************************************************
 * VMethod.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMETHOD_H_
#define VMETHOD_H_

#include "../oovisualization_api.h"
#include "VMethodStyle.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
	class VList;
	class PanelBorderLayout;
	class SequentialLayout;
}

namespace OOModel {
	class Method;
}

namespace OOVisualization {

class MethodIcon;

class OOVISUALIZATION_API VMethod : public Visualization::ModelItem
{
	ITEM_COMMON(VMethod)

	public:
		VMethod(Item* parent, OOModel::Method* node, const VMethodStyle* style = Visualization::Styles::item<VMethod>("default"));
		virtual ~VMethod();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::PanelBorderLayout* layout_;
		Visualization::SequentialLayout* header_;
		MethodIcon* icon_;
		Visualization::VText* name_;
		Visualization::VList* arguments_;
		Visualization::VList* content_;
		Visualization::VList* results_;
};

}

#endif /* VMETHOD_H_ */
