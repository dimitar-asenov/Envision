/***********************************************************************************************************************
 * VForEachStatement.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFOREACHSTATEMENT_H_
#define VFOREACHSTATEMENT_H_

#include "../oovisualization_api.h"
#include "VForEachStatementStyle.h"

#include "OOModel/headers/statements/ForEachStatement.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class VText;
	class SequentialLayout;
	class PanelBorderLayout;
	class VList;
}

namespace OOVisualization {

class OOVISUALIZATION_API VForEachStatement : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::ForEachStatement>
{
	ITEM_COMMON(VForEachStatement)

	public:
		VForEachStatement(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VForEachStatement();

	protected:
		void determineChildren();

	private:
		Visualization::SequentialLayout* header;

		Visualization::SequentialLayout* varContainer;
		Visualization::SequentialLayout* collectionBackground;

		Visualization::VText* varName;
		Visualization::Item* collection;
		Visualization::Item* varType;
		Visualization::VList* body;
};

}

#endif /* VFOREACHSTATEMENT_H_ */
