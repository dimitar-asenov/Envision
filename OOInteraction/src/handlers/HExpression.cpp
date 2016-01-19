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

#include "HExpression.h"
#include "../OOInteractionException.h"

#include "../expression_editor/CompoundObjectDescriptor.h"
#include "../string_offset_providers/StringComponents.h"
#include "../string_offset_providers/StringOffsetProvider.h"
#include "../string_offset_providers/CompoundObjectStringOffsetProvider.h"
#include "../expression_editor/OOExpressionBuilder.h"
#include "SetExpressionCursorEvent.h"

#include "VisualizationBase/src/cursor/Cursor.h"
#include "VisualizationBase/src/items/VList.h"
#include "OOModel/src/allOOModelNodes.h"
#include "OOModel/src/types/SymbolProviderType.h"

#include "InteractionBase/src/handlers/HList.h"
#include "InteractionBase/src/events/SetCursorEvent.h"
#include "InteractionBase/src/autocomplete/AutoComplete.h"
#include "InteractionBase/src/autocomplete/AutoCompleteEntry.h"
#include "Core/src/AdapterManager.h"

using namespace OOModel;
using namespace Visualization;
using namespace Interaction;

namespace OOInteraction {

HExpression* HExpression::instance()
{
	static HExpression h;
	return &h;
}

void HExpression::keyPressEvent(Item *target, QKeyEvent *event)
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
		target->setUpdateNeeded(Item::StandardUpdate);

