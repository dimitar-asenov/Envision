/***********************************************************************************************************************
 * TextStyle.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/TextStyle.h"

namespace Visualization {

TextStyle::TextStyle()
{
	font_.setPixelSize(15);
	selectionFont_.setPixelSize(15);
}

void TextStyle::load(StyleLoader& sl)
{
	ItemStyle::load(sl);
	sl.load("pen", pen_);
	sl.load("font", font_);
	sl.load("caretPen", caretPen_);
	sl.load("selectionPen", selectionPen_);
	sl.load("selectionFont", selectionFont_);

	QColor col;
	sl.load("selectionBackground", col);
	selectionBackground_ = QBrush(col);
}

}
