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

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Symbol;
	class VText;
}

namespace OOModel {
	class VariableDeclaration;
}

namespace OOVisualization {

class OOVISUALIZATION_API VVariableDeclaration : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::VariableDeclaration>
{
	ITEM_COMMON(VVariableDeclaration)

	public:
		VVariableDeclaration(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VVariableDeclaration>("default"));
		virtual ~VVariableDeclaration();

	protected:
		void determineChildren();

	private:
		Visualization::VText* name_;
		Visualization::Item* type_;
		Visualization::Symbol* assignmentSymbol_;
		Visualization::Item* initialValue_;
};

}

#endif /* VVARIABLEDECLARATION_H_ */
