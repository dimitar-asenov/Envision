/***********************************************************************************************************************
 * LayoutProviderBase.cpp
 *
 *  Created on: Feb 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/LayoutProviderBase.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(LayoutProviderBase)

LayoutProviderBase::LayoutProviderBase(Item* parent, const StyleType *style, Layout* layout) :
	Item(parent, style), layout_(layout)
{
}

LayoutProviderBase::~LayoutProviderBase()
{
	SAFE_DELETE_ITEM(layout_);
}

void LayoutProviderBase::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(layout_, availableWidth, availableHeight);
}

bool LayoutProviderBase::focusChild(FocusTarget location)
{
	return layout_->focusChild(location);
}

bool LayoutProviderBase::sizeDependsOnParent() const
{
	return layout_->sizeDependsOnParent();
}

bool LayoutProviderBase::isEmpty() const
{
	return layout_->isEmpty();
}

}
