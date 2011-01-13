/***********************************************************************************************************************
 * ItemStyle.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/ItemStyle.h"
#include "shapes/Shape.h"
#include "shapes/ShapeStyle.h"
#include "Styles.h"

namespace Visualization {

ItemStyle::ItemStyle()
{
	// TODO Auto-generated constructor stub

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

void ItemStyle::load()
{
	QString shape;
	Styles::load("shape", shapeName_);
	shapeName_ = shapeName_.trimmed();

	if ( !shapeName_.isEmpty() )
	{
		shapeStyle_ = Shape::createNewShapeStyle(shapeName_);
		Styles::load("shape", *shapeStyle_);
	}
	else shapeStyle_ = NULL;
}

}
