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

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class PanelBorderLayout;
	class VText;
	class SequentialLayout;
}

namespace OOModel {
	class ForEachStatement;
}

namespace OOVisualization {

class OOVISUALIZATION_API VForEachStatement : public Visualization::ModelItem
{
	ITEM_COMMON(VForEachStatement)

	public:
		VForEachStatement(Item* parent, OOModel::ForEachStatement* node, const VForEachStatementStyle* style = Visualization::Styles::item<VForEachStatement>("default"));
		virtual ~VForEachStatement();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::PanelBorderLayout* layout;
		Visualization::SequentialLayout* header;

		Visualization::SequentialLayout* varContainer;
		Visualization::SequentialLayout* collectionBackground;

		Visualization::VText* varName;
		Visualization::ModelItem* collection;
		Visualization::ModelItem* varType;
		Visualization::ModelItem* body;
};

}

#endif /* VFOREACHSTATEMENT_H_ */
