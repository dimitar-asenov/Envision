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
		QPen selectionPen_;
		QFont selectionFont_;

	public:
		TextStyle();
		void load();

		const QPen& pen() const;
		const QFont& font() const;
		const QPen& selectionPen() const;
		const QFont& selectionFont() const;

		static TextStyle* getDefault();
};

inline const QPen& TextStyle::pen() const { return pen_; };
inline const QFont& TextStyle::font() const { return font_; };
inline const QPen& TextStyle::selectionPen() const { return selectionPen_; };
inline const QFont& TextStyle::selectionFont() const { return selectionFont_; };

}

#endif /* TEXTSTYLE_H_ */
