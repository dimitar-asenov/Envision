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

#include "../nodes/QueryNode.h"
#include "../nodes/QueryNodeContainer.h"
#include "../nodes/CommandNode.h"
#include "../nodes/CommandArgument.h"
#include "../nodes/EmptyQueryNode.h"
#include "../parsing/QueryParser.h"
#include "../visualization/VCommandNode.h"
#include "../visualization/VCommandArgument.h"
#include "../visualization/VEmptyQueryNode.h"

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
	OOInteraction::StringComponents::add<CommandNode>([](CommandNode* command) {
		return OOInteraction::StringComponents::c("\"<", command->name(),
			OOInteraction::StringComponents::list(command->arguments(), " ", " ", "", true, true), ">\"");
	});
	OOInteraction::StringComponents::add<CommandArgument>([](CommandArgument* argument) {
		return OOInteraction::StringComponents::c(argument->argument());
	});

	OOInteraction::StringComponents::add<EmptyQueryNode>([](EmptyQueryNode*) {
		return OOInteraction::StringComponents::c("@<", "", ">@");
	});

	OOInteraction::GridBasedOffsetProvider::addGridConstructor<VCommandNode>(
	[](OOInteraction::GridBasedOffsetProvider* grid, VCommandNode* vis){
		grid->add(new OOInteraction::Cell(0, vis->name(), 1));
		grid->add(new OOInteraction::ListCell(1, vis->arguments(), 2, " ", " ", ""));
	});

	OOInteraction::GridBasedOffsetProvider::addGridConstructor<VCommandArgument>(
	[](OOInteraction::GridBasedOffsetProvider* grid, VCommandArgument* vis){
		grid->add(new OOInteraction::Cell(0, vis->argument(), 0));
	});

	OOInteraction::GridBasedOffsetProvider::addGridConstructor<VEmptyQueryNode>(
	[](OOInteraction::GridBasedOffsetProvider* grid, VEmptyQueryNode* vis){
		grid->add(new OOInteraction::Cell(0, vis->empty(), 1));
	});
}

void HQuery::keyPressEvent(Visualization::Item* target, QKeyEvent* event)
{
	target->setUpdateNeeded(Visualization::Item::StandardUpdate);

	auto key = event->key();
//	auto modifiers = event->modifiers();

	bool enterPressed = key == Qt::Key_Enter || key == Qt::Key_Return;
//	bool spacePressed = key == Qt::Key_Space;

	QString str;
	int index;
	auto topMostItem = stringInfo(target, (Qt::Key) key, str, index);
	qDebug() << str << index;
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
				if (! processDeleteOrBackspace(Qt::Key_Delete, newText, newIndex))
					newText.remove(index, 1);
			}
		} break;
		case Qt::Key_Backspace:
		{
			if (index > 0 )
			{
				if (! processDeleteOrBackspace(Qt::Key_Backspace, newText, newIndex))
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

	if (!enterPressed)
		setNewQuery(target, topMostItem, newText, newIndex);
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
	QueryParser::instance().adaptType(newText, index);
	qDebug() << "Adapted" << text << "to" << newText;
	QueryNode* newQuery = QueryParser::instance().parse(newText);

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

	target->scene()->addPostEventAction( new OOInteraction::SetExpressionCursorEvent(parent, newQuery, index));
}

bool HQuery::processDeleteOrBackspace(Qt::Key key, QString& exp, int& index)
{
	if (key == Qt::Key_Delete)
	{
		if ( exp.at(index) != '>') return false;
	}
	if (key == Qt::Key_Backspace)
	{
		if (exp.at(index-1) != '<') return false;
	}
	return true;
}

}
