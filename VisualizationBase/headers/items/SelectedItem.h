/***********************************************************************************************************************
 * SelectedItem.h
 *
 *  Created on: Jan 20, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SELECTEDITEM_H_
#define SELECTEDITEM_H_

#include "../visualizationbase_api.h"

#include "Item.h"
#include "SelectedItemStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API SelectedItem: public Item
{
	ITEM_COMMON(SelectedItem)

	public:
		SelectedItem(Item* selectedItem, const StyleType* style = itemStyles().get());
		virtual ~SelectedItem();

		virtual bool needsUpdate();
		Item* selectedItem();

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Item* selectedItem_;
};

inline Item* SelectedItem::selectedItem() { return selectedItem_; }

}

#endif /* SELECTEDITEM_H_ */
