/***********************************************************************************************************************
 * VVariableAccess.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VVARIABLEACCESS_H_
#define VVARIABLEACCESS_H_

#include "../oovisualization_api.h"
#include "VVariableAccessStyle.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Text;
	class Symbol;
	class SequentialLayout;
}

namespace OOModel {
	class VariableAccess;
}

namespace OOVisualization {

class OOVISUALIZATION_API VVariableAccess : public Visualization::ModelItem
{
	ITEM_COMMON(VVariableAccess)

	public:
		VVariableAccess(Item* parent, OOModel::VariableAccess* node, const VVariableAccessStyle* style = Visualization::Styles::item<VVariableAccess>("default"));
		virtual ~VVariableAccess();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::Text* name_;
		Visualization::Symbol* separator_;
		Visualization::ModelItem* prefix_;
};

}


#endif /* VVARIABLEACCESS_H_ */
