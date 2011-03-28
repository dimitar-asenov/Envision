/***********************************************************************************************************************
 * ItemStyle.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/ItemStyle.h"
#include "shapes/Shape.h"
#include "shapes/ShapeStyle.h"

namespace Visualization {

ItemStyle::ItemStyle() :
	shapeStyle_(NULL)
{
}

ItemStyle::~ItemStyle()
{
	SAFE_DELETE(shapeStyle_);
}

Shape* ItemStyle::createShape(Item* parent) const
{
	if (shapeName_.isEmpty()) return NULL;

	Shape* shape = Shape::createNewShape(shapeName_, parent);
	shape->setStyle(shapeStyle_);
	return shape;
}

void ItemStyle::load(StyleLoader& sl)
{
	QString shape;
	sl.load("drawsOnlyShape", drawsOnlyShape_);
	sl.load("drawShapeWhenEmpty", drawShapeWhenEmpty_);
	sl.load("shape", shapeName_);
	shapeName_ = shapeName_.trimmed();

	if ( !shapeName_.isEmpty() )
	{
		shapeStyle_ = Shape::createNewShapeStyle(shapeName_);
		sl.load("shape", *shapeStyle_);
	}
	else shapeStyle_ = NULL;
}

}
