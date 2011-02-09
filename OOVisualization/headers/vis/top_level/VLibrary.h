/***********************************************************************************************************************
 * VLibrary.h
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLIBRARY_H_
#define VLIBRARY_H_

#include "../../oovisualization_api.h"
#include "VLibraryStyle.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
	class PanelBorderLayout;
	class PositionLayout;
	class SequentialLayout;
}

namespace OOModel {
	class Library;
}

namespace OOVisualization {

class OOVISUALIZATION_API VLibrary : public Visualization::ModelItem
{
	ITEM_COMMON(VLibrary, Visualization::ModelItem)

	public:
		VLibrary(Item* parent, OOModel::Library* node, const VLibraryStyle* style = Visualization::Styles::item<VLibrary>("default"));
		virtual ~VLibrary();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::PanelBorderLayout* layout;
		Visualization::SequentialLayout* header;
		Visualization::VText* name;
		Visualization::PositionLayout* content;
};
}

#endif /* VLIBRARY_H_ */
