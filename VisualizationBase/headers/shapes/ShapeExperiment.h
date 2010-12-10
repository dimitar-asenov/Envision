/***********************************************************************************************************************
 * ShapeExperiment.h
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SHAPEEXPERIMENT_H_
#define SHAPEEXPERIMENT_H_

#include "Item.h"

#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QPainter>
#include <QtCore/QSize>

namespace Visualization {

template <class T>
class ShapeExperiment : public T
{
	private:
		QSize size_;

	public:

		ShapeExperiment(Item* parent) : T(parent)
		{}

		void setWidth(int width)
		{
			size_.setWidth(width);
		}

		void setHeight(int height)
		{
			size_.setHeight(height);
		}

		void paint(QPainter * painter, const QStyleOptionGraphicsItem *, QWidget *)
		{
			painter->drawRect(0, 0, size_.width(), size_.height());
		}
};

}

#endif /* SHAPEEXPERIMENT_H_ */
