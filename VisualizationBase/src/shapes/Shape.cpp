/***********************************************************************************************************************
 * Shape.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/Shape.h"

#include <cmath>
namespace Visualization {

QMap<QString, Shape::ShapeConstructor> Shape::shapeConstructors;
QMap<QString, Shape::ShapeStyleConstructor> Shape::shapeStyleConstructors;

Shape::Shape(Item* parent_, ShapeStyle *style) :
	parent(parent_), style_(style), width_(0), height_(0), xOffset_(0), yOffset_(0)
{
}

Shape::~Shape()
{
}

void Shape::setItemSize(int width, int height)
{
	parent->setSize(width, height);
	setParentNeedsUpdate();
}

void Shape::setInnerSize(int width, int height)
{
	width_ = width;
	height_ = height;
	sizeToUse = InnerSize;
	update();
	setParentNeedsUpdate();
}

void Shape::setOutterSize(int width, int height)
{
	width_ = width;
	height_ = height;
	sizeToUse = OutterSize;
	update();
	setParentNeedsUpdate();
}

void Shape::setOffset(int x, int y)
{
	xOffset_ = x;
	yOffset_ = y;
	update();
	setParentNeedsUpdate();
}

void Shape::setStyle(ShapeStyle *style)
{
	style_ = style;
	setParentNeedsUpdate();
}

int Shape::contentLeft()
{
	return xOffset_;
}

int Shape::contentTop()
{
	return yOffset_;
}

QSize Shape::innerSize(QSize outterSize) const
{
	if (style_) return QSize(outterSize.width() - std::ceil(style_->outline().width()), outterSize.height() - std::ceil(style_->outline().width()) );
	return QSize(outterSize.width(), outterSize.height());
}

QSize Shape::outterSize(QSize innerSize) const
{
	if (style_) return QSize(innerSize.width() + std::ceil(style_->outline().width()), innerSize.height() + std::ceil(style_->outline().width()) );
	return QSize(innerSize.width(), innerSize.height());
}

Shape* Shape::createNewShape(const QString& shapeName, Item* parent)
{
	if (shapeConstructors.contains(shapeName)) return shapeConstructors.value(shapeName)(parent);
	else throw VisualizationException("Trying to create a new shape for an unregistered type " + shapeName);
}

ShapeStyle* Shape::createNewShapeStyle(const QString& shapeName)
{
	if (shapeStyleConstructors.contains(shapeName)) return shapeStyleConstructors.value(shapeName)();
	else throw VisualizationException("Trying to create a new shape style for an unregistered shape type " + shapeName);
}

}
