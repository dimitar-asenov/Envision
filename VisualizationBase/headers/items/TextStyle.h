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

namespace Visualization {

class VISUALIZATIONBASE_API TextStyle
{
	private:
		QPen pen_;
		QFont font_;

	public:
		TextStyle();
		void load();

		QPen& pen();
		QFont& font();

		static TextStyle* getDefault();
};

inline QPen& TextStyle::pen() { return pen_; };
inline QFont& TextStyle::font() { return font_; };

}

#endif /* TEXTSTYLE_H_ */
