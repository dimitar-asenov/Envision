/***********************************************************************************************************************
 * Layout.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/Layout.h"
#include "ModelRenderer.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( Layout, "layout" )

Layout::Layout(Item* parent, const StyleType* style) :
	Item(parent, style)
{
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	//setFlag(QGraphicsItem::ItemIsFocusable, false);
	setAcceptedMouseButtons(0);
}

void Layout::setInnerSize(int width_, int height_)
{
	if (isEmpty() && !style()->drawShapeWhenEmpty()) setSize(0,0);
	else
	{
		if ( hasShape() )
		{
			getShape()->setOffset(style()->leftMargin(), style()->topMargin());
			getShape()->setInnerSize(width_, height_);
			setWidth(width() + style()->rightMargin());
			setHeight(height() + style()->bottomMargin());
		}
		else
		{
			setWidth(width_ + style()->leftMargin() + style()->rightMargin());
			setHeight(height_ + style()->topMargin() + style()->bottomMargin());
		}
	}
}

void Layout::determineChildren()
{
}

int Layout::xOffset() const
{
	if ( hasShape() ) return getShape()->contentLeft();
	else return style()->leftMargin();
}

int Layout::yOffset() const
{
	if ( hasShape() ) return getShape()->contentTop();
	else return style()->topMargin();
}

void Layout::synchronizeItem(Item*& layoutItem, Item*& externalItem, Model::Node* node)
{
	// When refactoring this method have in mind that layoutItem might point to the same item as externalItem.


	if (externalItem != layoutItem)
	{
		SAFE_DELETE_ITEM(layoutItem);
		setUpdateNeeded();
	}

	if (externalItem && externalItem->node() != node )
	{
		SAFE_DELETE_ITEM(externalItem);
		layoutItem = NULL; // One of the safe deletes above deleted this item
		setUpdateNeeded();
	}

	if (!externalItem && node)
	{
		externalItem = renderer()->render(NULL, node);
		externalItem->setParentItem(this);
		layoutItem = externalItem;
		setUpdateNeeded();
	}
}

}
