/***********************************************************************************************************************
 * VLibrary.h
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLIBRARY_H_
#define VLIBRARY_H_

#include "../oovisualization_api.h"
#include "VLibraryStyle.h"

#include "OOModel/headers/top_level/Library.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
	class PanelBorderLayout;
	class PositionLayout;
}

namespace OOVisualization {

class OOVISUALIZATION_API VLibrary : public Visualization::ItemWithNode< Visualization::LayoutProvider<Visualization::PanelBorderLayout>, OOModel::Library>
{
	ITEM_COMMON(VLibrary)

	public:
		VLibrary(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VLibrary>("default"));
		virtual ~VLibrary();

	protected:
		void determineChildren();

	private:
		Visualization::SequentialLayout* header;
		Visualization::VText* name;
		Visualization::PositionLayout* content;
};
}

#endif /* VLIBRARY_H_ */
