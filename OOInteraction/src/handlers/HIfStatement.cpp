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
 * HIfStatement.cpp
 *
 *  Created on: Mar 21, 2012
 *      Author: Dimitar Asenov
 */

#include "handlers/HIfStatement.h"

#include "OOVisualization/src/statements/VIfStatement.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "OOModel/src/statements/ExpressionStatement.h"

#include "InteractionBase/src/handlers/SetCursorEvent.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"

namespace OOInteraction {

HIfStatement::HIfStatement()
{}

HIfStatement* HIfStatement::instance()
{
	static HIfStatement h;
	return &h;
}

void HIfStatement::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	auto vif = dynamic_cast<OOVisualization::VIfStatement*> ( target );
	event->ignore();

	if (event->modifiers() == Qt::NoModifier)
	{
		if ( ( vif->condition()->itemOrChildHasFocus()
				&& (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) )
			|| (vif->elseBranch()->itemOrChildHasFocus() && event->key() == Qt::Key_Tab))
		{
			event->accept();
			if (vif->node()->thenBranch()->size() > 0)
			{
				target->scene()->addPostEventAction(
						new Interaction::SetCursorEvent(target, vif->node()->thenBranch()->at(0),
						Interaction::SetCursorEvent::CursorOnLeft));
			}
			else
			{
				auto empty = new OOModel::EmptyExpression();
				auto es = new OOModel::ExpressionStatement();
				es->setExpression(empty);
				vif->node()->model()->beginModification(vif->node(), "create then branch");
				vif->node()->thenBranch()->append(es);
				vif->node()->model()->endModification();

				vif->thenBranch()->setUpdateNeeded(Visualization::Item::StandardUpdate);
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent(target, empty,
						Interaction::SetCursorEvent::CursorOnLeft));
			}
		}
		else if (vif->thenBranch()->itemOrChildHasFocus() && event->key() == Qt::Key_Tab)
		{
			event->accept();

			if (vif->node()->elseBranch()->size() > 0)
			{
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent(target,
						vif->node()->elseBranch()->at(0), Interaction::SetCursorEvent::CursorOnLeft));
			}
			else
			{
				auto empty = new OOModel::EmptyExpression();
				auto es = new OOModel::ExpressionStatement();
				es->setExpression(empty);
				vif->node()->model()->beginModification(vif->node(), "create else branch");
				vif->node()->elseBranch()->append(es);
				vif->node()->model()->endModification();

				vif->elseBranch()->setUpdateNeeded(Visualization::Item::StandardUpdate);
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent(target, empty,
						Interaction::SetCursorEvent::CursorOnLeft));
			}
		}
	}


	if (!event->isAccepted()) HStatement::keyPressEvent(target, event);
}

} /* namespace OOInteraction */
