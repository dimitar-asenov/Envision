/***********************************************************************************************************************
 * VPrimitiveType.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "types/VPrimitiveType.h"
#include "OOVisualizationException.h"

#include "OOModel/headers/types/PrimitiveType.h"

#include "VisualizationBase/headers/items/Text.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VPrimitiveType)

VPrimitiveType::VPrimitiveType(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode< Item, PrimitiveType>(parent, node, style),
	vis_( new Text(this, style))
{
}

VPrimitiveType::~VPrimitiveType()
{
	SAFE_DELETE_ITEM(vis_);
}

void VPrimitiveType::determineChildren()
{
	vis_->setStyle(style());

	switch( node()->type() )
	{
		case PrimitiveType::INT:
			vis_->setText("int");
			break;
		case PrimitiveType::LONG:
			vis_->setText("long");
			break;
		case PrimitiveType::UNSIGNED_INT:
			vis_->setText("int+");
			break;
		case PrimitiveType::UNSIGNED_LONG:
			vis_->setText("long+");
			break;
		case PrimitiveType::FLOAT:
			vis_->setText("float");
			break;
		case PrimitiveType::DOUBLE:
			vis_->setText("double");
			break;
		case PrimitiveType::BOOLEAN:
			vis_->setText("bool");
			break;
		case PrimitiveType::CHAR:
			vis_->setText("char");
			break;
		case PrimitiveType::VOID:
			vis_->setText("void");
			break;
		default:
			throw OOVisualizationException("Unknown primitive type in VPrimitiveType::determineChildren()");
	}
}

void VPrimitiveType::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(vis_, availableWidth, availableHeight);
}

bool VPrimitiveType::focusChild(FocusTarget location)
{
	return vis_->focusChild(location);
}

}
