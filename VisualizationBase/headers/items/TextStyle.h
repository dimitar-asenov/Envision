/***********************************************************************************************************************
 * TextStyle.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TEXTSTYLE_H_
#define TEXTSTYLE_H_

#include "visualizationbase_api.h"

#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QTextOption>

namespace Visualization {

class VISUALIZATIONBASE_API TextStyle
{
	private:
		QPen pen_;
		QFont font_;
		QTextOption option_;

	public:
		TextStyle();

		QPen& pen();
		QFont& font();
		QTextOption& option();

		static TextStyle* getDefault();
};

inline QPen& TextStyle::pen() { return pen_; };
inline QFont& TextStyle::font() { return font_; };
inline QTextOption& TextStyle::option() { return option_; };

}

#endif /* TEXTSTYLE_H_ */
