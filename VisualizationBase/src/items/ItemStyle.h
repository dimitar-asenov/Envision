/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * ItemStyle.h
 *
 *  Created on: Dec 21, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ITEMSTYLE_H_
#define ITEMSTYLE_H_

#include "../visualizationbase_api.h"
#include "../styles/Style.h"

namespace Visualization {

class ShapeStyle;
class Shape;
class Item;

class VISUALIZATIONBASE_API ItemStyle : public Style
{
	private:
		ShapeStyle* shapeStyle_;
		QString shapeName_;
		bool drawsOnlyShape_;
		bool drawShapeWhenEmpty_;
		bool wholeItemCursor_;

	public:
		ItemStyle();
		virtual ~ItemStyle();

		Shape* createShape(Item* parent) const;
		bool hasShape() const;

		virtual void load(StyleLoader& sl);

		bool drawsOnlyShape() const;
		bool drawShapeWhenEmpty() const;
		bool wholeItemCursor() const;
};

inline bool ItemStyle::hasShape() const { return !shapeName_.isEmpty(); }
inline bool ItemStyle::drawsOnlyShape() const { return drawsOnlyShape_; }
inline bool ItemStyle::drawShapeWhenEmpty() const {return drawShapeWhenEmpty_; }
inline bool ItemStyle::wholeItemCursor() const {return wholeItemCursor_; }

}

#endif /* ITEMSTYLE_H_ */
