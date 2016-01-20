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

#include "HLoop.h"
#include "OOVisualization/src/statements/VLoopStatement.h"
#include "OOVisualization/src/elements/VStatementItemList.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "OOModel/src/statements/ExpressionStatement.h"

#include "InteractionBase/src/events/SetCursorEvent.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/NodeWrapper.h"

namespace OOInteraction {

HLoop::HLoop()
{}

HLoop* HLoop::instance()
{
	static HLoop h;
	return &h;
}

void HLoop::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	auto vloop = DCast<OOVisualization::VLoopStatement> ( target );
	event->ignore();

	bool createDown = event->modifiers() == Qt::NoModifier &&
			(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return);
	bool createRight = event->modifiers() == Qt::ShiftModifier &&
			(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return);
	bool switchHorizontal = event->modifiers() == Qt::NoModifier &&
			( event->key() == Qt::Key_Tab || event->key() == Qt::Key_Semicolon);

	if (vloop->initStep() && vloop->initStep()->itemOrChildHasFocus() && (switchHorizontal || createRight) )
	{
		event->accept();
		if (vloop->node()->condition())
			target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target, vloop->node()->condition()});
		else
		{
			auto empty = new OOModel::EmptyExpression{};
			vloop->node()->beginModification("create condition");
			vloop->node()->setCondition(empty);
			vloop->node()->endModification();

			vloop->setUpdateNeeded(Visualization::Item::StandardUpdate);
			target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target, empty});
		}
	}
	else if (vloop->condition() && vloop->condition()->itemOrChildHasFocus() && (switchHorizontal || createRight))
	{
		event->accept();
		if (vloop->node()->loopKind() == OOModel::LoopStatement::LoopKind::PreCheck)
		{
			if (vloop->node()->updateStep())
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target, vloop->node()->updateStep()});
			else
			{
				auto empty = new OOModel::EmptyExpression{};
				vloop->node()->beginModification("create updateStep");
				vloop->node()->setUpdateStep(empty);
				vloop->node()->endModification();

				vloop->setUpdateNeeded(Visualization::Item::StandardUpdate);
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target, empty});
			}
		}
	}
	else if (vloop->updateStep() && vloop->updateStep()->itemOrChildHasFocus() && (switchHorizontal || createRight))
	{
		event->accept();
		if (vloop->node()->initStep())
			target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target, vloop->node()->initStep()});
		else
		{
			auto empty = new OOModel::EmptyExpression{};
			vloop->node()->beginModification("create initStep");
			vloop->node()->setInitStep(empty);
			vloop->node()->endModification();

			vloop->setUpdateNeeded(Visualization::Item::StandardUpdate);
			target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target, empty});
		}

	}
	else if (vloop->body() && !vloop->body()->itemOrChildHasFocus() && createDown)
	{
		event->accept();
		if (vloop->node()->body()->size() > 0)
		{
			target->scene()->addPostEventAction(
					new Interaction::SetCursorEvent{target, vloop->node()->body()->at(0)});
		}
		else
		{
			auto empty = new OOModel::EmptyExpression{};
			vloop->node()->beginModification("create loop body");
			vloop->node()->body()->append( new OOModel::ExpressionStatement{empty} );
			vloop->node()->endModification();

			vloop->body()->setUpdateNeeded(Visualization::Item::StandardUpdate);
			target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target, empty});
		}

	}else if (vloop->body() && vloop->body()->itemOrChildHasFocus() && (createDown || createRight))
	{
		event->accept();
		if (vloop->node()->condition())
			target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target, vloop->node()->condition()});
		else
		{
			auto empty = new OOModel::EmptyExpression{};
			vloop->node()->beginModification("create condition");
			vloop->node()->setCondition(empty);
			vloop->node()->endModification();

			vloop->setUpdateNeeded(Visualization::Item::StandardUpdate);
			target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target, empty});
		}

	}else if (event->modifiers() == Qt::NoModifier
			&& ((event->key() == Qt::Key_Backspace && vloop->initStep()->itemOrChildHasFocus())
					|| ((event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
							&& vloop->icon()->itemOrChildHasFocus())))
	{
		event->accept();
		removeFromList(target);
	}


	if (!event->isAccepted()) HStatement::keyPressEvent(target, event);
}

}
