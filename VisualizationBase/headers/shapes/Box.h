/***********************************************************************************************************************
 * Box.h
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BOX_H_
#define BOX_H_

#include "../visualizationbase_api.h"

#include "Shape.h"
#include "BoxStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API Box: public Shape
{
	SHAPE_COMMON(Box)

	protected:
		qreal contentBoxWidth;
		qreal contentBoxHeight;

		QPainterPath getRectanglePath(qreal x, qreal y, int width, int height);

	public:
		Box(Item *parent, StyleType *style = itemStyles().get());

		virtual void update();
		virtual int contentLeft();
		virtual int contentTop();

		virtual QSize innerSize(QSize outterSize) const;
		virtual QSize outterSize(QSize innerSize) const;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

}

#endif /* BOX_H_ */
