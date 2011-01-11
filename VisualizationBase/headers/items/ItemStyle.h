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

	public:
		ItemStyle();
		virtual ~ItemStyle();

		Shape* createShape(Item* parent) const;
		bool hasShape() const;

		virtual void load();
};

inline bool ItemStyle::hasShape() const { return !shapeName_.isEmpty(); }

}

#endif /* ITEMSTYLE_H_ */
