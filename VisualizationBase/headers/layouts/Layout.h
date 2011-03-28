/***********************************************************************************************************************
 * Layout.h
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "../visualizationbase_api.h"

#include "LayoutStyle.h"
#include "../items/Item.h"
#include "../shapes/Shape.h"

namespace Visualization {

class VISUALIZATIONBASE_API Layout: public Item
{
	ITEM_COMMON(Layout)

	public:
		Layout(Item* parent, const StyleType* style);

		void setInnerSize(int width, int height);

		virtual void determineChildren();

	protected:
		int xOffset() const;
		int yOffset() const;

		void synchronizeItem(Item*& layoutItem, Item*& externalItem, Model::Node* node);
		template <class T> void synchronizeItem(Item*& layoutItem, T*& externalItem, bool present, const typename T::StyleType* style);
		template <class T> void synchronizeItem(Item*& layoutItem, T*& externalItem, typename T::NodeType* node, const typename T::StyleType* style);
};

template <class T> void Layout::synchronizeItem(Item*& layoutItem, T*& externalItem, bool present, const typename T::StyleType* style)
{
	// When refactoring this method have in mind that layoutItem might point to the same item as externalItem.


	if (externalItem != layoutItem)
	{
		SAFE_DELETE_ITEM(layoutItem);
		setUpdateNeeded();
	}

	if (externalItem && !present )
	{
		SAFE_DELETE_ITEM(externalItem);
		layoutItem = NULL; // One of the safe deletes above deleted this item
		setUpdateNeeded();
	}

	if (!externalItem && present)
	{
		if (style) externalItem = new T(NULL, style);
		else externalItem = new T(NULL);

		externalItem->setParentItem(this);
		layoutItem = externalItem;
		setUpdateNeeded();
	}
}

template <class T> void Layout::synchronizeItem(Item*& layoutItem, T*& externalItem, typename T::NodeType* node, const typename T::StyleType* style)
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
		if (style) externalItem = new T(NULL, node, style);
		else externalItem = new T(NULL, node);

		externalItem->setParentItem(this);
		layoutItem = externalItem;
		setUpdateNeeded();
	}
}

}

#endif /* LAYOUT_H_ */
