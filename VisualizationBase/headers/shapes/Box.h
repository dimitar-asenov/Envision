/***********************************************************************************************************************
 * Box.h
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BOX_H_
#define BOX_H_

#include "visualizationbase_api.h"

#include "Shape.h"

namespace Visualization {

class VISUALIZATIONBASE_API Box: public Shape
{
	public:
		Box(Item* parent);
		virtual void update();
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

}

#endif /* BOX_H_ */