		// Find the top most parent that is adaptable to StringProvider
		QString str;
		int index;
		Item* topMostItem = stringInfo(target, (Qt::Key) key, str, index);

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
					&& topMostItem->node()->parent()->typeId() == ExpressionStatement::typeIdStatic())
			{
				if ( auto list = DCast<StatementItemList>(topMostItem->node()->parent()->parent()) )
				{
					int thisNodeListIndex = list->indexOf(topMostItem->node()->parent());
					int nodeToDeletelistIndex = thisNodeListIndex + (key == Qt::Key_Backspace ? -1 : +1);
					bool empty = DCast<EmptyExpression>(topMostItem->node());

					// Get a parent which represents a list (of statements or statement items)
					auto parent = topMostItem->parent();
					VList* vlist = nullptr;
					while (!(vlist = DCast<VList>(parent)) && parent->parent())
						parent = parent->parent();

					if (nodeToDeletelistIndex >= 0 && nodeToDeletelistIndex < list->size())
					{
						// Delete the current or the previous or the next empty item
						auto st = DCast<ExpressionStatement>(list->at(nodeToDeletelistIndex));
						if (st && DCast<EmptyExpression>(st->expression()))
						{
							HList::instance()->removeNodeAndSetCursor(vlist,
									empty ? thisNodeListIndex : nodeToDeletelistIndex, key == Qt::Key_Delete,
										key == Qt::Key_Delete
										? SetCursorEvent::CursorOnRight : SetCursorEvent::CursorOnLeft);
							return;
						}
					}

					// If we could not delete the previous/next statements, then delete the current one if it is empty.
					// In either case position the cursor appropriately
					if (empty)
						HList::instance()->removeNodeAndSetCursor(vlist, thisNodeListIndex);
					else
						HList::instance()->scheduleSetCursor(vlist, thisNodeListIndex +
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

		// Surround statements with an if
		if (newText == "}")
		{
			if (auto parentStatement = parentExpressionStatement(DCast<OOModel::Expression>(target->node())))
			{
				if (auto list = DCast<OOModel::StatementItemList>(parentStatement->parent()))
				{
					// Scan the list for an if marker
					for (int i = 0; i<list->size(); ++i)
					{
						if (list->at(i) == parentStatement) break;

						if (auto es = DCast<ExpressionStatement>(list->at(i)))
						{
							if (StringComponents::stringForNode(es->expression()) == "if{")
							{
								StatementItem* st = nullptr;
								Model::Node* toFocus = nullptr;

								auto ifs =  new IfStatement{};
								ifs->setCondition(new EmptyExpression{});

								toFocus = ifs->condition();
								st = ifs;

								list->beginModification("surround statements with if");
								list->replaceChild(es, st);
								int middleElement = i+1;
								while (middleElement < list->size())
								{
									auto statement = list->at(middleElement);
									if (statement == parentStatement) break;
									list->remove(middleElement);
									ifs->thenBranch()->append(statement);
								}
								list->remove(parentStatement);
								list->endModification();

								// Get a parent which represents a list (of statements or statement items)
								auto parent = topMostItem->parent();
								while (! DCast<VList>(parent) && parent->parent()) parent = parent->parent();

								target->scene()->addPostEventAction(new SetCursorEvent{parent, toFocus});
								return;
							}
						}
					}
				}
			}
		}

		// Process keywords for statements
		ExpressionStatement* replaceStatement = nullptr;
		auto trimmedText = newText.trimmed();
		if ( (enterPressed || spacePressed)
				&& (trimmedText == "for" || trimmedText == "foreach" || trimmedText == "if" || trimmedText == "class"
						|| trimmedText == "continue" || trimmedText == "break" || trimmedText == "return" ||
						trimmedText == "do" || trimmedText == "//" || trimmedText == "switch" || trimmedText == "case"
						|| trimmedText == "try" || trimmedText == "assert"|| trimmedText == "synchronized"))
			replaceStatement = parentExpressionStatement(DCast<OOModel::Expression>(target->node()));

		if (replaceStatement)
		{
			StatementItem* st = nullptr;
			Model::Node* toFocus = nullptr;
			if (trimmedText == "for")
			{
				auto loop =  new LoopStatement{};
				loop->setInitStep(new EmptyExpression{});

				toFocus = loop->initStep();
				st = loop;
			}
			else if (trimmedText == "foreach")
			{
				auto loop =  new ForEachStatement{};
				loop->setCollection(new EmptyExpression{});

				toFocus = loop->varNameNode();
				st = loop;
			}
			else if (trimmedText == "if")
			{
				auto ifs =  new IfStatement{};
				ifs->setCondition(new EmptyExpression{});

				toFocus = ifs->condition();
				st = ifs;
			}
			else if (trimmedText == "continue")
			{
				st = new ContinueStatement{};
				toFocus = st;
			}
			else if (trimmedText == "break")
			{
				st = new BreakStatement{};
				toFocus = st;
			}
			else if (trimmedText == "return")
			{
				auto ret =  new ReturnStatement{};
				ret->values()->append(new EmptyExpression{});

				toFocus = ret->values()->at(0);
				st = ret;
			}
			else if (trimmedText == "class")
			{
				auto ret =  new Class{};
				toFocus = ret;
				st = new DeclarationStatement{ret};
			}
			else if (trimmedText == "do")
			{
				auto loop =  new LoopStatement{LoopStatement::LoopKind::PostCheck};
				auto empty = new EmptyExpression{};
				loop->body()->append(new ExpressionStatement{empty});

				toFocus = empty;
				st = loop;
			}
			else if (trimmedText == "//")
			{
				auto comment = new CommentStatementItem{};
				auto line = comment->commentNode()->lines()->createDefaultElement();
				comment->commentNode()->lines()->append(line);
				toFocus = line;
				st = comment;
			}
			else if (trimmedText == "switch")
			{
				auto switchs =  new SwitchStatement{};
				switchs->setSwitchExpression(new EmptyExpression{});

				toFocus = switchs->switchExpression();
				st = switchs;
			}
			else if (trimmedText == "case")
			{
				auto cases =  new CaseStatement{};
				cases->setCaseExpression(new EmptyExpression{});

				toFocus = cases->caseExpression();
				st = cases;
			}
			else if (trimmedText == "try")
			{
				auto trys =  new TryCatchFinallyStatement{};
				auto empty = new EmptyExpression{};
				trys->tryBody()->append(new ExpressionStatement{empty});

				toFocus = empty;
				st = trys;
			}
			else if (trimmedText == "assert")
			{
				auto asserts =  new AssertStatement{};
				asserts->setExpression(new EmptyExpression{});

				toFocus = asserts->expression();
				st = asserts;
			}
			else if (trimmedText == "synchronized")
			{
				auto sync =  new SynchronizedStatement{};
				sync->setExpression(new EmptyExpression{});

				toFocus = sync->expression();
				st = sync;
			}

			Model::Node* containerNode = replaceStatement->parent();
			containerNode->manager()->beginModification(containerNode, "replace expression statement");
			containerNode->replaceChild(replaceStatement, st);
			containerNode->manager()->endModification();

			// Get a parent which represents a list (of statements or statement items)
			auto parent = topMostItem->parent();
			while (! DCast<VList>(parent) && parent->parent()) parent = parent->parent();

			target->scene()->addPostEventAction(new SetCursorEvent{parent, toFocus});
			return;
		}

		// Insert a new line if enter is pressed at the boundary
		if (enterPressed && (index == 0 || index == newText.size()))
		{
			auto expSt = parentExpressionStatement(DCast<OOModel::Expression>(target->node()));
			if (expSt)
			{
				auto stList = DCast<StatementItemList>(expSt->parent());
				if (stList)
				{
					auto es = new ExpressionStatement(new EmptyExpression());
					stList->beginModification("add empty statement");
					stList->insert(stList->indexOf(expSt) + (index==0 && !str.isEmpty()?0:1), es);
					stList->endModification();

					// Issue a cursor update
					if ( index == 0 && !str.isEmpty())
					{
						// For the current item
						target->scene()->addPostEventAction(
							new SetCursorEvent(topMostItem->parent(), target->node(),
								SetCursorEvent::CursorOnLeft));
					}
					else
					{
						// For the newly created line
						auto p = target;
						while (p->parent()) p = p->parent(); // Using topMostItem->parent() does not work.
						target->scene()->addPostEventAction(
							new SetCursorEvent(p, es->expression(), SetCursorEvent::CursorOnLeft));
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
				s->addPostEventAction(new CustomSceneEvent(
						[s, this]() {
							auto mc = s->mainCursor();
							if (mc) // This will be null if 'target' was deleted, e.g. because the new expression
								// else whereis visualized
							{
								auto o = mc->owner();
								while (o && o->handler() != this) o = o->parent();
								if (o) showAutoComplete(o, false, false);
							}
						}));
			}
			else
			{
				target->scene()->addPostEventAction(new CustomSceneEvent{AutoComplete::hide});
			}

			return;
		}
	}
	catch (Core::EnvisionException &e)
	{
		e.printError();
	}

	GenericHandler::keyPressEvent(target, event);
}

Item* HExpression::stringInfo(Item* target, Qt::Key key, QString& str, int& index)
{
	Item* topMostItem = target;
	auto* topMostSP = Core::AdapterManager::adapt<StringOffsetProvider>(topMostItem);

	auto p = topMostItem->parent();
	while (p)
	{
		auto* adapted = Core::AdapterManager::adapt<StringOffsetProvider>(p);
		if (adapted)
		{
			// If we reach a compound object do not search past it.
			if (dynamic_cast<CompoundObjectStringOffsetProvider*>(adapted)) break;

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

ExpressionStatement* HExpression::parentExpressionStatement(OOModel::Expression* e)
{
	// Is this expression part of an expression statement
	auto ep = e->parent();
	while (ep && !DCast<Statement>(ep)) ep = ep->parent();

	return DCast<ExpressionStatement>(ep);
}

void HExpression::setNewExpression(Item* target, Item* topMostItem, const QString& text,
				int index)
{
	OOModel::Expression* newExpression = OOExpressionBuilder::getOOExpression( text );

	Model::Node* containerNode = topMostItem->node()->parent();
	containerNode->manager()->beginModification(containerNode, "edit expression");
	containerNode->replaceChild(topMostItem->node(), newExpression);
	containerNode->manager()->endModification();

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

	target->scene()->addPostEventAction( new SetExpressionCursorEvent{parent, newExpression, index});

	// Clear any compound objects that were stored during this expression processing
	// TODO: This is not always what we want. For example if we type a quote when trying to make a new string argument
	// that appears before a lambda, the lambda will be converted to a string but will not be immediately converted back
	// to a lambda. If we clean all stored compound object, we won't be able to restore the lambda, once the closing
	// quote is typed.
	CompoundObjectDescriptor::cleanAllStoredExpressions();
}

void HExpression::showAutoComplete(Item* target, bool showIfEmpty, bool showIfPreciselyMatched)
{
	// Make a string pattern to look for. Given an input like:
	// someclass.met|hod
	// The pattern will look this this:
	// m*e*t*   (Note that the first letter has to match)
	// Everything after the cursor is discarded. The identifier characters before the cursor are used and each character
	// is surrounded with *
	QString userWord;
	int index;
	stringInfo(target, Qt::Key_A, userWord, index); //Any non special key

	userWord = userWord.left(index);
	index = userWord.size() - 1;
	while (index >= 0)
	{
		if (userWord.at(index).isLetterOrNumber() || userWord.at(index) == '_') --index;
		else break;
	}
	++index;

	userWord = userWord.right(userWord.size() - index);
	if (!userWord.isEmpty() && userWord.at(0).isDigit()) // No auto completion for digits
	{
		AutoComplete::hide();
		return;
	}

	QString searchPattern = userWord;
	for (int i = searchPattern.size(); i>0; --i) searchPattern.insert(i, "*");

	QStringList autoCompleteCandidates;
	// Try to match built in keywords and types
	{
		auto matcher = QRegExp(searchPattern, Qt::CaseInsensitive, QRegExp::Wildcard);
		for (QString str : {"int", "bool", "long", "float", "double", "short", "byte", "char", "for", "if", "while",
									"do", "switch", "case", "try", "assert", "foreach", "continue", "break", "return",
									"synchronized"})
			if (matcher.exactMatch(str))
				autoCompleteCandidates.append(str);
	}


	SymbolProviderType* scopePrefix = nullptr;
	bool afterDot = false;

	if (auto ref = DCast<ReferenceExpression>(target->node()))
	{
		// If the auto complete is invoked somewhere in a reference expression after a '.' only look for members that
		// match.
		if (ref->prefix())
		{
			afterDot = true;
			auto t = ref->prefix()->type();
			scopePrefix = dynamic_cast<SymbolProviderType*>(t);
			if (!scopePrefix) SAFE_DELETE(t);
		}
	}
	else if (auto unf = DCast<OOModel::UnfinishedOperator>(target->node()))
	{
		// If the auto complete is invoked just after a '.' only look for members that match within the scope of the
		// prefix.
		if (unf->delimiters()->size() == 2 && unf->delimiters()->at(1)->get() == ".")
		{
			afterDot = true;
			auto t = unf->operands()->first()->type();
			scopePrefix = dynamic_cast<SymbolProviderType*>(t);
			if (!scopePrefix) SAFE_DELETE(t);
		}
	}

	auto searchNode = scopePrefix ? scopePrefix->symbolProvider() : target->node();

	QSet<Model::Node*> foundSymbols;

	Model::SymbolMatcher matcher{new QRegExp{searchPattern, Qt::CaseInsensitive, QRegExp::Wildcard}};
	if (afterDot)
		searchNode->findSymbols(foundSymbols, matcher, searchNode, Model::Node::SEARCH_DOWN,
										Model::Node::ANY_SYMBOL, true);
	else
		searchNode->findSymbols(foundSymbols, matcher, target->node(), Model::Node::SEARCH_UP,
										Model::Node::ANY_SYMBOL, true);

	searchNode->findSymbols(foundSymbols, new QRegExp{searchPattern, Qt::CaseInsensitive, QRegExp::Wildcard},
			target->node(), (afterDot ? Model::Node::SEARCH_DOWN : Model::Node::SEARCH_UP), Model::Node::ANY_SYMBOL,
			afterDot == false);

	for (auto n : foundSymbols) autoCompleteCandidates.append(n->symbolName());

	QList<AutoCompleteEntry*> entries;
	// Insert in sorted order, skipping duplicates.
	for (auto candidate : autoCompleteCandidates)
	{
		auto insertionPoint = std::find_if(entries.begin(), entries.end(),
				[=](AutoCompleteEntry* e){return e->text() >= candidate;});

		if ( insertionPoint == entries.end() || (*insertionPoint)->text() != candidate)
		{
			entries.insert(insertionPoint, new AutoCompleteEntry(candidate, QString(), nullptr,
				[=](AutoCompleteEntry* entry) { doAutoComplete(target, entry->text()); }));
		}
	}

	SAFE_DELETE(scopePrefix);

	bool show = true;
	if (!showIfEmpty && entries.isEmpty()) show = false;
	if (show && !showIfPreciselyMatched)
	{
		for (auto e : entries)
			if (e->text() == userWord)
			{
				show = false;
				break;
			}
	}

	if (show) AutoComplete::show(entries);
	else AutoComplete::hide();
}

void HExpression::toggleAutoComplete(Item* target)
{
	if (AutoComplete::isVisible())
		AutoComplete::hide();
	else showAutoComplete(target);
}

void HExpression::doAutoComplete(Item* target, const QString& autoCompleteStr)
{
	// We need to trigger an update of all the visualizations leading up to the target, even though the target
	// visualization will probably be deleted and replaced with a new one.
	target->setUpdateNeeded(Item::StandardUpdate);

	QString str;
	int index;
	Item* topMostItem = stringInfo(target, Qt::Key_A, str, index); //Any non special key

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

}
