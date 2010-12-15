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
#include "ShapeStyle.h"

#include "VisualizationException.h"

namespace Visualization {

class VISUALIZATIONBASE_API Shape
{
	protected:
		typedef enum {InnerSize, OutterSize} SizeType;

	private:
		Item* parent;
		ShapeStyle* style_;

		SizeType sizeToUse;
		int width_;
		int height_;
		int xOffset_;
		int yOffset_;

	protected:
		int width() const;
		int height() const;
		int xOffset() const;
		int yOffset() const;
		SizeType sizeSpecified() const;

		template <class T> void setGenericStyle(ShapeStyle *style)
		{
			T* s = dynamic_cast<T*> (style);
			if (!s) throw VisualizationException("Trying to set a style incompatible with the shape.");
			style_ = s;
		}

		ShapeStyle* style() const;

		void setItemSize(int width, int height);
		void setItemBoundingRect(int x, int y, int width, int height);
		void setParentNeedsUpdate();

		virtual void update() = 0;

	public:
		Shape(Item* parent, ShapeStyle *style = NULL);
		virtual ~Shape();

		void setInnerWidth(int width);
		void setInnerHeight(int height);
		void setInnerSize(int widht, int height);

		void setOutterWidth(int width);
		void setOutterHeight(int height);
		void setOutterSize(int width, int height);
		void setOffset(int x, int y);

		virtual void setStyle(ShapeStyle *style);

		virtual int contentLeft();
		virtual int contentTop();
		virtual int getOutterWidth(int innerWidth) const;
		virtual int getOutterHeight(int innerHeight) const;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) = 0;
};

inline int Shape::width() const { return width_; }
inline int Shape::height() const { return height_; }
inline int Shape::xOffset() const { return xOffset_; }
inline int Shape::yOffset() const { return yOffset_; }
inline Shape::SizeType Shape::sizeSpecified() const { return sizeToUse; }
inline ShapeStyle* Shape::style() const {	return style_; }

}

#endif /* SHAPE_H_ */
