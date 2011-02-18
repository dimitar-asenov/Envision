/***********************************************************************************************************************
 * VMethodCallStatement.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMETHODCALLSTATEMENT_H_
#define VMETHODCALLSTATEMENT_H_

#include "../oovisualization_api.h"
#include "VMethodCallStatementStyle.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class Text;
	class Symbol;
	class SequentialLayout;
	class VList;
}

namespace OOModel {
	class MethodCallStatement;
}

namespace OOVisualization {

class OOVISUALIZATION_API VMethodCallStatement : public Visualization::ModelItem
{
	ITEM_COMMON(VMethodCallStatement)

	public:
		VMethodCallStatement(Item* parent, OOModel::MethodCallStatement* node, const VMethodCallStatementStyle* style = Visualization::Styles::item<VMethodCallStatement>("default"));
		virtual ~VMethodCallStatement();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::Text* name_;
		Visualization::Symbol* separator_;
		Visualization::ModelItem* prefix_;
		Visualization::VList* arguments_;
};

}

#endif /* VMETHODCALLSTATEMENT_H_ */
