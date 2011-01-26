/***********************************************************************************************************************
 * HList.cpp
 *
 *  Created on: Jan 26, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HList.h"

#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/Scene.h"
#include "FilePersistence/headers/SystemClipboard.h"
#include "ModelBase/headers/nodes/List.h"

namespace Interaction {

HList::HList()
{

}

HList* HList::instance()
{
	static HList h;
	return &h;
}

void HList::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)  && event->key() == Qt::Key_V)
	{
		Visualization::VList* list = static_cast<Visualization::VList*> (target);
		FilePersistence::SystemClipboard clipboard;
		int selIndex = list->focusedElementIndex();

		if (clipboard.numNodes() > 0 && selIndex >= 0)
		{
			Model::List* l = dynamic_cast<Model::List*> (list->getNode());
			FilePersistence::SystemClipboard clipboard;
			l->model()->beginModification(l,"paste");
			if (l) l->paste(clipboard, selIndex+1);
			l->model()->endModification();
			target->setUpdateNeeded();
		}
	}
	else GenericHandler::keyPressEvent(target, event);
}

}
