/***********************************************************************************************************************
 * HSceneHandlerItem.cpp
 *
 *  Created on: Jan 19, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HSceneHandlerItem.h"

#include "commands/CSceneHandlerItemExit.h"

namespace Interaction {

HSceneHandlerItem::HSceneHandlerItem()
{
	// TODO: is it appropriate to add commands in the constructor or should they be registered somewhere else?
	addCommand(new CSceneHandlerItemExit());
}

HSceneHandlerItem* HSceneHandlerItem::instance()
{
	static HSceneHandlerItem h;
	return &h;
}

}
