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

#include "HStatement.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "OOModel/src/statements/ExpressionStatement.h"

#include "InteractionBase/src/events/SetCursorEvent.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"

namespace OOInteraction {

HStatement::HStatement()
{}

HStatement* HStatement::instance()
{
	static HStatement h;
	return &h;
}

void HStatement::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	Visualization::VList* lst = nullptr;
	auto p = target->parent();
	while (p && !(lst = DCast<Visualization::VList>(p))) p = p->parent();

	bool processed = false;

	if (lst && event->modifiers() == Qt::NoModifier
			&& (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return))
	{
			processed = true;
			event->accept();

			int index = lst->focusedNodeIndex() + 1;

			auto empty = new OOModel::EmptyExpression{};
			auto es = new OOModel::ExpressionStatement{};
			es->setExpression(empty);
			lst->node()->beginModification( "add empty statement");
			lst->node()->insert(index, es);
			lst->node()->endModification();

			lst->setUpdateNeeded(Visualization::Item::StandardUpdate);
			target->scene()->addPostEventAction( new Interaction::SetCursorEvent{lst, empty});
	}

	if (!processed)
	{
		// check if any of the register handlers, handles the event.
		// TODO: for now only the first handler gets respected,
		// maybe we should add a warning if there are multiple for the same event.
		for (auto handler : keyPressHandlers_)
			if ((processed = handler(target, event))) break;
	}
	if (!processed)
		GenericHandler::keyPressEvent(target, event);
}

}
