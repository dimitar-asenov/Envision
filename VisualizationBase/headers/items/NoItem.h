/***********************************************************************************************************************
 * NoItem.h
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef NOITEM_H_
#define NOITEM_H_

#include "visualizationbase_api.h"

#include "Item.h"

namespace Visualization {

class VISUALIZATIONBASE_API NoItem: public Visualization::Item
{
	public:
		NoItem(Item* parent);

		virtual bool needsUpdate();
		virtual void determineChildren();
		virtual void updateState();
		virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
};

}

#endif /* NOITEM_H_ */
