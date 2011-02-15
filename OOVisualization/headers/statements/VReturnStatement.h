/***********************************************************************************************************************
 * VReturnStatement.h
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VRETURNSTATEMENT_H_
#define VRETURNSTATEMENT_H_

#include "../oovisualization_api.h"
#include "VReturnStatementStyle.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Symbol;
	class VList;
	class SequentialLayout;
}

namespace OOModel {
	class ReturnStatement;
}

namespace OOVisualization {

class OOVISUALIZATION_API VReturnStatement : public Visualization::ModelItem
{
	ITEM_COMMON(VReturnStatement, Visualization::ModelItem)

	public:
		VReturnStatement(Item* parent, OOModel::ReturnStatement* node, const VReturnStatementStyle* style = Visualization::Styles::item<VReturnStatement>("default"));
		virtual ~VReturnStatement();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::Symbol* symbol_;
		Visualization::VList* values_;
};

}

#endif /* VRETURNSTATEMENT_H_ */
