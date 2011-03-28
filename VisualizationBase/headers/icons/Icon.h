/***********************************************************************************************************************
 * Icon.h
 *
 *  Created on: Feb 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ICON_H_
#define ICON_H_

#include "../visualizationbase_api.h"

#include "../items/Item.h"
#include "IconStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API Icon: public Item
{
	ITEM_COMMON(Icon)

	protected:
		virtual void determineChildren();

		int xOffset() const;
		int yOffset() const;

		void setInnerSize(int width, int height);

	public:
		Icon(Item* parent, const IconStyle* style);
};

}

#endif /* ICON_H_ */
