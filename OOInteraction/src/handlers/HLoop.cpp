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
 * HLoop.cpp
 *
 *  Created on: Mar 21, 2012
 *      Author: Dimitar Asenov
 */

#include "handlers/HLoop.h"
#include "OOVisualization/src/statements/VLoopStatement.h"
#include "OOVisualization/src/elements/VStatementItemList.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "OOModel/src/statements/ExpressionStatement.h"

#include "InteractionBase/src/handlers/SetCursorEvent.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"

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
	auto vloop = dynamic_cast<OOVisualization::VLoopStatement*> ( target );
	bool processed = false;

	if (event->modifiers() == Qt::NoModifier)
	{
		if (vloop->initStep() && vloop->initStep()->itemOrChildHasFocus() && event->key() == Qt::Key_Tab)
		{
			processed = true;
			event->accept();
			if (vloop->node()->condition())
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent(target,
										vloop->node()->condition(), Interaction::SetCursorEvent::CursorOnLeft));
			else
			{
				auto empty = new OOModel::EmptyExpression();
				vloop->node()->model()->beginModification(vloop->node(), "create condition");
				vloop->node()->setCondition(empty);
				vloop->node()->model()->endModification();

				vloop->header()->setUpdateNeeded(Visualization::Item::StandardUpdate);
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent(target, empty,
						Interaction::SetCursorEvent::CursorOnLeft));
			}
		}
		else if (vloop->condition() && vloop->condition()->itemOrChildHasFocus() && event->key() == Qt::Key_Tab)
		{
			processed = true;
			event->accept();
			auto empty = new OOModel::EmptyExpression();
			vloop->node()->model()->beginModification(vloop->node(), "create updateStep");
			vloop->node()->setUpdateStep(empty);
			vloop->node()->model()->endModification();

			vloop->header()->setUpdateNeeded(Visualization::Item::StandardUpdate);
			target->scene()->addPostEventAction( new Interaction::SetCursorEvent(target, empty,
					Interaction::SetCursorEvent::CursorOnLeft));
		}
		else if (vloop->updateStep() && vloop->updateStep()->itemOrChildHasFocus() && event->key() == Qt::Key_Tab)
		{
			processed = true;
			event->accept();
			if (vloop->node()->initStep())
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent(target,
						vloop->node()->initStep(), Interaction::SetCursorEvent::CursorOnLeft));
			else
			{
				auto empty = new OOModel::EmptyExpression();
				vloop->node()->model()->beginModification(vloop->node(), "create initStep");
				vloop->node()->setInitStep(empty);
				vloop->node()->model()->endModification();

				vloop->header()->setUpdateNeeded(Visualization::Item::StandardUpdate);
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent(target, empty,
						Interaction::SetCursorEvent::CursorOnLeft));
			}

		}
		else if (vloop->body() && !vloop->body()->itemOrChildHasFocus()
				&& (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return))
		{
			processed = true;
			event->accept();
			if (vloop->node()->body()->size() > 0)
			{
				target->scene()->addPostEventAction(
						new Interaction::SetCursorEvent(target, vloop->node()->body()->at(0),
								Interaction::SetCursorEvent::CursorOnLeft));
			}
			else
			{
				auto empty = new OOModel::EmptyExpression();
				vloop->node()->model()->beginModification(vloop->node(), "create then branch");
				vloop->node()->body()->append( new OOModel::ExpressionStatement(empty) );
				vloop->node()->model()->endModification();

				vloop->body()->setUpdateNeeded(Visualization::Item::StandardUpdate);
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent(target, empty,
						Interaction::SetCursorEvent::CursorOnLeft));
			}

		}
	}


	if (!processed) HStatement::keyPressEvent(target, event);
}

} /* namespace OOInteraction */
