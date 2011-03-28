/***********************************************************************************************************************
 * Shape.h
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SHAPE_H_
#define SHAPE_H_

#include "../visualizationbase_api.h"
#include "shapeMacros.h"

#include "../items/Item.h"
#include "ShapeStyle.h"

#include "../VisualizationException.h"

#include <QtCore/QString>
#include <QtCore/QMap>

namespace Visualization {

class VISUALIZATIONBASE_API Shape
{
	public:
		typedef ShapeStyle StyleType;
		static const QString& className();

		const StyleType* style() const;
		virtual void setStyle(const Visualization::ShapeStyle* style);
		static StyleSet<Shape>& itemStyles();

		Shape(Item* parent, ShapeStyle *style = NULL);
		virtual ~Shape();

		void setInnerSize(int width, int height);
		void setOutterSize(int width, int height);
		void setOffset(int x, int y);

		virtual int contentLeft();
		virtual int contentTop();

		virtual QSize innerSize(QSize outterSize) const;
		virtual QSize outterSize(QSize innerSize) const;
		QSize innerSize(int outterWidth, int outterHeight) const;
		QSize outterSize(int innerWidth, int innerHeight) const;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) = 0;

		template <class ShapeClass> static void registerShape();
		static Shape* createNewShape(const QString& shapeName, Item* parent);
		static ShapeStyle* createNewShapeStyle(const QString& shapeName);

	protected:
		typedef enum {InnerSize, OutterSize} SizeType;

		int width() const;
		int height() const;
		int xOffset() const;
		int yOffset() const;
		SizeType sizeSpecified() const;

		void setItemSize(int width, int height);
		void setParentNeedsUpdate();

		virtual void update() = 0;

	private:
		Item* parent;
		const ShapeStyle* style_;

		SizeType sizeToUse;
		int width_;
		int height_;
		int xOffset_;
		int yOffset_;

		typedef Shape* (*ShapeConstructor)(Item* parent);
		typedef ShapeStyle* (*ShapeStyleConstructor)();
		static QMap<QString, ShapeConstructor> shapeConstructors;
		static QMap<QString, ShapeStyleConstructor> shapeStyleConstructors;
		template <class Base, class Actual> static Base* makeDefaultStyle();
		template <class Base, class Actual> static Base* makeDefaultShape(Item* parent);
};

inline const Shape::StyleType* Shape::style() const { return style_; }

inline int Shape::width() const { return width_; }
inline int Shape::height() const { return height_; }
inline int Shape::xOffset() const { return xOffset_; }
inline int Shape::yOffset() const { return yOffset_; }
inline Shape::SizeType Shape::sizeSpecified() const { return sizeToUse; }

inline void Shape::setParentNeedsUpdate() { parent->setUpdateNeeded(); }

inline QSize Shape::innerSize(int outterWidth, int outterHeight) const { return innerSize(QSize(outterWidth, outterHeight)); }
inline QSize Shape::outterSize(int innerWidth, int innerHeight) const { return outterSize(QSize(innerWidth, innerHeight)); }

template <class Base, class Actual> inline Base* Shape::makeDefaultStyle() { return new Actual(); }
template <class Base, class Actual> inline Base* Shape::makeDefaultShape(Item* parent) {return new Actual(parent); };

template <class ShapeClass> inline void Shape::registerShape()
{
	if (shapeConstructors.contains(ShapeClass::className()))
		throw VisualizationException("Trying to register an already registered shape type " + ShapeClass::className());

	shapeConstructors.insert(ShapeClass::className(),makeDefaultShape<Shape,ShapeClass>);
	shapeStyleConstructors.insert(ShapeClass::className(),makeDefaultStyle<ShapeStyle,typename ShapeClass::StyleType>);
}

}

#endif /* SHAPE_H_ */
