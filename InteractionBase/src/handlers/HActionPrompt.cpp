/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

/*
 * HActionPrompt.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: Dimitar Asenov
 */

#include "HActionPrompt.h"

#include "actions/ActionPrompt.h"
#include "actions/Action.h"
#include "events/SetCursorEvent.h"

#include "VisualizationBase/src/Scene.h"

namespace Interaction {

HActionPrompt::HActionPrompt()
{
}

HActionPrompt* HActionPrompt::instance()
{
	static HActionPrompt h;
	return &h;
}

void HActionPrompt::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	if (event->modifiers() != Qt::NoModifier)
		return GenericHandler::keyPressEvent(target, event);


	ActionPrompt* prompt = static_cast<ActionPrompt*> (target);

	if (event->key() == Qt::Key_Escape)
	{
		prompt->hidePrompt();
	}
	else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
	{
		tryExecutingAction(prompt);
	} else if (event->key() == Qt::Key_Backspace && !prompt->text()->text().isEmpty())
	{
		prompt->text()->setText(prompt->text()->text().left(prompt->text()->text().length() - 1));
		prompt->scene()->addPostEventAction(new SetCursorEvent(prompt->text(), SetCursorEvent::CursorOnRight));
	}
	else if (!event->text().isEmpty())
	{
		prompt->text()->setText(prompt->text()->text() + event->text());
		prompt->scene()->addPostEventAction(new SetCursorEvent(prompt->text(), SetCursorEvent::CursorOnRight));
		if (prompt->autoExecuteAction()) tryExecutingAction(prompt);
	} else if ( event->key() == Qt::Key_Up )
	{
		prompt->upParentActionsLevel();
	} else if ( event->key() == Qt::Key_Down )
	{
		prompt->downParentActionsLevel();
	}
	else GenericHandler::keyPressEvent(target, event);
}

bool HActionPrompt::tryExecutingAction(ActionPrompt *prompt)
{
	Action* found{};
	for(auto a : Action::actions(prompt->actionReceiver()->node()) )
	{
		if( a->shortcut().startsWith(prompt->text()->text()))
		{
			if (found)
			{
				// If there are more than two commands with the same prefix then do to not execute
				found = nullptr;
				break;
			}
			else found = a;
		}
	}

	if (found)
	{
		found->execute(prompt->actionReceiver());
		prompt->hidePrompt();
		return true;
	}
	return false;
}

} /* namespace Interaction */
