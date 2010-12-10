/***********************************************************************************************************************
 * Box.h
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BOX_H_
#define BOX_H_

#include "Item.h"
#include "layouts/SequentialLayout.h"

namespace Visualization {

class Box: public Item
{
	private:
		SequentialLayout items;

	public:
		Box(Item* parent, int sub);

		void determineChildren();
		void updateState();
		void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
};

}

#endif /* BOX_H_ */
