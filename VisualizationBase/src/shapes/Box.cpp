/***********************************************************************************************************************
 * Box.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "shapes/Box.h"
#include "VisualizationException.h"

#include <QtGui/QPainter>

namespace Visualization {

Box::Box(Item *parent, BoxStyle *style) :
	Shape(parent, style)
{
}

void Box::setStyle(ShapeStyle *style)
{
	setGenericStyle<BoxStyle> (style);
}

BoxStyle* Box::style() const
{
	return static_cast<BoxStyle*> (Shape::style());
}

void Box::update()
{
	setItemSize(width(), height());
	setItemBoundingRect(xOffset(), yOffset(), width(), height());
}

QPainterPath Box::getRectanglePath(int x, int y, int width, int height)
{
	QPainterPath path;

	path.moveTo(width + x, style()->cornerRadius() + y);

	if ( style()->corner() == BoxStyle::CutCorner ) path.lineTo(width + x - style()->cornerRadius(), y);
	else
		path.arcTo(width - 2 * style()->cornerRadius() + x, y, style()->cornerRadius() * 2, style()->cornerRadius() * 2, 0.0, 90.0);

	path.lineTo(style()->cornerRadius() + x, y);

	if ( style()->corner() == BoxStyle::CutCorner ) path.lineTo(x, style()->cornerRadius() + y);
	else
		path.arcTo(x, y, style()->cornerRadius() * 2, style()->cornerRadius() * 2, 90.0, 90.0);

	path.lineTo(x, height - style()->cornerRadius() + y);

	if ( style()->corner() == BoxStyle::CutCorner ) path.lineTo(x + style()->cornerRadius(), height + y);
	else
		path.arcTo(x, height - 2 * style()->cornerRadius() + y, style()->cornerRadius() * 2, style()->cornerRadius() * 2, 180.0, 90.0);

	path.lineTo(width - style()->cornerRadius() + x, height + y);

	if ( style()->corner() == BoxStyle::CutCorner ) path.lineTo(x + width, height - style()->cornerRadius() + y);
	else
		path.arcTo(width - 2 * style()->cornerRadius() + x, height - 2 * style()->cornerRadius() + y, style()->cornerRadius() * 2, style()->cornerRadius() * 2, 270.0, 90.0);

	path.closeSubpath();
	return path;
}

void Box::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(style()->outline());
	painter->setBrush(style()->background());

	painter->drawRect(xOffset(), yOffset(), width(), height());
}

}
