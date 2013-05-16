/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "HExpression.h"
#include "../OOInteractionException.h"

#include "../expression_editor/operators/CompoundObjectDescriptor.h"
#include "string_offset_providers/StringComponents.h"
#include "string_offset_providers/StringOffsetProvider.h"
#include "expression_editor/OOExpressionBuilder.h"
#include "handlers/SetExpressionCursorEvent.h"

#include "VisualizationBase/src/cursor/Cursor.h"
#include "VisualizationBase/src/items/VList.h"
#include "OOModel/src/allOOModelNodes.h"
#include "OOModel/src/types/SymbolProviderType.h"

#include "InteractionBase/src/handlers/HList.h"
#include "InteractionBase/src/events/SetCursorEvent.h"
#include "InteractionBase/src/autocomplete/AutoComplete.h"
#include "InteractionBase/src/autocomplete/AutoCompleteEntry.h"
#include "Core/src/AdapterManager.h"

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
	auto key = event->key();
	auto modifiers = event->modifiers();

	if (modifiers == Qt::ControlModifier && key == Qt::Key_Space)
	{
		toggleAutoComplete(target);
		return;
	}

	bool enterPressed = key == Qt::Key_Enter || key == Qt::Key_Return;
	bool spacePressed = key == Qt::Key_Space;

	// TODO implement this better. It is supposed to only let typed characters through and ignore modifier keys.
	// However it does not work with e.g. ALTGR characters.
	if ( key != Qt::Key_Home && key != Qt::Key_End && (
			   event->text().isEmpty()
			|| key == Qt::Key_Escape
			|| key == Qt::Key_Tab
			|| key == Qt::Key_Semicolon
			|| (modifiers != Qt::NoModifier && modifiers != Qt::ShiftModifier)
			|| (modifiers == Qt::ShiftModifier && (enterPressed || spacePressed)))
			)
	{
		GenericHandler::keyPressEvent(target, event);
		return;
	}

	try
	{
		// We need to trigger an update of all the visualizations leading up to the target, even though the target
		// visualization will probably be deleted and replaced with a new one.
		target->setUpdateNeeded(Visualization::Item::StandardUpdate);

		// Find the top most parent that is adaptable to StringProvider
		QString str;
		int index;
		Visualization::Item* topMostItem = stringInfo(target, (Qt::Key) key, str, index);

		if (index < 0) return;

		// Handle the HOME and END keys
		if (key == Qt::Key_Home || key == Qt::Key_End)
		{
			auto parent = topMostItem->parent();
			index = (key == Qt::Key_Home ) ? 0 : str.length();
			target->scene()->addPostEventAction( new SetExpressionCursorEvent(parent, topMostItem->node(), index));
			return;
		}

		// Handle the removing of an empty line and moving up/down on BACKSPACE/DELETE
		if ((key == Qt::Key_Backspace && index == 0) || (key == Qt::Key_Delete && index == str.length()))
		{
			if (topMostItem->node()->parent()
					&& topMostItem->node()->parent()->typeId() == OOModel::ExpressionStatement::typeIdStatic())
			{
				if ( auto list = dynamic_cast<OOModel::StatementItemList*>(topMostItem->node()->parent()->parent()) )
				{
					int thisNodeListIndex = list->indexOf(topMostItem->node()->parent());
					int nodeToDeletelistIndex = thisNodeListIndex + (key == Qt::Key_Backspace ? -1 : +1);
					bool empty = dynamic_cast<OOModel::EmptyExpression*>(topMostItem->node());

					// Get a parent which represents a list (of statements or statement items)
					auto parent = topMostItem->parent();
					Visualization::VList* vlist = nullptr;
					while (!(vlist = dynamic_cast<Visualization::VList*>(parent)) && parent->parent())
						parent = parent->parent();

					if (nodeToDeletelistIndex >= 0 && nodeToDeletelistIndex < list->size())
					{
						// Delete the current or the previous or the next empty item
						auto st = dynamic_cast<OOModel::ExpressionStatement*>(list->at(nodeToDeletelistIndex));
						if (st && dynamic_cast<OOModel::EmptyExpression*>(st->expression()))
						{
							Interaction::HList::instance()->removeNodeAndSetCursor(vlist,
									empty ? thisNodeListIndex : nodeToDeletelistIndex, key == Qt::Key_Delete,
										key == Qt::Key_Delete
										? Interaction::SetCursorEvent::CursorOnRight : Interaction::SetCursorEvent::CursorOnLeft);
							return;
						}
					}

					// If we could not delete the previous/next statements, then delete the current one if it is empty.
					// In either case position the cursor appropriately
					if (empty)
						Interaction::HList::instance()->removeNodeAndSetCursor(vlist, thisNodeListIndex);
					else
						Interaction::HList::instance()->scheduleSetCursor(vlist, thisNodeListIndex +
								(key == Qt::Key_Delete ? 1 : 0));
					return;
				}
			}

			// If we reach this point then we do not know how do handle these keys so just send them to the parent
			GenericHandler::keyPressEvent(target, event);
			return;
		}


		// Modify the string, inserting the pressed key's text (or deleting text)
		QString newText = str;
		int newIndex = index;
		switch (key)
		{
			// Below we let CompoundObjectDescriptor process Delete and Backspace since it might need to remove
			// extra characters if those keys are pressed just on the boundary of a compound object
			case Qt::Key_Delete:
			{
				if (index < str.size() )
				{
					if (! CompoundObjectDescriptor::processDeleteOrBackspaceKey(Qt::Key_Delete, newText, newIndex))
						newText.remove(index, 1);
				}
			} break;
			case Qt::Key_Backspace:
			{
				if (index > 0 )
				{
					if (! CompoundObjectDescriptor::processDeleteOrBackspaceKey(Qt::Key_Backspace, newText, newIndex))
					{
						newText.remove(index-1, 1);
						--newIndex;
					}
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

		// Process keywords for statements
		OOModel::ExpressionStatement* replaceStatement = nullptr;
		auto trimmedText = newText.trimmed();
		if ( (enterPressed || spacePressed)
				&& (trimmedText == "for" || trimmedText == "foreach" || trimmedText == "if" || trimmedText == "class"
						|| trimmedText == "continue" || trimmedText == "break" || trimmedText == "return"))
			replaceStatement = parentExpressionStatement(dynamic_cast<OOModel::Expression*>(target->node()));

		if (replaceStatement)
		{
			OOModel::StatementItem* st = nullptr;
			Model::Node* toFocus = nullptr;
			if(trimmedText == "for")
			{
				auto loop =  new OOModel::LoopStatement();
				loop->setInitStep(new OOModel::EmptyExpression());

				toFocus = loop->initStep();
				st = loop;
			}
			else if (trimmedText == "foreach")
			{
				auto loop =  new OOModel::ForEachStatement();
				loop->setCollection(new OOModel::EmptyExpression());

				toFocus = loop->varNameNode();
				st = loop;
			}
			else if (trimmedText == "if")
			{
				auto ifs =  new OOModel::IfStatement();
				ifs->setCondition(new OOModel::EmptyExpression());

				toFocus = ifs->condition();
				st = ifs;
			}
			else if (trimmedText == "continue")
			{
				st = new OOModel::ContinueStatement();
				toFocus = st;
			}
			else if (trimmedText == "break")
			{
				st = new OOModel::BreakStatement();
				toFocus = st;
			}
			else if (trimmedText == "return")
			{
				auto ret =  new OOModel::ReturnStatement();
				ret->values()->append(new OOModel::EmptyExpression());

				toFocus = ret->values()->at(0);
				st = ret;
			}
			else if (trimmedText == "class")
			{
				auto ret =  new OOModel::Class();
				toFocus = ret;
				st = new OOModel::DeclarationStatement(ret);
			}

			Model::Node* containerNode = replaceStatement->parent();
			containerNode->model()->beginModification(containerNode, "replace expression statement");
			containerNode->replaceChild(replaceStatement, st);
			containerNode->model()->endModification();

			// Get a parent which represents a list (of statements or statement items)
			auto parent = topMostItem->parent();
			while (! dynamic_cast<Visualization::VList*>(parent) && parent->parent()) parent = parent->parent();

			target->scene()->addPostEventAction(new Interaction::SetCursorEvent(parent, toFocus));
			return;
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

		if (!enterPressed)
		{
			setNewExpression(target, topMostItem, newText, newIndex);

			// Trigger auto complete
			auto cutText = newText.left(newIndex);
			if (cutText.endsWith('.')
					|| (cutText.size() > 0 && cutText.at(cutText.size()-1).isLetterOrNumber())
				)
			{
				auto s = target->scene();
				s->addPostEventAction(new Visualization::CustomSceneEvent(
						[s, this]() {
							auto mc = s->mainCursor();
							if (mc) // This will be null if 'target' was deleted, e.g. because the new expression
								// else whereis visualized
							{
								auto o = mc->owner();
								while (o && o->handler() != this) o = o->parent();
								if (o) showAutoComplete(o);
							}
						}));
			}
			else
			{
				target->scene()->addPostEventAction(new Visualization::CustomSceneEvent(Interaction::AutoComplete::hide));
			}

			return;
		}
	}
	catch(Core::EnvisionException &e)
   {
		e.printError();
   }

	GenericHandler::keyPressEvent(target, event);
}

Visualization::Item* HExpression::stringInfo(Visualization::Item* target, Qt::Key key, QString& str, int& index)
{
	Visualization::Item* topMostItem = target;
	auto* topMostSP = Core::AdapterManager::adapt<StringOffsetProvider>(topMostItem);

	auto p = topMostItem->parent();
	while(p)
	{
		auto* adapted = Core::AdapterManager::adapt<StringOffsetProvider>(p);
		if (adapted)
		{
			SAFE_DELETE(topMostSP);
			topMostSP = adapted;
			topMostItem = p;
		}
		p = p->parent();
	}

	str = topMostSP->string();
	index = topMostSP->offset( key );
	SAFE_DELETE(topMostSP);

	return topMostItem;
}

OOModel::ExpressionStatement* HExpression::parentExpressionStatement(OOModel::Expression* e)
{
	// Is this expression part of an expression statement
	auto ep = e->parent();
	while (ep && !dynamic_cast<OOModel::Statement*>(ep)) ep = ep->parent();

	return dynamic_cast<OOModel::ExpressionStatement*>(ep);
}

void HExpression::setNewExpression(Visualization::Item* target, Visualization::Item* topMostItem, const QString& text,
				int index)
{
	OOModel::Expression* newExpression = OOExpressionBuilder::getOOExpression( text );

	Model::Node* containerNode = topMostItem->node()->parent();
	containerNode->model()->beginModification(containerNode, "edit expression");
	containerNode->replaceChild(topMostItem->node(), newExpression);
	containerNode->model()->endModification();

	// Compute the new offset. This can change in case the string of the new expression is different.
	QString expString = StringComponents::stringForNode(newExpression);
	index += expString.length() - text.length();

	// Let interested plug-ins know that an expression was edited. Plug-ins might decide to make changes.
	for (auto monitor : expressionMonitors_) monitor(newExpression, index);

	// Find an item that represents a node, as any intermediate items might disappear when during the update.
	// E.g. VLoopStatement keeps it's condition inside a wrapper for background color that also get deleted during the
	// update.
	auto parent = topMostItem->parent();
	while (!parent->node() && parent->parent()) parent=parent->parent();

	target->scene()->addPostEventAction( new SetExpressionCursorEvent(parent, newExpression, index));

	// Clear any compound objects that were stored during this expression processing
	// TODO: This is not always what we want. For example if we type a quote when trying to make a new string argument
	// that appears before a lambda, the lambda will be converted to a string but will not be immediately converted back
	// to a lambda. If we clean all stored compound object, we won't be able to restore the lambda, once the closing
	// quote is typed.
	CompoundObjectDescriptor::cleanAllStoredExpressions();
}

void HExpression::showAutoComplete(Visualization::Item* target)
{
	// Make a string pattern to look for. Given an input like:
	// someclass.met|hod
	// The pattern will look this this:
	// *m*e*t*
	// Everything after the cursor is discarded. The identifier characters before the cursor are used and each character
	// is surrounded with *
	QString str;
	int index;
	stringInfo(target, Qt::Key_A, str, index); //Any non special key

	str = str.left(index);
	index = str.size() - 1;
	while (index >= 0)
	{
		if (str.at(index).isLetterOrNumber() || str.at(index) == '_') --index;
		else break;
	}
	++index;

	str = str.right(str.size() - index);
	for(int i = str.size(); i>=0; --i) str.insert(i, "*");

	QList<Interaction::AutoCompleteEntry*> entries;

	OOModel::SymbolProviderType* scopePrefix = nullptr;
	bool afterDot = false;

	if (auto ref = dynamic_cast<OOModel::ReferenceExpression*>(target->node()))
	{
		// If the auto complete is invoked somewhere in a reference expression after a '.' only look for members that
		// match.
		if (ref->prefix())
		{
			afterDot = true;
			auto t = ref->prefix()->type();
			scopePrefix = dynamic_cast<OOModel::SymbolProviderType*>(t);
			if (!scopePrefix) SAFE_DELETE(t);
		}
	}
	else if (auto unf = dynamic_cast<OOModel::UnfinishedOperator*>(target->node()))
	{
		// If the auto complete is invoked just after a '.' only look for members that match within the scope of the
		// prefix.
		if (unf->delimiters()->size() == 2 && unf->delimiters()->at(1)->get() == ".")
		{
			afterDot = true;
			auto t = unf->operands()->first()->type();
			scopePrefix = dynamic_cast<OOModel::SymbolProviderType*>(t);
			if (!scopePrefix) SAFE_DELETE(t);
		}
	}

	auto searchNode = scopePrefix ? scopePrefix->symbolProvider() : target->node();

	for(auto n : searchNode->findSymbols(QRegExp(str, Qt::CaseInsensitive, QRegExp::Wildcard),
		target->node(), (afterDot ? Model::Node::SEARCH_DOWN : Model::Node::SEARCH_UP), afterDot == false))
			entries.append(new Interaction::AutoCompleteEntry(n->symbolName(), QString(), nullptr,
				[=](Interaction::AutoCompleteEntry* entry) { doAutoComplete(target, entry->text()); }));

	SAFE_DELETE(scopePrefix);

	Interaction::AutoComplete::show(entries);
}

void HExpression::toggleAutoComplete(Visualization::Item* target)
{
	if (Interaction::AutoComplete::isVisible())
		Interaction::AutoComplete::hide();
	else showAutoComplete(target);
}

void HExpression::doAutoComplete(Visualization::Item* target, const QString& autoCompleteStr)
{
	// We need to trigger an update of all the visualizations leading up to the target, even though the target
	// visualization will probably be deleted and replaced with a new one.
	target->setUpdateNeeded(Visualization::Item::StandardUpdate);

	QString str;
	int index;
	Visualization::Item* topMostItem = stringInfo(target, Qt::Key_A, str, index); //Any non special key

	int startIndex = index - 1;
	while (startIndex >= 0)
	{
		if (str.at(startIndex).isLetterOrNumber() || str.at(startIndex) == '_') --startIndex;
		else break;
	}
	++startIndex;

	int endIndex = index - 1;
	while (endIndex >= 0 && endIndex < str.size())
	{
		if (str.at(endIndex).isLetterOrNumber() || str.at(endIndex) == '_') ++endIndex;
		else break;
	}
	--endIndex;

	// The part between endIndex and startIndex will be replaced by autoCompleteStr

	int oldIdentifierSize = endIndex - startIndex + 1;
	if (oldIdentifierSize < 0) oldIdentifierSize = 0;

	// Modify the string, inserting the pressed key's text (or deleting text)
	QString newText = str;
	newText.replace(startIndex, oldIdentifierSize, autoCompleteStr);
	int newIndex = index + autoCompleteStr.size() - oldIdentifierSize;

	setNewExpression(target, topMostItem, newText, newIndex);
}

} /* namespace OOInteraction */
