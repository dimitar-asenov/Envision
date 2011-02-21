/***********************************************************************************************************************
 * VStatic.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/VStatic.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VStatic)

VStatic::VStatic(Item* parent, NodeType* stat, const StyleType* style) :
	ItemWithNode<TextRenderer, Static>(parent, stat, style)
{
}

bool VStatic::setText(const QString&)
{
	return false;
}

QString VStatic::currentText()
{
	switch (node()->get())
	{
		case OOModel::Static::CLASS_VARIABLE:
			return "static";
			break;
		case OOModel::Static::INSTANCE_VARIABLE:
			return "not static";
			break;
		default:
			return "unknown static";
			break;
	}
}

}
