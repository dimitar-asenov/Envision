/***********************************************************************************************************************
 * Item.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ITEM_H_
#define ITEM_H_

#include "visualizationbase_api.h"

#include <QtGui/QGraphicsItem>

namespace Visualization {

class VISUALIZATIONBASE_API Item : public QGraphicsItem
{
	protected:
		QRectF bounding_rect;
		QRectF logical_rect;
		QSize sizeLimit;

	public:
		Item(Item* parent);

		QRectF boundingRect() const;
		QRectF logicalRect() const;

		const QSize& getSizeLimit() const;

		virtual void setSizeLimit(int limitWidth, int limitHeight);
		virtual void setSizeWidthLimit(int limitWidth);
		virtual void setSizeHeightLimit(int limitHeight);
};

}

#endif /* ITEM_H_ */
