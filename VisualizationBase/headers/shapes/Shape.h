/***********************************************************************************************************************
 * Shape.h
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SHAPE_H_
#define SHAPE_H_

#include "visualizationbase_api.h"

#include "../Item.h"

namespace Visualization {

class VISUALIZATIONBASE_API Shape
{
	protected:
		typedef enum {InnerSize, OutterSize} SizeType;

	private:
		Item* parent;
		SizeType sizeToUse;
		int width_;
		int height_;
		int xOffset_;
		int yOffset_;

	protected:
		int width();
		int height();
		int xOffset();
		int yOffset();
		SizeType sizeSpecified();

		void setItemSize(int width, int height);
		void setItemBoundingRect(int x, int y, int width, int height);

		virtual void update() = 0;

	public:
		Shape(Item* parent);
		virtual ~Shape();

		void setInnerWidth(int width);
		void setInnerHeight(int height);
		void setInnerSize(int widht, int height);

		void setOutterWidth(int width);
		void setOutterHeight(int height);
		void setOutterSize(int width, int height);
		void setOffset(int x, int y);

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) = 0;
};

}

#endif /* SHAPE_H_ */
