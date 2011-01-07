/***********************************************************************************************************************
 * Item.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ITEM_H_
#define ITEM_H_

#include "visualizationbase_api.h"
#include "ItemMacros.h"

#include <QtGui/QGraphicsItem>

namespace Visualization {

class Shape;
class ShapeStyle;
class ItemStyle;

class VISUALIZATIONBASE_API Item : public QGraphicsItem
{
	private:
		friend class Shape;
		const ItemStyle* style_;
		Shape* shape_;
		bool needsUpdate_;

		void updateChildren();

	protected:
		QRectF bounding_rect;
		QSize size;

		virtual void determineChildren() = 0;
		virtual void updateState() = 0;

	public:
		Item(Item* parent, const ItemStyle* style = NULL);
		virtual ~Item();

		QRectF boundingRect() const;
		int width() const;
		int height() const;

		void setUpdateNeeded();

		virtual bool needsUpdate();
		virtual void updateSubtreeState();

		virtual void setStyle(const ItemStyle* style);
		const ItemStyle* style() const;
		Shape* getShape() const;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

inline int Item::width() const { return size.width(); }
inline int Item::height() const { return size.height(); }
inline const ItemStyle* Item::style() const { return style_; }
inline Shape* Item::getShape() const {	return shape_; }
inline QRectF Item::boundingRect() const { return bounding_rect; }
inline bool Item::needsUpdate() { return needsUpdate_; }

}

#endif /* ITEM_H_ */
