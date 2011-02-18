/***********************************************************************************************************************
 * SingleLayoutBase.cpp
 *
 *  Created on: Feb 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/SingleLayoutBase.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(SingleLayoutBase)

SingleLayoutBase::SingleLayoutBase(Item* parent, const StyleType *style, Layout* layout) :
	Item(parent, style), layout_(layout)
{
}

SingleLayoutBase::~SingleLayoutBase()
{
	SAFE_DELETE_ITEM(layout_);
}

void SingleLayoutBase::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(layout_, availableWidth, availableHeight);
}

bool SingleLayoutBase::focusChild(FocusTarget location)
{
	return layout_->focusChild(location);
}

bool SingleLayoutBase::sizeDependsOnParent() const
{
	return layout_->sizeDependsOnParent();
}

bool SingleLayoutBase::isEmpty() const
{
	return layout_->isEmpty();
}

}
