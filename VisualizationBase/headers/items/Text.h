/***********************************************************************************************************************
 * Text.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VISUALIZATION_TEXT_H_
#define VISUALIZATION_TEXT_H_

#include "../visualizationbase_api.h"

#include "TextRenderer.h"

namespace Visualization {

class VISUALIZATIONBASE_API Text : public TextRenderer
{
	ITEM_COMMON(Text)

	public:
		Text(Item* parent, const QString& text = QString());
		Text(Item* parent, const StyleType *style, const QString& text = QString());
};

}

#endif /* VISUALIZATION_TEXT_H_ */
