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
 * HOOExpression.cpp
 *
 *  Created on: Feb 8, 2012
 *      Author: Dimitar Asenov
 */

#include "handlers/HOOExpression.h"

#include "string_providers/StringProvider.h"
#include "expression_editor/OOExpressionBuilder.h"
#include "handlers/SetCursorEvent.h"

#include "OOModel/headers/allOOModelNodes.h"

#include "ModelBase/headers/adapter/AdapterManager.h"

#include <QtCore/QDebug>

namespace OOInteraction {

HOOExpression::HOOExpression()
{

}

HOOExpression* HOOExpression::instance()
{
	static HOOExpression h;
	return &h;
}

void HOOExpression::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	// TODO implement this better. It is supposed to only let typed characters through and igonre modifier keys.
	// However it does not work with e.g. ALTGR characters.
	if (event->text().isEmpty() || (event->modifiers() != Qt::NoModifier && event->modifiers() != Qt::ShiftModifier))
	{
		GenericHandler::keyPressEvent(target, event);
		return;
	}

	// Find the top most parent that is adaptable to StringProvider
	Visualization::Item* topMostItem = target;
	StringProvider* topMostSP = Model::AdapterManager::adapt<StringProvider>(topMostItem);

	auto p = topMostItem->parentItem();
	while(p)
	{
		StringProvider* adapted = Model::AdapterManager::adapt<StringProvider>(p);
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

	auto parent = static_cast<Visualization::Item*> (topMostItem->parentItem());

	Model::Node* containerNode = topMostItem->node()->parent();
	containerNode->model()->beginModification(containerNode, "edit expression");
	OOModel::Expression* newExpression = OOExpressionBuilder::getOOExpression( newText );
	containerNode->replaceChild(topMostItem->node(), newExpression);
	containerNode->model()->endModification();

	// We need to trigger an update of all the visualizations leading up to the target, even though the target
	// visualization will probably be deleted and replaced with a new one.
	target->setUpdateNeeded();

	QApplication::postEvent(target->scene(), new SetCursorEvent(parent, newExpression, newIndex));

	GenericHandler::keyPressEvent(target, event);
	event->accept();
}

} /* namespace OOInteraction */
