/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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
#include "../styles/Style.h"
#include "../styles/StyleProperty.h"
#include "../shapes/ShapeStyle.h"

namespace Visualization {

class Shape;
class Item;

// TODO: Document somewhere that Styles should be copiable
class VISUALIZATIONBASE_API ItemStyle : public Super<Style>
{
	public:
		ItemStyle();
		virtual ~ItemStyle() override;

		Shape* createShape(Item* parent) const;
		bool hasShape() const;

		virtual void load(StyleLoader& sl) override;

		Property<bool> drawsOnlyShape{this, "drawsOnlyShape"};
		Property<bool> drawShapeWhenEmpty{this, "drawShapeWhenEmpty"};
		Property<bool> wholeItemCursor{this, "wholeItemCursor"};
		Property<bool> noItemRegions{this, "noItemRegions"};
		Property<bool> allowEquivalentCursorsThroughBoundary{this, "allowEquivalentCursorsThroughBoundary"};

		/**
		 * If false, the cursor will move strictly in the direction of the arrow key the user pressed.
		 * If true, the cursor will move any region that is rougly in the direction the user pressed. This is useful
		 * for example in lists of Statements, when pressing down at the end of the statement and we want to end up at
		 * the end of the next line. If that line is shorter than the current and this property is false, we will skip
		 * that line.
		 */
		Property<bool> linelikeCursorMovementAlongX{this, "linelikeCursorMovementAlongX"};
		Property<bool> linelikeCursorMovementAlongY{this, "linelikeCursorMovementAlongY"};

	private:
		QString shapeName_;
		QSharedPointer<ShapeStyle> shapeStyle_;
};

inline bool ItemStyle::hasShape() const { return !shapeName_.isEmpty(); }

}
