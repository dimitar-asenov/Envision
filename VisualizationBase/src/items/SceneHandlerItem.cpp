/***********************************************************************************************************************
 * SceneHandlerItem.cpp
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/SceneHandlerItem.h"
#include "Scene.h"

namespace Visualization {

InteractionHandler* SceneHandlerItem::handler_ = InteractionHandler::instance();

SceneHandlerItem::SceneHandlerItem(Scene* scene) :
	Item(NULL)
{
	scene->addItem(this);
}

void SceneHandlerItem::determineChildren()
{
}

void SceneHandlerItem::updateState()
{
}

}
