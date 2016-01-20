/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "HActionPrompt.h"

#include "../actions/ActionPrompt.h"
#include "../actions/Action.h"
#include "../events/SetCursorEvent.h"

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
		tryExecutingAction(prompt, true);
	}
	else if (event->key() == Qt::Key_Delete)
	{
		prompt->text()->setText("remove");
		auto executed = tryExecutingAction(prompt, true);
		if (!executed)
			prompt->scene()->addPostEventAction(new SetCursorEvent{prompt->text(), SetCursorEvent::CursorOnRight});
	}
	else if (event->key() == Qt::Key_Backspace)
	{
		if (!prompt->text()->text().isEmpty())
		{
			prompt->text()->setText(prompt->text()->text().left(prompt->text()->text().length() - 1));
			prompt->scene()->addPostEventAction(new SetCursorEvent{prompt->text(), SetCursorEvent::CursorOnRight});
		}
	}
	else if (!event->text().isEmpty())
	{
		// Add character to the action text
		prompt->text()->setText(prompt->text()->text() + event->text());

		// Try executing
		bool executed = false;
		if (prompt->autoExecuteAction())
			executed = tryExecutingAction(prompt, false);

		// If execution failed and the prompt is still visible, focus the end of the text.
		if (!executed)
			prompt->scene()->addPostEventAction(new SetCursorEvent{prompt->text(), SetCursorEvent::CursorOnRight});

	} else if ( event->key() == Qt::Key_Up )
	{
		prompt->upParentActionsLevel();
	} else if ( event->key() == Qt::Key_Down )
	{
		prompt->downParentActionsLevel();
	}
	else GenericHandler::keyPressEvent(target, event);
}

bool HActionPrompt::tryExecutingAction(ActionPrompt *prompt, bool requirePreciseMatch)
{
	Action* found{};

	for (auto a : Action::actions(prompt->currentActionReceiver()->node()) )
	{
		auto match = requirePreciseMatch ?
				a->shortcut() == prompt->text()->text() :
				a->shortcut().startsWith(prompt->text()->text());

		if ( match )
		{
			if (found)
			{
				// If there is more than one matching command with the same prefix then do to not execute any
				found = nullptr;
				break;
			}
			else found = a;
		}
	}

	if (found)
	{
		prompt->hidePrompt();
		found->execute(prompt->currentActionReceiver());
		return true;
	}
	return false;
}

}
