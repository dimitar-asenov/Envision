/***********************************************************************************************************************
 * VField.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFIELD_H_
#define VFIELD_H_

#include "../../oovisualization_api.h"
#include "VFieldStyle.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
	class SequentialLayout;
}

namespace OOModel {
	class Field;
}

namespace OOVisualization {

class VVisibility;

class OOVISUALIZATION_API VField : public Visualization::ModelItem
{
	ITEM_COMMON(VField, Visualization::ModelItem)

	public:
		VField(Item* parent, OOModel::Field* node, const VFieldStyle* style = Visualization::Styles::item<VField>("default"));
		virtual ~VField();

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

#endif /* VFIELD_H_ */
