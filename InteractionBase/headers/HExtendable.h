/***********************************************************************************************************************
 * HExtendable.h
 *
 *  Created on: Jan 13, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef HEXTENDABLE_H_
#define HEXTENDABLE_H_

#include "interactionbase_api.h"

#include "GenericHandler.h"

namespace Interaction {

class INTERACTIONBASE_API HExtendable : public GenericHandler
{
	protected:
		HExtendable();

	public:
		static HExtendable* instance();

		virtual void mouseDoubleClickEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
};

}

#endif /* HEXTENDABLE_H_ */
