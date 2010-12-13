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
#include "shapes/BoxStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API Box: public Shape
{
	protected:
		qreal contentBoxWidth;
		qreal contentBoxHeight;

		QPainterPath getRectanglePath(qreal x, qreal y, int width, int height);

	public:
		Box(Item *parent, BoxStyle *style = BoxStyle::getDefault());

		BoxStyle* style() const;
		virtual void setStyle(ShapeStyle *style);

		virtual void update();
		virtual QPoint contentPosition();
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

}

#endif /* BOX_H_ */
