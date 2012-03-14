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
 * HMethod.cpp
 *
 *  Created on: Mar 1, 2012
 *      Author: Dimitar Asenov
 */

#include "handlers/HMethod.h"

#include "OOVisualization/headers/top_level/VMethod.h"
#include "OOModel/headers/expressions/EmptyExpression.h"
#include "OOModel/headers/statements/ExpressionStatement.h"
#include "OOModel/headers/elements/FormalArgument.h"

#include "InteractionBase/headers/handlers/SetCursorEvent.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/items/VText.h"
#include "VisualizationBase/headers/cursor/LayoutCursor.h"

namespace OOInteraction {

HMethod::HMethod()
{}

HMethod* HMethod::instance()
{
	static HMethod h;
	return &h;
}

void HMethod::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	auto m = dynamic_cast<OOVisualization::VMethod*> ( target );
	bool processed = false;
	if (m && event->modifiers() == Qt::NoModifier)
	{
		if (m->name()->itemOrChildHasFocus() && (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return))
		{
			processed = true;
			event->accept();
			if (m->node()->items()->size() > 0)
			{
				QApplication::postEvent(target->scene(), new Interaction::SetCursorEvent(target, m->node()->items()->at(0),
						Interaction::SetCursorEvent::CursorOnLeft));
			}
			else
			{
				auto empty = new OOModel::EmptyExpression();
				auto es = new OOModel::ExpressionStatement();
				es->setExpression(empty);
				m->node()->model()->beginModification(m->node(), "add empty statement");
				m->node()->items()->append(es);
				m->node()->model()->endModification();

				m->content()->setUpdateNeeded();
				QApplication::postEvent(target->scene(), new Interaction::SetCursorEvent(target, empty,
						Interaction::SetCursorEvent::CursorOnLeft));
			}
		}
		else if (m->name()->itemOrChildHasFocus() && event->key() == Qt::Key_Tab)
		{
			processed = true;
			event->accept();

			if (m->node()->arguments()->size() > 0)
			{
				QApplication::postEvent(target->scene(), new Interaction::SetCursorEvent(target,
						m->node()->arguments()->at(0), Interaction::SetCursorEvent::CursorOnLeft));
			}
			else
				createNewArgument(m, 0);
		}
		else if (m->arguments()->itemOrChildHasFocus()
				&& (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return))
		{
			processed = true;
			event->accept();

			int index = m->arguments()->layout()->focusedElementIndex();
			if (index == -1 && m->scene()->mainCursor() && m->scene()->mainCursor()->owner() == m->arguments()->layout())
				index = m->arguments()->layout()->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
			else index++;

			if (index >= 0) createNewArgument(m, index);

		}
	}

	if (!processed) GenericHandler::keyPressEvent(target, event);
}

void HMethod::createNewArgument(OOVisualization::VMethod* method, int position)
{
	auto arg = new OOModel::FormalArgument();
	auto empty = new OOModel::EmptyExpression();
	arg->setType(empty);
	method->node()->model()->beginModification(method->node(), "add new argument");
	method->node()->arguments()->insert(position, arg);
	method->node()->model()->endModification();

	method->arguments()->setUpdateNeeded();
	QApplication::postEvent(method->scene(), new Interaction::SetCursorEvent(method, arg->nameNode(),
			Interaction::SetCursorEvent::CursorOnLeft));
}

} /* namespace OOInteraction */
