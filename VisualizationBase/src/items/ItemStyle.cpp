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

#include "items/ItemStyle.h"
#include "shapes/Shape.h"

namespace Visualization {

ItemStyle::ItemStyle() :
	shapeStyle_(nullptr)
{
}

ItemStyle::~ItemStyle()
{
}

Shape* ItemStyle::createShape(Item* parent) const
{
	if (shapeName_.isEmpty()) return nullptr;

	Shape* shape = Shape::createNewShape(shapeName_, parent);
	shape->setStyle(shapeStyle_.data());
	return shape;
}

void ItemStyle::load(StyleLoader& sl)
{
	Style::load(sl);

	QString shape;
	sl.load("wholeItemCursor", wholeItemCursor_);
	sl.load("noItemRegions", noItemRegions_);
	sl.load("drawsOnlyShape", drawsOnlyShape_);
	sl.load("drawShapeWhenEmpty", drawShapeWhenEmpty_);
	sl.load("allowEquivalentCursorsThroughBoundary", allowEquivalentCursorsThroughBoundary_);
	sl.load("shape", shapeName_);
	shapeName_ = shapeName_.trimmed();

	if ( !shapeName_.isEmpty() )
	{
		shapeStyle_ = QSharedPointer<ShapeStyle>(Shape::createNewShapeStyle(shapeName_));
		sl.load("shape", *shapeStyle_);
	}
	else shapeStyle_.clear();
}

}
