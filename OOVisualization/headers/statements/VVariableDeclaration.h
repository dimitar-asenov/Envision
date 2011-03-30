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

#include "OOModel/headers/statements/VariableDeclaration.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Static;
	class VText;
}

namespace OOVisualization {

class OOVISUALIZATION_API VVariableDeclaration : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::VariableDeclaration>
{
	ITEM_COMMON(VVariableDeclaration)

	public:
		VVariableDeclaration(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VVariableDeclaration();

	protected:
		void determineChildren();

	private:
		Visualization::VText* name_;
		Visualization::Item* type_;
		Visualization::Static* assignmentSymbol_;
		Visualization::Item* initialValue_;
};

}

#endif /* VVARIABLEDECLARATION_H_ */
