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
	private:
		bool needsUpdate_;

	public:
		Layout(Item* parent);

		void setNeedsUpdate();
		bool needsUpdate();
		void determineChildren();

		void updateSubtreeState();

		void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
};

}

#endif /* LAYOUT_H_ */
