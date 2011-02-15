/***********************************************************************************************************************
 * VBlock.h
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VBLOCK_H_
#define VBLOCK_H_

#include "../oovisualization_api.h"
#include "VBlockStyle.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VList;
}

namespace OOModel {
	class Block;
}

namespace OOVisualization {

class OOVISUALIZATION_API VBlock : public Visualization::ModelItem
{
	ITEM_COMMON(VBlock, Visualization::ModelItem)

	public:
		VBlock(Item* parent, OOModel::Block* node, const VBlockStyle* style = Visualization::Styles::item<VBlock>("default"));
		virtual ~VBlock();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::VList* items_;
};

}

#endif /* VBLOCK_H_ */
