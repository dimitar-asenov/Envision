/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "HQuery.h"

#include "nodes/QueryNode.h"
#include "nodes/QueryNodeContainer.h"
#include "nodes/CommandNode.h"
#include "nodes/CommandArgument.h"
#include "nodes/CompositeQueryNode.h"
#include "nodes/OperatorQueryNode.h"

#include "parsing/QueryParser.h"

#include "visualization/VCommandNode.h"
#include "visualization/VCommandArgument.h"
#include "visualization/VCompositeQueryNode.h"

#include "OOInteraction/src/string_offset_providers/StringComponents.h"
#include "OOInteraction/src/string_offset_providers/StringOffsetProvider.h"

#include "Core/src/AdapterManager.h"

#include "OOInteraction/src/string_offset_providers/GridBasedOffsetProvider.h"
#include "OOInteraction/src/string_offset_providers/Cell.h"
#include "OOInteraction/src/string_offset_providers/ListCell.h"
#include "OOInteraction/src/handlers/SetExpressionCursorEvent.h"

namespace InformationScripting {

HQuery* HQuery::instance()
{
	static HQuery instance;
	return &instance;
}

void HQuery::initStringComponents()
{
	using namespace OOInteraction;

	StringComponents::add<CommandNode>([](CommandNode* command) {
		return StringComponents::c(command->name(),
			StringComponents::list(command->arguments(), " ", " ", "", true, true));
	});
	StringComponents::add<CommandArgument>([](CommandArgument* argument) {
		return StringComponents::c(argument->argument());
	});
	StringComponents::add<CompositeQueryNode>([](CompositeQueryNode* composite) {
		return StringComponents::c(StringComponents::list(composite->queries(),
																		  QueryParser::LIST_LEFT,
																		  QueryParser::LIST_DELIM,
																		  QueryParser::LIST_RIGHT, true, true));
	});

	StringComponents::add<OperatorQueryNode>([](OperatorQueryNode* opNode) {
		return StringComponents::c(opNode->left(),
			StringComponents::choose(opNode->op(),
								OperatorQueryNode::OperatorTypes::Pipe, "|",
								OperatorQueryNode::OperatorTypes::Substract, "|-"),
					opNode->right());
	});

	GridBasedOffsetProvider::addGridConstructor<VCommandNode>(
	[](GridBasedOffsetProvider* grid, VCommandNode* vis){
		grid->add(new Cell{0, vis->name(), 0});
		if ( !vis->node()->arguments()->isEmpty())
			grid->add(new ListCell{1, vis->arguments(), 1, " ", " ", ""});
	});

	GridBasedOffsetProvider::addGridConstructor<VCompositeQueryNode>(
	[](GridBasedOffsetProvider* grid, VCompositeQueryNode* vis){
		grid->add(new ListCell{0, vis->queries(), 0,
									  QueryParser::LIST_LEFT,
									  QueryParser::LIST_DELIM,
									  QueryParser::LIST_RIGHT});
	});

	GridBasedOffsetProvider::addGridConstructor<VCommandArgument>(
	[](GridBasedOffsetProvider* grid, VCommandArgument* vis){
		grid->add(new Cell{0, vis->argument(), 0});
	});
}

void HQuery::keyPressEvent(Visualization::Item* target, QKeyEvent* event)
{
	auto key = static_cast<Qt::Key>(event->key());
	if (key == Qt::Key_Right || key == Qt::Key_Left || key == Qt::Key_Up || key == Qt::Key_Down ||
		 (event->modifiers() != Qt::ControlModifier && (key == Qt::Key_Enter || key == Qt::Key_Return)))
		return GenericHandler::keyPressEvent(target, event);

	QString str;
	int index;
	auto topMostItem = stringInfo(target, (Qt::Key) key, str, index);
	QString newText = str;
	int newIndex = index;
	removeListsWithOneElement(newText, newIndex);

	bool changed = false;
	if (event->modifiers() == Qt::NoModifier && (key == Qt::Key_Delete || key == Qt::Key_Backspace))
		changed = processDeleteOrBackspace(key, newText, newIndex);
	else if (event->modifiers() == Qt::ControlModifier && (key == Qt::Key_Enter || key == Qt::Key_Return))
	{
		changed = true;
		newIndex = processEnter(newText, index);
	}
	else if (event->matches(QKeySequence::Copy) && !target->ignoresCopyAndPaste())
	{
		QApplication::clipboard()->setText(newText);
	}
	else if (event->matches(QKeySequence::Paste) && !target->ignoresCopyAndPaste())
	{
		auto clipboardText = QApplication::clipboard()->text();
		newText.insert(newIndex, clipboardText);
		newIndex += clipboardText.size();
		changed = !clipboardText.isEmpty();
	}
	else if (!event->text().isEmpty())
	{
		changed = true;
		newText.insert(index, event->text());
		newIndex += event->text().size();
	}

	if (changed)
	{
		target->setUpdateNeeded(Visualization::Item::StandardUpdate);
		setNewQuery(target, topMostItem, newText, newIndex);
	}
}

QueryNodeContainer* HQuery::parentContainer(InformationScripting::QueryNode* e)
{
	// Is this expression part of an expression statement
	auto ep = e->parent();
	while (ep && !DCast<QueryNodeContainer>(ep)) ep = ep->parent();

	return DCast<QueryNodeContainer>(ep);
}

Visualization::Item* HQuery::stringInfo(Visualization::Item* target, Qt::Key key, QString& str, int& index)
{
	auto topMostItem = target;
	auto* topMostSP = Core::AdapterManager::adapt<OOInteraction::StringOffsetProvider>(topMostItem);

	auto p = topMostItem->parent();
	while (p)
	{
		auto* adapted = Core::AdapterManager::adapt<OOInteraction::StringOffsetProvider>(p);
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

void HQuery::setNewQuery(Visualization::Item* target, Visualization::Item* topMostItem, const QString& text, int index)
{
	QString newText = text;
	QueryNode* newQuery = QueryParser::parse(newText);

	Model::Node* containerNode = topMostItem->node()->parent();
	Q_ASSERT(containerNode);
	containerNode->replaceChild(topMostItem->node(), newQuery);

	// Compute the new offset. This can change in case the string of the new expression is different.
	QString expString = OOInteraction::StringComponents::stringForNode(newQuery);
	index += expString.length() - newText.length();

	// Find an item that represents a node, as any intermediate items might disappear when during the update.
	// E.g. VLoopStatement keeps it's condition inside a wrapper for background color that also get deleted during the
	// update.
	auto parent = topMostItem->parent();
	while (!parent->node() && parent->parent()) parent=parent->parent();

	target->scene()->addPostEventAction(new OOInteraction::SetExpressionCursorEvent{parent, newQuery, index});
}

bool HQuery::processDeleteOrBackspace(Qt::Key key, QString& exp, int& index)
{
	int indexToRemove = (key == Qt::Key_Delete ? index : index - 1);
	if (canBeRemoved(exp, indexToRemove))
	{
		exp.remove(indexToRemove, 1);
		if (key == Qt::Key_Backspace) --index;
		return true;
	}

	return false;
}

bool HQuery::canBeRemoved(const QString& exp, int index)
{
	if (index >=0 && index < exp.length()
		 && exp[index] != QueryParser::LIST_LEFT && exp[index] != QueryParser::LIST_RIGHT)
	{
		QChar before;
		QChar after;
		if (index > 0) before = exp[index-1];
		if (index < exp.length()-1) after = exp[index+1];

		// Since all operators start with OP_PIPE it is enough to check for the pipe below:
		if (before == QueryParser::LIST_RIGHT)
			return after.isNull() || after == QueryParser::LIST_DELIM || after == QueryParser::OP_PIPE;

		if (after == QueryParser::LIST_LEFT)
			return before.isNull() || before == QueryParser::LIST_DELIM || isOperatorAtIndex(exp, index - 1);

		return true;
	}

	return false;
}

int HQuery::processEnter(QString& exp, int index)
{
	// Insert a new list delimiter
	exp.insert(index, QueryParser::LIST_DELIM);
	int finalIndex = index+1;

	auto findBoundary = [&exp](int startIndex, bool forward, int& foundIndex) -> bool {
		// Check if it is needed to insert the list syntax
		auto needsDelimiter = false;
		foundIndex = startIndex;
		int subLists = 0;
		while ( foundIndex >= -1 && foundIndex <= exp.length()) // +-1 outside of range
		{
			Q_ASSERT( (foundIndex>=0 && foundIndex< exp.length()) || subLists == 0);

			if (subLists == 0)
			{
				if (foundIndex < 0 || foundIndex == exp.length() || isOperatorAtIndex(exp, foundIndex))
				{
					// We don't have a list, we must insert the list delimiters
					needsDelimiter = true;
					break;
				}
				else if (exp[foundIndex] == QueryParser::LIST_DELIM ||
							exp[foundIndex] == (forward?QueryParser::LIST_RIGHT:QueryParser::LIST_LEFT))
					break; // We already have a list, so nothing to do
				else if (exp[foundIndex] == (forward?QueryParser::LIST_LEFT:QueryParser::LIST_RIGHT) ) {
					++subLists;
				}
			}
			else if (exp[foundIndex] == (forward?QueryParser::LIST_RIGHT:QueryParser::LIST_LEFT)) --subLists;

			if (forward) ++foundIndex;
			else --foundIndex;
		}
		Q_ASSERT(subLists == 0);

		return needsDelimiter;
	};

	int frontIndex, backIndex;
	auto frontNeedsDelim = findBoundary(index-1, false, frontIndex);
	auto backNeedsDelim = findBoundary(index+1, true, backIndex);

	if (frontNeedsDelim || backNeedsDelim )
	{
		exp.insert(frontIndex+1, QueryParser::LIST_LEFT);
		++finalIndex;
		exp.insert(backIndex+1, QueryParser::LIST_RIGHT); // +1 because the previous insert shifted the text by 1
	}

	return finalIndex;
}

int HQuery::removeListsWithOneElement(QString& exp, int& index, int iteratorIndex)
{
	int listStart = iteratorIndex;
	++iteratorIndex;
	int numDelims = 0;

	while (iteratorIndex < exp.length())
	{
		if (exp[iteratorIndex] == QueryParser::LIST_DELIM)
		{
			++numDelims;
			++iteratorIndex;
		}
		else if (exp[iteratorIndex] == QueryParser::LIST_LEFT)
			iteratorIndex = removeListsWithOneElement(exp, index, iteratorIndex);
		else if (exp[iteratorIndex] == QueryParser::LIST_RIGHT)
		{
			if (numDelims == 0) {
				if (index > iteratorIndex) --index;
				exp.remove(iteratorIndex, 1);
				if (index > listStart) --index;
				exp.remove(listStart, 1);
				iteratorIndex -= 1;
			}
			else
				++iteratorIndex;

			break;
		}
		else ++iteratorIndex;
	}

	Q_ASSERT(listStart >= 0 || iteratorIndex >= exp.length());
	return iteratorIndex;
}

bool HQuery::isOperatorAtIndex(const QString& exp, int index)
{
	return exp[index] == QueryParser::OP_PIPE
			|| (exp[index] == QueryParser::OP_MINUS_POSTFIX
				 && index > 0 && exp[index-1] == QueryParser::OP_PIPE);
}

}
