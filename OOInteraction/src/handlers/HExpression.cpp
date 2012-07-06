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

#include "HExpression.h"
#include "../OOInteractionException.h"

#include "string_components/StringComponents.h"
#include "string_offset_providers/StringOffsetProvider.h"
#include "expression_editor/OOExpressionBuilder.h"
#include "handlers/SetExpressionCursorEvent.h"

#include "VisualizationBase/src/items/VList.h"
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
	// TODO implement this better. It is supposed to only let typed characters through and ignore modifier keys.
	// However it does not work with e.g. ALTGR characters.
	if ( event->key() != Qt::Key_Home && event->key() != Qt::Key_End && (
			   event->text().isEmpty()
			|| event->key() == Qt::Key_Escape
			|| event->key() == Qt::Key_Tab
			|| event->key() == Qt::Key_Semicolon
			|| (event->modifiers() != Qt::NoModifier && event->modifiers() != Qt::ShiftModifier))
			)
	{
		GenericHandler::keyPressEvent(target, event);
		return;
	}

	bool enterPressed = event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return;
	bool spacePressed = event->key() == Qt::Key_Space;

	try
	{
		// We need to trigger an update of all the visualizations leading up to the target, even though the target
		// visualization will probably be deleted and replaced with a new one.
		target->setUpdateNeeded(Visualization::Item::StandardUpdate);

		// Find the top most parent that is adaptable to StringProvider
		Visualization::Item* topMostItem = target;
		auto* topMostSP = Model::AdapterManager::adapt<StringOffsetProvider>(topMostItem);

		auto p = topMostItem->parent();
		while(p)
		{
			auto* adapted = Model::AdapterManager::adapt<StringOffsetProvider>(p);
			if (adapted)
			{
				SAFE_DELETE(topMostSP);
				topMostSP = adapted;
				topMostItem = p;
			}
			p = p->parent();
		}

		QString str = topMostSP->string();
		int index = topMostSP->offset( (Qt::Key) event->key() );
		SAFE_DELETE(topMostSP);

		if (index < 0) return;

		// Handle the HOME and END keys
		if (event->key() == Qt::Key_Home || event->key() == Qt::Key_End)
		{
			auto parent = topMostItem->parent();
			index = (event->key() == Qt::Key_Home ) ? 0 : str.length();
			target->scene()->addPostEventAction( new SetExpressionCursorEvent(parent, topMostItem->node(), index));
			return;
		}

		// Handle the removing of an empty line and moving up/down on BACKSPACE/DELETE
		if (topMostItem->node()->parent()
		      && ((event->key() == Qt::Key_Backspace && index == 0)
		            || (event->key() == Qt::Key_Delete && index == str.length())))
		{
			auto list = dynamic_cast<OOModel::StatementItemList*>(topMostItem->node()->parent()->parent());
			if (list)
			{
				int thisItemListIndex = list->indexOf(topMostItem->node()->parent());
				int itemToDeletelistIndex = thisItemListIndex + (event->key() == Qt::Key_Backspace ? -1 : +1);

				if (itemToDeletelistIndex >= 0 && itemToDeletelistIndex < list->size())
				{
					auto st = dynamic_cast<OOModel::ExpressionStatement*>(list->at(itemToDeletelistIndex));
					if (st && dynamic_cast<OOModel::EmptyExpression*>(st->expression()))
					{
						list->model()->beginModification(list, "remove empty line");
						list->remove(itemToDeletelistIndex, false);
						list->model()->endModification();

						target->scene()->addPostEventAction(
							new Interaction::SetCursorEvent(topMostItem->parent(), target->node(),
									(event->key() == Qt::Key_Backspace ?
											Interaction::SetCursorEvent::CursorOnLeft
											: Interaction::SetCursorEvent::CursorOnRight)));
						return;
					} else
					{
						// Get a parent which represents a list (of statements or statement items)
						auto parent = topMostItem->parent();
						Visualization::VList* vlist = nullptr;
						while (!(vlist = dynamic_cast<Visualization::VList*>(parent)) && parent->parent())
							parent = parent->parent();

						auto vis = vlist->findVisualizationOf(topMostItem->node()->parent());
						if (event->key() == Qt::Key_Backspace)
						{
							vlist->layout()->moveCursor(Visualization::Item::CursorMoveDirection::MoveUpOf,
								vis->pos().toPoint() + QPoint(vis->width() / 2, 0));

							if (dynamic_cast<OOModel::EmptyExpression*>(topMostItem->node()))
							{
								list->model()->beginModification(list, "remove empty line");
								list->remove(thisItemListIndex, false);
								list->model()->endModification();
							}
						}
						else
						{
							vlist->layout()->moveCursor(Visualization::Item::CursorMoveDirection::MoveDownOf,
								vis->pos().toPoint() + QPoint(vis->width() / 2, vis->height()-1));
						}

						return;
					}
				}
			}
		}


		// Modify the string, inserting the pressed key's text (or deleting text)
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
				if (index > 0 )
				{
					newText.remove(index-1, 1);
					--newIndex;
				}

			} break;
			default:
			{
				if (!enterPressed && !event->text().isEmpty())
				{
					newText.insert(index, event->text());
					newIndex += event->text().size();
				}
			} break;
		}

		// Insert a new line if enter is pressed at the boundary
		if (enterPressed && (index == 0 || index == newText.size()))
		{
			auto expSt = parentExpressionStatement(dynamic_cast<OOModel::Expression*>(target->node()));
			if (expSt)
			{
				auto stList = dynamic_cast<OOModel::StatementItemList*>(expSt->parent());
				if (stList)
				{
					auto es = new OOModel::ExpressionStatement(new OOModel::EmptyExpression());
					stList->model()->beginModification(stList, "add empty statement");
					stList->insert(stList->indexOf(expSt) + (index==0 && !str.isEmpty()?0:1), es);
					stList->model()->endModification();

					// Issue a cursor update
					if ( index == 0 && !str.isEmpty())
					{
						// For the current item
						target->scene()->addPostEventAction(
							new Interaction::SetCursorEvent(topMostItem->parent(), target->node(),
								Interaction::SetCursorEvent::CursorOnLeft));
					}
					else
					{
						// For the newly created line
						auto p = target;
						while (p->parent()) p = p->parent(); // Using topMostItem->parent() does not work.
						target->scene()->addPostEventAction(
							new Interaction::SetCursorEvent(p, es->expression(), Interaction::SetCursorEvent::CursorOnLeft));
					}
					return;
				}
			}
		}

		// Process keywords for statements
		OOModel::ExpressionStatement* replaceStatement = nullptr;
		if ( (enterPressed || spacePressed)
				&& (newText.startsWith("for") || newText.startsWith("foreach")|| newText.startsWith("if")
						|| newText.startsWith("continue") || newText.startsWith("break") || newText.startsWith("return")))
			replaceStatement = parentExpressionStatement(dynamic_cast<OOModel::Expression*>(target->node()));

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

			// Get a parent which represents a list (of statements or statement items)
			auto parent = topMostItem->parent();
			while (! dynamic_cast<Visualization::VList*>(parent) && parent->parent()) parent = parent->parent();

			target->scene()->addPostEventAction(
					new Interaction::SetCursorEvent(parent, toFocus, Interaction::SetCursorEvent::CursorOnLeft));
			return;
		}
		else if (!enterPressed)
		{
			OOModel::Expression* newExpression = OOExpressionBuilder::getOOExpression( newText );
			Model::Node* containerNode = topMostItem->node()->parent();
			containerNode->model()->beginModification(containerNode, "edit expression");
			containerNode->replaceChild(topMostItem->node(), newExpression, false);
			containerNode->model()->endModification();

			// Compute the new offset. This can change in case the string of the new expression is different.
			QString expString = StringComponents::stringForNode(newExpression);
			newIndex += expString.length() - newText.length();

			auto parent = topMostItem->parent();
			target->scene()->addPostEventAction( new SetExpressionCursorEvent(parent, newExpression, newIndex));

			return;
		}
	}
	catch(Envision::EnvisionException &e)
   {
		e.printError();
   }

	if (enterPressed) return;
	GenericHandler::keyPressEvent(target, event);
}

OOModel::ExpressionStatement* HExpression::parentExpressionStatement(OOModel::Expression* e)
{
	// Is this expression part of an expression statement
	auto ep = e->parent();
	while (ep && !dynamic_cast<OOModel::Statement*>(ep)) ep = ep->parent();

	return dynamic_cast<OOModel::ExpressionStatement*>(ep);
}

} /* namespace OOInteraction */
