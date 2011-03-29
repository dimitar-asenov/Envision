/***********************************************************************************************************************
 * VStorageSpecifier.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/VStorageSpecifier.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VStorageSpecifier, "item")

VStorageSpecifier::VStorageSpecifier(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<TextRenderer, StorageSpecifier>(parent, node, style)
{
}

bool VStorageSpecifier::setText(const QString&)
{
	return false;
}

QString VStorageSpecifier::currentText()
{
	switch (node()->get())
	{
		case OOModel::StorageSpecifier::CLASS_VARIABLE:
			return "static";
			break;
		case OOModel::StorageSpecifier::INSTANCE_VARIABLE:
			return "not static";
			break;
		default:
			return "unknown storage specifier";
			break;
	}
}

}
