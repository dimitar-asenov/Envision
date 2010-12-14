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

namespace Visualization {

class Layout: public Item
{
	public:
		Layout(Item* parent);

		virtual void determineChildren();
		virtual bool needsUpdate();

		virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
};

}

#endif /* LAYOUT_H_ */
