/***********************************************************************************************************************
 * GenericHandler.h
 *
 *  Created on: Jan 13, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef GENERICHANDLER_H_
#define GENERICHANDLER_H_

#include "interactionbase_api.h"

#include "VisualizationBase/headers/items/Item.h"
#include "VisualizationBase/headers/InteractionHandler.h"

namespace Interaction {

class INTERACTIONBASE_API GenericHandler : public Visualization::InteractionHandler
{
	protected:
		GenericHandler();

	public:
		static GenericHandler* instance();

		virtual void mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
};

}

#endif /* GENERICHANDLER_H_ */
