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
	private:
		QSize sizeLimit;
		bool sizeLimitChanged_;

		bool sizeLimitChanged();
		void updateChildren();

	protected:
		QRectF bounding_rect;
		QSize size;


	public:
		Item(Item* parent);

		QRectF boundingRect() const;
		int width() const;
		int height() const;

		const QSize& getSizeLimit() const;

		void setSizeLimit(int limitWidth, int limitHeight);
		void setSizeWidthLimit(int limitWidth);
		void setSizeHeightLimit(int limitHeight);

		virtual bool needsUpdate();
		virtual void updateSubtreeState();
		virtual void determineChildren() = 0;
		virtual void updateState() = 0;


};

}

#endif /* ITEM_H_ */
