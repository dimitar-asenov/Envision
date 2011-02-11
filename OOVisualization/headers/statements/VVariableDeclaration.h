/***********************************************************************************************************************
 * VVariableDeclaration.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VVARIABLEDECLARATION_H_
#define VVARIABLEDECLARATION_H_

#include "../oovisualization_api.h"
#include "VVariableDeclarationStyle.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Text;
	class VText;
	class SequentialLayout;
}

namespace OOModel {
	class VariableDeclaration;
}

namespace OOVisualization {

class OOVISUALIZATION_API VVariableDeclaration : public Visualization::ModelItem
{
	ITEM_COMMON(VVariableDeclaration, Visualization::ModelItem)

	public:
		VVariableDeclaration(Item* parent, OOModel::VariableDeclaration* node, const VVariableDeclarationStyle* style = Visualization::Styles::item<VVariableDeclaration>("default"));
		virtual ~VVariableDeclaration();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::VText* name_;
		Visualization::ModelItem* type_;
		Visualization::Text* assignmentSymbol_;
		Visualization::ModelItem* initialValue_;
};

}

#endif /* VVARIABLEDECLARATION_H_ */
