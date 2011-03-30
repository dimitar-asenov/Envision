/***********************************************************************************************************************
 * Static.cpp
 *
 *  Created on: Mar 30, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/Static.h"
#include "VisualizationException.h"
#include "shapes/Shape.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(Static, "item")

QMap<QString, Static::staticItemConstructorType> Static::itemClasses_;
QMap<QString, Static::staticItemStyleConstructorType> Static::itemStyles_;

Static::Static(Item* parent, const StyleType *style) :
	Item(parent, style), item_(NULL)
{
}

Static::~Static()
{
	SAFE_DELETE_ITEM(item_);
}

bool Static::isEmpty() const
{
	return style()->isEmpty();
}

void Static::updateGeometry(int availableWidth, int availableHeight)
{
	if (item_) Item::updateGeometry(item_, availableWidth, availableHeight);
	else
	{
		if (hasShape())
		{
			getShape()->setOffset(0, 0);
			getShape()->setInnerSize(0, 0);
		}
		else setSize(QSize(0,0));
	}
}

bool Static::focusChild(FocusTarget location)
{
	if (item_) return item_->focusChild(location);
	else return false;
}

void Static::determineChildren()
{
	if (style()->isEmpty()) SAFE_DELETE_ITEM(item_);
	else
	{
		if (item_ && item_->style() != &style()->itemStyle())
		{
			SAFE_DELETE_ITEM(item_);
		}

		if (!item_)
		{
			if (itemClasses_.contains(style()->itemClass()))
				item_ = itemClasses_.value(style()->itemClass())(this, &style()->itemStyle());
			else throw VisualizationException("Invalid item class specified for a Static item");
		}

		item_->setStyle( &style()->itemStyle());
	}
}

ItemStyle* Static::constructStyle(const QString& itemClass)
{
	if (itemStyles_.contains(itemClass))
		return itemStyles_.value(itemClass)();
	else return NULL;
}

}
