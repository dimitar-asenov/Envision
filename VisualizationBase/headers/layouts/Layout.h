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
#include "shapes/Shape.h"

namespace Visualization {

class Layout: public Item
{
	protected:
		int xOffset();
		int yOffset();

	public:
		Layout(Item* parent);

		void setInnerSize(int width, int height);

		virtual void determineChildren();
		virtual bool needsUpdate();

		virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
};

inline int Layout::xOffset() { if (getShape()) return getShape()->contentLeft(); else return 0; }
inline int Layout::yOffset() { if (getShape()) return getShape()->contentTop(); else return 0; }
}

#endif /* LAYOUT_H_ */
