/***********************************************************************************************************************
 * Item.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ITEM_H_
#define ITEM_H_

#include "visualizationbase_api.h"

#include <QtGui/QGraphicsItem>

namespace Visualization {

class VISUALIZATIONBASE_API Item : public QGraphicsItem
{
	public:
		Item(Item* parent);
};

}

#endif /* ITEM_H_ */
