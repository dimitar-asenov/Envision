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

class Shape;
class ShapeStyle;

class VISUALIZATIONBASE_API Item : public QGraphicsItem
{
	private:
		friend class Shape;
		QSize sizeLimit;
		bool needsUpdate_;

		Shape* shape_;

		bool sizeLimitChanged();
		void updateChildren();

	protected:
		QRectF bounding_rect;
		QSize size;


	public:
		Item(Item* parent, Shape *shape = NULL);
		virtual ~Item();

		QRectF boundingRect() const;
		int width() const;
		int height() const;

		const QSize& getSizeLimit() const;

		void setSizeLimit(int limitWidth, int limitHeight);
		void setSizeWidthLimit(int limitWidth);
		void setSizeHeightLimit(int limitHeight);
		void setUpdateNeeded();

		virtual bool needsUpdate();
		virtual void updateSubtreeState();
		virtual void determineChildren() = 0;
		virtual void updateState() = 0;

		void setShape(Shape* shape);
		void setShapeStyle(ShapeStyle* style);
		Shape* getShape() const;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

inline int Item::width() const { return size.width(); }
inline int Item::height() const { return size.height(); }
inline const QSize& Item::getSizeLimit() const { return sizeLimit; }
inline Shape* Item::getShape() const {	return shape_; };

inline void Item::setUpdateNeeded() { needsUpdate_ = true; };

}

#endif /* ITEM_H_ */
