/***********************************************************************************************************************
 * Shape.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/Shape.h"

namespace Visualization {

Shape::Shape(Item* parent_) :
	parent(parent_), width_(0), height_(0), xOffset_(0), yOffset_(0)
{
}

Shape::~Shape()
{
}

int Shape::width()
{
	return width_;
}

int Shape::height()
{
	return height_;
}

int Shape::xOffset()
{
	return xOffset_;
}

int Shape::yOffset()
{
	return yOffset_;
}

Shape::SizeType Shape::sizeSpecified()
{
	return sizeToUse;
}

void Shape::setItemSize(int width, int height)
{
	parent->size.setWidth(width);
	parent->size.setHeight(height);
}

void Shape::setItemBoundingRect(int x, int y, int width, int height)
{
	parent->bounding_rect.setRect(x, y, width, height);
}

void Shape::setInnerWidth(int width)
{
	width_ = width;
	if ( sizeToUse == OutterSize ) height_ = 0;
	sizeToUse = InnerSize;
	update();
}

void Shape::setInnerHeight(int height)
{
	height_ = height;
	if ( sizeToUse == OutterSize ) width_ = 0;
	sizeToUse = InnerSize;
	update();
}

void Shape::setInnerSize(int width, int height)
{
	width_ = width;
	height_ = height;
	sizeToUse = InnerSize;
	update();
}

void Shape::setOutterWidth(int width)
{
	width_ = width;
	if ( sizeToUse == InnerSize ) height_ = 0;
	sizeToUse = OutterSize;
	update();
}
void Shape::setOutterHeight(int height)
{
	height_ = height;
	if ( sizeToUse == InnerSize ) width_ = 0;
	sizeToUse = OutterSize;
	update();
}

void Shape::setOutterSize(int width, int height)
{
	width_ = width;
	height_ = height;
	sizeToUse = OutterSize;
	update();
}

void Shape::setOffset(int x, int y)
{
	xOffset_ = x;
	yOffset_ = y;
	update();
}

}
