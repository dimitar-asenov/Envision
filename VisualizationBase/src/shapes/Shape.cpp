/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#include "shapes/Shape.h"

namespace Visualization {

::Core::InitializationRegistry& shapeTypeInitializationRegistry();
DEFINE_TYPE_ID_BASE(Shape, shapeTypeInitializationRegistry, "Shape",)

QMap<QString, Shape::ShapeConstructor> Shape::shapeConstructors;
QMap<QString, Shape::ShapeStyleConstructor> Shape::shapeStyleConstructors;

Shape::Shape(Item* parent_, ShapeStyle *style) :
	parent(parent_), style_(style), width_(0), height_(0), xOffset_(0), yOffset_(0)
{
}

Shape::~Shape()
{
}

void Shape::setStyle(const Visualization::ShapeStyle* newStyle)
{
	if (style_ == newStyle) return;
	else
	{
		style_ = newStyle;
		setParentNeedsUpdate();
	}
}

StyleSet<Shape>& Shape::itemStyles()
{
	static StyleSet<Shape> styles(QString("shape"));
	return styles;
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

void Shape::setParentNeedsUpdate()
{
	if (parent->needsUpdate() == Item::NoUpdate)
		parent->setUpdateNeeded(Item::StandardUpdate);
}

}
