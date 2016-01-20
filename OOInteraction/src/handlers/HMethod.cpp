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

#include "HMethod.h"

#include "InteractionBase/src/commands/CDiff.h"
#include "InteractionBase/src/commands/CHistory.h"

#include "OOVisualization/src/declarations/VMethod.h"
#include "OOVisualization/src/elements/VStatementItemList.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "OOModel/src/statements/ExpressionStatement.h"
#include "OOModel/src/elements/FormalArgument.h"

#include "InteractionBase/src/events/SetCursorEvent.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"

namespace OOInteraction {

HMethod::HMethod()
{
	addCommand(new Interaction::CDiff{});
	addCommand(new Interaction::CHistory{});
}

HMethod* HMethod::instance()
{
	static HMethod h;
	return &h;
}

void HMethod::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	bool createDown = event->modifiers() == Qt::NoModifier &&
			(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return);
	bool createRight = event->modifiers() == Qt::ShiftModifier &&
			(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return);
	//bool switchVertical = event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Tab;
	bool switchHorizontal = event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Tab;

	bool processed = false;
	auto m = DCast<OOVisualization::VMethod> ( target );
	if (m)
	{
		if ( (m->name()->itemOrChildHasFocus() || m->arguments()->itemOrChildHasFocus()) && createDown)
		{
			processed = true;
			if (m->node()->items()->size() > 0)
			{
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target, m->node()->items()->at(0)});
			}
			else
			{
				auto empty = new OOModel::EmptyExpression{};
				auto es = new OOModel::ExpressionStatement{};
				es->setExpression(empty);
				m->node()->beginModification("add empty statement");
				m->node()->items()->append(es);
				m->node()->endModification();

				m->setUpdateNeeded(Visualization::Item::StandardUpdate);
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target, empty});
			}
		}
		else if (m->name()->itemOrChildHasFocus() && switchHorizontal)
		{
			processed = true;

			if (m->node()->arguments()->size() > 0)
			{
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target,
						m->node()->arguments()->at(0)});
			}
		}
		else if ( (m->name()->itemOrChildHasFocus()) && createRight)
		{
			processed = true;
			createNewArgument(m, 0);
		}
	}

	if (!processed) GenericHandler::keyPressEvent(target, event);
}

void HMethod::createNewArgument(OOVisualization::VMethod* method, int position)
{
	auto arg = new OOModel::FormalArgument{};
	arg->setTypeExpression(new OOModel::EmptyExpression{});
	method->node()->beginModification("add new argument");
	method->node()->arguments()->insert(position, arg);
	method->node()->endModification();

	method->arguments()->setUpdateNeeded(Visualization::Item::StandardUpdate);
	method->scene()->addPostEventAction( new Interaction::SetCursorEvent{method, arg});
}

}
