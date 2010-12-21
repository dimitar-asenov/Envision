/***********************************************************************************************************************
 * Layout.h
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "visualizationbase_api.h"

#include "Item.h"
#include "LayoutStyle.h"
#include "shapes/Shape.h"

namespace Visualization {

class VISUALIZATIONBASE_API Layout: public Item
{
	ITEM_COMMON(Layout, Item)

	protected:
		int xOffset() const;
		int yOffset() const;

	public:
		Layout(Item* parent, const LayoutStyle* style);

		void setInnerSize(int width, int height);

		virtual void determineChildren();
		virtual bool needsUpdate();

		virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
};

}

#endif /* LAYOUT_H_ */
