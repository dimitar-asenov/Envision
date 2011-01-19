/***********************************************************************************************************************
 * HSceneHandlerItem.h
 *
 *  Created on: Jan 19, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef HSCENEHANDLERITEM_H_
#define HSCENEHANDLERITEM_H_

#include "../interactionbase_api.h"

#include "GenericHandler.h"

namespace Interaction {

class INTERACTIONBASE_API HSceneHandlerItem : public GenericHandler
{
	public:
		static HSceneHandlerItem* instance();

	protected:
		HSceneHandlerItem();
};

}

#endif /* HSCENEHANDLERITEM_H_ */
