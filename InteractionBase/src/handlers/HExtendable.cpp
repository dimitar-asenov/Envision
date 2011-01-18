/***********************************************************************************************************************
 * HExtendable.cpp
 *
 *  Created on: Jan 13, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HExtendable.h"

#include "VisualizationBase/headers/items/VExtendable.h"
#include "VisualizationBase/headers/Scene.h"

namespace Interaction {

HExtendable::HExtendable()
{

}

HExtendable* HExtendable::instance()
{
	static HExtendable h;
	return &h;
}

void HExtendable::mouseDoubleClickEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		Visualization::VExtendable *ext = dynamic_cast<Visualization::VExtendable*> (target);
		ext->setExpanded(! ext->expanded());
		target->scene()->updateTopLevelItems();
	}
	else GenericHandler::mouseDoubleClickEvent(target, event);
}

}
