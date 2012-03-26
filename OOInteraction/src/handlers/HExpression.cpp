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
 * HExpression.cpp
 *
 *  Created on: Feb 8, 2012
 *      Author: Dimitar Asenov
 */

#include "handlers/HExpression.h"

#include "string_offset_providers/StringOffsetProvider.h"
#include "expression_editor/OOExpressionBuilder.h"
#include "handlers/SetExpressionCursorEvent.h"

#include "OOModel/src/allOOModelNodes.h"

#include "InteractionBase/src/handlers/SetCursorEvent.h"
#include "ModelBase/src/adapter/AdapterManager.h"

namespace OOInteraction {

HExpression::HExpression()
{

}

HExpression* HExpression::instance()
{
	static HExpression h;
	return &h;
}

void HExpression::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	// TODO implement this better. It is supposed to only let typed characters through and igonre modifier keys.
	// However it does not work with e.g. ALTGR characters.
	if (event->text().isEmpty()
			|| event->key() == Qt::Key_Escape
			|| event->key() == Qt::Key_Tab
			|| (event->modifiers() != Qt::NoModifier && event->modifiers() != Qt::ShiftModifier)
			)
	{
		GenericHandler::keyPressEvent(target, event);
		return;
	}

	bool enterPressed = event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return;
	bool spacePressed = event->key() == Qt::Key_Space;

	// We need to trigger an update of all the visualizations leading up to the target, even though the target
	// visualization will probably be deleted and replaced with a new one.
	target->setUpdateNeeded();

	// Find the top most parent that is adaptable to StringProvider
	Visualization::Item* topMostItem = target;
	auto* topMostSP = Model::AdapterManager::adapt<StringOffsetProvider>(topMostItem);

	auto p = topMostItem->parentItem();
	while(p)
	{
		auto* adapted = Model::AdapterManager::adapt<StringOffsetProvider>(p);
		if (adapted)
		{
			SAFE_DELETE(topMostSP);
			topMostSP = adapted;
			topMostItem = static_cast<Visualization::Item*> (p);
		}
		p = p->parentItem();
	}

	QString str = topMostSP->string();
	int index = topMostSP->offset();
	SAFE_DELETE(topMostSP);

	QString newText = str;
	int newIndex = index;
	switch (event->key())
	{
		case Qt::Key_Delete:
		{
			if (index < str.size() ) newText.remove(index, 1);
		} break;
		case Qt::Key_Backspace:
		{
			if (index > 0 ) newText.remove(index-1, 1);
			--newIndex;
		} break;
		default:
		{
			if (!event->text().isEmpty())
			{
				newText.insert(index, event->text());
				newIndex += event->text().size();
			}
		} break;
	}

	OOModel::ExpressionStatement* replaceStatement = nullptr;
	if ( (enterPressed || spacePressed)
			&& (newText.startsWith("for") || newText.startsWith("foreach")|| newText.startsWith("if")
					|| newText.startsWith("continue") || newText.startsWith("break") || newText.startsWith("return")))
	{
		// Is this expression part of an expression statement
		auto e = dynamic_cast<OOModel::Expression*>(target->node());
		auto ep = e->parent();
		while (ep && !dynamic_cast<OOModel::Statement*>(ep)) ep = ep->parent();

		replaceStatement = dynamic_cast<OOModel::ExpressionStatement*>(ep);
	}

	if (replaceStatement)
	{
		OOModel::Statement* st = nullptr;
		Model::Node* toFocus = nullptr;
		if(newText.startsWith("for"))
		{
			auto loop =  new OOModel::LoopStatement();
			loop->setInitStep(new OOModel::EmptyExpression());

			toFocus = loop->initStep();
			st = loop;
		}
		else if (newText.startsWith("foreach"))
		{
			auto loop =  new OOModel::ForEachStatement();
			loop->setCollection(new OOModel::EmptyExpression());

			toFocus = loop->varNameNode();
			st = loop;
		}
		else if (newText.startsWith("if"))
		{
			auto ifs =  new OOModel::IfStatement();
			ifs->setCondition(new OOModel::EmptyExpression());

			toFocus = ifs->condition();
			st = ifs;
		}
		else if (newText.startsWith("continue"))
		{
			st = new OOModel::ContinueStatement();
			toFocus = st;
		}
		else if (newText.startsWith("break"))
		{
			st = new OOModel::BreakStatement();
			toFocus = st;
		}
		else if (newText.startsWith("return"))
		{
			auto ret =  new OOModel::ReturnStatement();
			ret->values()->append(new OOModel::EmptyExpression());

			toFocus = ret->values()->at(0);
			st = ret;
		}

		Model::Node* containerNode = replaceStatement->parent();
		containerNode->model()->beginModification(containerNode, "replace expression statement");
		containerNode->replaceChild(replaceStatement, st, false);
		containerNode->model()->endModification();

		auto parent = static_cast<Visualization::Item*> (topMostItem->parentItem());
		target->scene()->addPostEventAction(
				new Interaction::SetCursorEvent(parent, toFocus, Interaction::SetCursorEvent::CursorOnLeft));

		return;
	}
	else if (!enterPressed)
	{
		Model::Node* containerNode = topMostItem->node()->parent();
		containerNode->model()->beginModification(containerNode, "edit expression");
		OOModel::Expression* newExpression = OOExpressionBuilder::getOOExpression( newText );
		containerNode->replaceChild(topMostItem->node(), newExpression, false);
		containerNode->model()->endModification();

		auto parent = static_cast<Visualization::Item*> (topMostItem->parentItem());
		target->scene()->addPostEventAction( new SetExpressionCursorEvent(parent, newExpression, newIndex));
		return;
	}

	GenericHandler::keyPressEvent(target, event);
}

} /* namespace OOInteraction */
