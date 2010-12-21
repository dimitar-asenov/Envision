/***********************************************************************************************************************
 * TextStyle.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TEXTSTYLE_H_
#define TEXTSTYLE_H_

#include "visualizationbase_api.h"

#include "ItemStyle.h"

#include <QtGui/QPen>
#include <QtGui/QFont>

namespace Visualization {

class VISUALIZATIONBASE_API TextStyle : public ItemStyle
{
	private:
		QPen pen_;
		QFont font_;

	public:
		TextStyle();
		void load();

		const QPen& pen() const;
		const QFont& font() const;

		static TextStyle* getDefault();
};

inline const QPen& TextStyle::pen() const { return pen_; };
inline const QFont& TextStyle::font() const { return font_; };

}

#endif /* TEXTSTYLE_H_ */
