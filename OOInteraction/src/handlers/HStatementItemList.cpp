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
 * HStatementItemList.cpp
 *
 *  Created on: Jun 5, 2012
 *      Author: Dimitar Asenov
 */

#include "HStatementItemList.h"

#include "OOVisualization/src/elements/VStatementItemList.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "OOModel/src/statements/ExpressionStatement.h"

#include "InteractionBase/src/handlers/SetCursorEvent.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"

namespace OOInteraction {

HStatementItemList::HStatementItemList()
{}

HStatementItemList* HStatementItemList::instance()
{
	static HStatementItemList h;
	return &h;
}

void HStatementItemList::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	auto vis = dynamic_cast<OOVisualization::VStatementItemList*> ( target );
	bool processed = false;

	if (event->modifiers() == Qt::NoModifier && (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return))
	{
		int index = -1;
		if (vis->scene()->mainCursor() && vis->scene()->mainCursor()->owner() == vis->layout())
			index = vis->layout()->correspondingSceneCursor<Visualization::LayoutCursor>()->index();
		else
		{
			index = vis->focusedElementIndex();
			if (index >= 0) ++index;
		}

		if (index >= 0)
		{
			processed = true;
			event->accept();

			auto es = new OOModel::ExpressionStatement(new OOModel::EmptyExpression());
			vis->node()->model()->beginModification(vis->node(), "add empty statement");
			vis->node()->insert(index, es);
			vis->node()->model()->endModification();

			vis->setUpdateNeeded(Visualization::Item::StandardUpdate);
				target->scene()->addPostEventAction( new Interaction::SetCursorEvent(vis, es,
				Interaction::SetCursorEvent::CursorOnLeft));
		}


	}

	if (!processed) HList::keyPressEvent(target, event);
}

} /* namespace OOInteraction */
