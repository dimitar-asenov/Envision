/***********************************************************************************************************************
 * TextStyle.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TEXTSTYLE_H_
#define TEXTSTYLE_H_

#include "../visualizationbase_api.h"

#include "ItemStyle.h"

#include <QtGui/QPen>
#include <QtGui/QFont>

namespace Visualization {

class VISUALIZATIONBASE_API TextStyle : public ItemStyle
{
	private:
		QPen pen_;
		QFont font_;
		QPen caretPen_;
		QPen selectionPen_;
		QFont selectionFont_;
		QBrush selectionBackground_;

	public:
		TextStyle();
		void load(StyleLoader& sl);

		const QPen& pen() const;
		const QFont& font() const;
		const QPen& caretPen() const;
		const QPen& selectionPen() const;
		const QFont& selectionFont() const;
		const QBrush& selectionBackground() const;
};

inline const QPen& TextStyle::pen() const { return pen_; };
inline const QFont& TextStyle::font() const { return font_; };
inline const QPen& TextStyle::caretPen() const { return caretPen_; };
inline const QPen& TextStyle::selectionPen() const { return selectionPen_; };
inline const QFont& TextStyle::selectionFont() const { return selectionFont_; };
inline const QBrush& TextStyle::selectionBackground() const { return selectionBackground_; };

}

#endif /* TEXTSTYLE_H_ */
