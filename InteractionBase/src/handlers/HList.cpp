/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

/***********************************************************************************************************************
 * HList.cpp
 *
 *  Created on: Jan 26, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HList.h"
#include "handlers/SetCursorEvent.h"

#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/cursor/LayoutCursor.h"
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
	Visualization::VList* list = static_cast<Visualization::VList*> (target);

	if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)  && event->key() == Qt::Key_V)
	{

		FilePersistence::SystemClipboard clipboard;
		int selIndex = list->focusedElementIndex();

		if (clipboard.numNodes() > 0 && selIndex >= 0)
		{
			Model::List* l = dynamic_cast<Model::List*> (list->node());
			FilePersistence::SystemClipboard clipboard;
			l->model()->beginModification(l,"paste");
			if (l) l->paste(clipboard, selIndex+1);
			l->model()->endModification();
			target->setUpdateNeeded();
		}
	}
	else if (event->modifiers() == Qt::NoModifier
			&& (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace))
	{
		int index = list->layout()->focusedElementIndex();
		if (index == -1 && list->scene()->mainCursor() && list->scene()->mainCursor()->owner() == list->layout())
			index = list->layout()->correspondingSceneCursor<Visualization::LayoutCursor>()->index();

		if ((index > 0 && event->key() == Qt::Key_Backspace)
				|| (index < list->layout()->length() && event->key() == Qt::Key_Delete))
		{
			if (event->key() == Qt::Key_Backspace) --index;

			list->node()->model()->beginModification(list->node(), "remove element");
			list->node()->remove(index, false);
			list->node()->model()->endModification();

			if (event->key() == Qt::Key_Backspace) --index;
			list->setUpdateNeeded();

			Model::Node* focusTarget = list->node();
			if (index >=0 && index < list->node()->size())
				focusTarget = list->node()->at<Model::Node>(index);

			QApplication::postEvent(target->scene(),
				new Interaction::SetCursorEvent(target, focusTarget, (event->key() == Qt::Key_Backspace ?
						(list->layout()->isHorizontal() ?
								Interaction::SetCursorEvent::CursorOnRight : Interaction::SetCursorEvent::CursorOnBottom)
						: (list->layout()->isHorizontal() ?
								Interaction::SetCursorEvent::CursorOnLeft : Interaction::SetCursorEvent::CursorOnTop))));
		}

	}
	else GenericHandler::keyPressEvent(target, event);
}

}
