/***********************************************************************************************************************
 * HPositionLayout.h
 *
 *  Created on: Feb 3, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef HPOSITIONLAYOUT_H_
#define HPOSITIONLAYOUT_H_

#include "../interactionbase_api.h"

#include "GenericHandler.h"

#include "VisualizationBase/headers/items/Item.h"
#include "VisualizationBase/headers/node_extensions/Position.h"

namespace Interaction {

class INTERACTIONBASE_API HPositionLayout : public GenericHandler
{
	public:
		// Mouse events
		virtual void mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseReleaseEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);

		static HPositionLayout* instance();

	protected:
		HPositionLayout();

	private:
		int originalX;
		int originalY;
		Visualization::Item* currentItem;
		Visualization::Position* currentItemPosition;

};

}

#endif /* HPOSITIONLAYOUT_H_ */
