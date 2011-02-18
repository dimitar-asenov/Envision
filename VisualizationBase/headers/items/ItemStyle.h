/***********************************************************************************************************************
 * ItemStyle.h
 *
 *  Created on: Dec 21, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ITEMSTYLE_H_
#define ITEMSTYLE_H_

#include "../visualizationbase_api.h"

#include <QtCore/QString>

namespace Visualization {

class ShapeStyle;
class Shape;
class Item;

class VISUALIZATIONBASE_API ItemStyle
{
	private:
		ShapeStyle* shapeStyle_;
		QString shapeName_;
		bool drawsOnlyShape_;
		bool drawShapeWhenEmpty_;

	public:
		ItemStyle();
		virtual ~ItemStyle();

		Shape* createShape(Item* parent) const;
		bool hasShape() const;

		virtual void load();

		bool drawsOnlyShape() const;
		bool drawShapeWhenEmpty() const;
};

inline bool ItemStyle::hasShape() const { return !shapeName_.isEmpty(); }
inline bool ItemStyle::drawsOnlyShape() const { return drawsOnlyShape_; }
inline bool ItemStyle::drawShapeWhenEmpty() const {return drawShapeWhenEmpty_; }

}

#endif /* ITEMSTYLE_H_ */
