/***********************************************************************************************************************
 * VStorageSpecifier.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VSTORAGESPECIFIER_H_
#define VSTORAGESPECIFIER_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextRenderer.h"
#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/TextStyle.h"

#include "OOModel/headers/elements/StorageSpecifier.h"

namespace OOVisualization {

class OOVISUALIZATION_API VStorageSpecifier : public Visualization::ItemWithNode< Visualization::TextRenderer, OOModel::StorageSpecifier>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VStorageSpecifier, Visualization::TextStyle)

	public:
		VStorageSpecifier(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual bool setText(const QString& newText);

	protected:
		virtual QString currentText();
};

}

#endif /* VSTORAGESPECIFIER_H_ */
