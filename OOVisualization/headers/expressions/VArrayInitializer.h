/***********************************************************************************************************************
 * VArrayInitializer.h
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VARRAYINITIALIZER_H_
#define VARRAYINITIALIZER_H_

#include "../oovisualization_api.h"
#include "VArrayInitializerStyle.h"

#include "OOModel/headers/expressions/ArrayInitializer.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class GridLayout;
	class VList;
}

namespace OOVisualization {

class OOVISUALIZATION_API VArrayInitializer : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::GridLayout>, OOModel::ArrayInitializer>
{
	ITEM_COMMON(VArrayInitializer)

	public:
		VArrayInitializer(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VArrayInitializer>("default"));
		virtual ~VArrayInitializer();

		void showInMatrixForm(bool matrixForm = true);
		bool isShownInMatrixForm() const;

	protected:
		void determineChildren();

	private:
		Visualization::VList* values_;
		bool matrixForm_;

		bool viewFormSwitched() const;
};

inline bool VArrayInitializer::isShownInMatrixForm() const { return matrixForm_; }

}

#endif /* VARRAYINITIALIZER_H_ */
