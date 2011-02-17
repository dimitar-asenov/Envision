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
#include "../Styles.h"
#include "../shapes/Shape.h"

namespace Visualization {

class VISUALIZATIONBASE_API Layout: public Item
{
	ITEM_COMMON(Layout)

	protected:
		int xOffset() const;
		int yOffset() const;

	public:
		Layout(Item* parent, const LayoutStyle* style);

		void setInnerSize(int width, int height);

		virtual void determineChildren();

		virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
};

}

#endif /* LAYOUT_H_ */
