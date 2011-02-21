/***********************************************************************************************************************
 * VVisibility.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/VVisibility.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VVisibility)

VVisibility::VVisibility(Item* parent, NodeType* vis, const StyleType* style) :
	ItemWithNode<TextRenderer, Visibility>(parent, vis, style)
{
}

bool VVisibility::setText(const QString&)
{
	return false;
}

QString VVisibility::currentText()
{
	switch (node()->get())
	{
		case OOModel::Visibility::DEFAULT:
			return "";
			break;
		case OOModel::Visibility::PUBLIC:
			return "public";
			break;
		case OOModel::Visibility::PRIVATE:
			return "private";
			break;
		case OOModel::Visibility::PROTECTED:
			return "protected";
			break;
		default:
			return "unknown visibility";
			break;
	}
}

}
