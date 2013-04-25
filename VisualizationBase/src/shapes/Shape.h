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

#pragma once

#include "../visualizationbase_api.h"
#include "shapeMacros.h"

#include "../items/Item.h"
#include "ShapeStyle.h"

#include "../VisualizationException.h"

namespace Visualization {

class VISUALIZATIONBASE_API Shape
{
	public:
		typedef ShapeStyle StyleType;
		static const QString& staticTypeName();

		const StyleType* style() const;
		virtual void setStyle(const Visualization::ShapeStyle* style);
		static StyleSet<Shape>& itemStyles();

		Shape(Item* parent, ShapeStyle *style = nullptr);
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

inline QSize Shape::innerSize(int outterWidth, int outterHeight) const { return innerSize(QSize(outterWidth, outterHeight)); }
inline QSize Shape::outterSize(int innerWidth, int innerHeight) const { return outterSize(QSize(innerWidth, innerHeight)); }

template <class Base, class Actual> inline Base* Shape::makeDefaultStyle() { return new Actual(); }
template <class Base, class Actual> inline Base* Shape::makeDefaultShape(Item* parent) {return new Actual(parent); }

template <class ShapeClass> inline void Shape::registerShape()
{
	if (shapeConstructors.contains(ShapeClass::staticTypeName()))
		throw VisualizationException("Trying to register an already registered shape type "
				+ ShapeClass::staticTypeName());

	shapeConstructors.insert(ShapeClass::staticTypeName(),
			makeDefaultShape<Shape,ShapeClass>);
	shapeStyleConstructors.insert(ShapeClass::staticTypeName(),
			makeDefaultStyle<ShapeStyle,typename ShapeClass::StyleType>);
}

}
