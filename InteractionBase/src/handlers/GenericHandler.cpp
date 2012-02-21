/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * GenericHandler.cpp
 *
 *  Created on: Jan 13, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/GenericHandler.h"

#include "commands/CommandExecutionEngine.h"
#include "vis/CommandPrompt.h"

#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/cursor/Cursor.h"
#include "FilePersistence/headers/SystemClipboard.h"
#include "ModelBase/headers/nodes/List.h"
#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"

namespace Interaction {

CommandExecutionEngine* GenericHandler::executionEngine_ = CommandExecutionEngine::instance();
CommandPrompt* GenericHandler::prompt_ = nullptr;

QPoint GenericHandler::cursorOriginMidPoint_;
GenericHandler::CursorMoveOrientation GenericHandler::cursorMoveOrientation_ = NoOrientation;

GenericHandler::GenericHandler()
{
}

GenericHandler* GenericHandler::instance()
{
	static GenericHandler h;
	return &h;
}

CommandExecutionEngine* GenericHandler::executionEngine()
{
	return executionEngine_;
}

void GenericHandler::setCommandExecutionEngine(CommandExecutionEngine *engine)
{
	executionEngine_ = engine;
}

void GenericHandler::resetCursorOrigin()
{
	cursorOriginMidPoint_ = QPoint();
	cursorMoveOrientation_ = NoOrientation;
}

CommandPrompt* GenericHandler::prompt()
{
	return prompt_;
}

void GenericHandler::removeCommandPrompt()
{
	if (prompt_) prompt_->scene()->removeTopLevelItem(prompt_);
	SAFE_DELETE_ITEM(prompt_);
}

void GenericHandler::showCommandPrompt(Visualization::Item* commandReceiver)
{
	if (prompt_ && prompt_->commandReceiver() == commandReceiver)
	{
		prompt_->showPrompt();
	}
	else
	{
		removeCommandPrompt();
		prompt_ = new CommandPrompt(commandReceiver);
		commandReceiver->scene()->addTopLevelItem(prompt_);
		prompt_->initializeCommand();
	}
}

void GenericHandler::command(Visualization::Item *target, const QString& command)
{
	executionEngine_->execute(target, command);
}

void GenericHandler::beforeEvent(Visualization::Item *, QEvent* event)
{
	if (	event->type() == QEvent::GraphicsSceneMouseMove
			|| event->type() == QEvent::GraphicsSceneMousePress
			|| event->type() == QEvent::GraphicsSceneMouseDoubleClick)
	{
		resetCursorOrigin();
	}
}

void GenericHandler::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	if (event->matches(QKeySequence::Copy))
	{
		QList<const Model::Node*> nodesToCopy;
		QList<QGraphicsItem*> selected = target->scene()->selectedItems();

		// Get all items from the current selection that are model items.
		for (int i = 0; i<selected.size(); ++i)
		{
			Visualization::Item* item = static_cast<Visualization::Item*> (selected.at(i));
			if (item->hasNode()) nodesToCopy.append(item->node());
		}

		// In case there is exactly one selected item that is not a model item try to find the first parent that it has which is a model item.
		if (nodesToCopy.size() == 0 && selected.size() == 1)
		{
			Visualization::Item* item = static_cast<Visualization::Item*> (selected.at(0));
			while (item)
			{
				if (item->hasNode())
				{
					nodesToCopy.append(item->node());
					break;
				}

				item = static_cast<Visualization::Item*> ( item->parentItem() );
			}
		}

		if (nodesToCopy.size() > 0)
		{

			FilePersistence::SystemClipboard clipboard;
			arrangeNodesForClipboard(nodesToCopy);
			clipboard.putNodes(nodesToCopy);
		}
	}
	else if (event->matches(QKeySequence::Paste))
	{
		FilePersistence::SystemClipboard clipboard;
		if (clipboard.numNodes() == 1 && target->scene()->selectedItems().size() == 1 && target->isSelected())
		{
			if (target->hasNode() && target->node()->typeName() == clipboard.currentNodeType())
			{
				target->node()->model()->beginModification(target->node(), "paste");
				target->node()->load(clipboard);
				target->node()->model()->endModification();
				target->setUpdateNeeded();
			}
			else InteractionHandler::keyPressEvent(target, event);
		}
		else InteractionHandler::keyPressEvent(target, event);
	}
	else if (event->matches(QKeySequence::Undo))
	{
		if (target->hasNode())
		{
			Model::Model* m = target->node()->model();
			m->beginModification(nullptr, "undo");
			m->undo();
			m->endModification();
			target->setUpdateNeeded();
		}
		else InteractionHandler::keyPressEvent(target, event);
	}
	else if (event->matches(QKeySequence::Redo))
	{
		if (target->hasNode())
		{
			Model::Model* m = target->node()->model();
			m->beginModification(nullptr, "redo");
			m->redo();
			m->endModification();
			target->setUpdateNeeded();
		}
		else InteractionHandler::keyPressEvent(target, event);
	}
	else if (event->modifiers() == 0
			&& (	event->key() == Qt::Key_Up
					|| event->key() == Qt::Key_Down
					|| event->key() == Qt::Key_Left
					|| event->key() == Qt::Key_Right))
	{
		bool processed = false;
		Visualization::Item::CursorMoveDirection dir;

		// Set the source navigation point when beginning to navigate in a new direction
		if (	(event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
				&& cursorMoveOrientation_ != VerticalOrientation)
		{
			cursorMoveOrientation_ = VerticalOrientation;
			Visualization::Cursor* c = target->scene()->mainCursor();
			if (c) cursorOriginMidPoint_ = c->region().center();
		}
		if (	(event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)
				&& cursorMoveOrientation_ != HorizontalOrientation)
		{
			cursorMoveOrientation_ = HorizontalOrientation;
			Visualization::Cursor* c = target->scene()->mainCursor();
			if (c) cursorOriginMidPoint_ = c->region().center();
		}

		QPoint midpoint = target->mapFromScene(cursorOriginMidPoint_).toPoint();
		switch( event->key() )
		{
			case Qt::Key_Up:
			{
				processed = target->moveCursor(Visualization::Item::MoveUp, midpoint);
				if (!processed) dir = Visualization::Item::MoveUpOf;
			}
			break;
			case Qt::Key_Down:
			{
				processed = target->moveCursor(Visualization::Item::MoveDown, midpoint);
				if (!processed) dir = Visualization::Item::MoveDownOf;
			}
			break;
			case Qt::Key_Left:
			{
				processed = target->moveCursor(Visualization::Item::MoveLeft, midpoint);
				if (!processed) dir = Visualization::Item::MoveLeftOf;
			}
			break;
			case Qt::Key_Right:
			{
				processed = target->moveCursor(Visualization::Item::MoveRight, midpoint);
				if (!processed) dir = Visualization::Item::MoveRightOf;
			}
			break;
			default:
			{
				resetCursorOrigin();
			}
			break;
		}

		if (!processed)
		{
			Visualization::Item* current = target;

			while (current && !processed)
			{
				Visualization::Item* parent = static_cast<Visualization::Item*> (current->parentItem());
				if (!parent) break;

				QPoint reference;
				switch( event->key() )
				{
					case Qt::Key_Up:
					{
						int border = current->scenePos().y();
						reference = QPoint(cursorOriginMidPoint_.x(), border);
					} break;
					case Qt::Key_Down:
					{
						int border = current->scenePos().y() + current->height()-1;
						reference = QPoint(cursorOriginMidPoint_.x(), border);
					} break;
					case Qt::Key_Left:
					{
						int border = current->scenePos().x();
						reference = QPoint(border, cursorOriginMidPoint_.y());
					} break;
					case Qt::Key_Right:
					{
						int border = current->scenePos().x() + current->width()-1;
						reference = QPoint(border, cursorOriginMidPoint_.y());
					} break;
				}

				reference = parent->mapFromScene(reference).toPoint();

				processed = parent->moveCursor(dir, reference);
				current = parent;
			}
		}

		event->accept();
	}
	else InteractionHandler::keyPressEvent(target, event);
}

void GenericHandler::keyReleaseEvent(Visualization::Item *target, QKeyEvent *event)
{
	// Only show the command prompt if this event was not received within it.
	if (event->key() == Qt::Key_Escape && !(prompt_ && prompt_->isAncestorOf(target)) )
	{
		showCommandPrompt(target);
	}
	else InteractionHandler::keyReleaseEvent(target, event);
}

void GenericHandler::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier)
	{
		// Ignore the event and do not send it to the Interaction handler. This prevents the default event handlers from
		// processing the event.
		event->ignore();
		return;
	}

	if (event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier)
		target->moveCursor(Visualization::Item::MoveOnPosition, event->pos().toPoint());

	InteractionHandler::mousePressEvent(target, event);
}

void GenericHandler::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->modifiers() == 0 && !event->buttonDownPos( Qt::LeftButton).isNull() )
	{
		QPainterPath path;
		path.addRect( QRectF(event->buttonDownScenePos(Qt::LeftButton), event->scenePos()) );
		target->scene()->setSelectionArea(path, Qt::IntersectsItemShape);
		filterSelectedItems(target, event);
		target->scene()->scheduleUpdate();
	}
	else InteractionHandler::mouseMoveEvent(target, event);
}

void GenericHandler::mouseDoubleClickEvent(Visualization::Item *, QGraphicsSceneMouseEvent *event)
{
	event->ignore();
}

void GenericHandler::focusInEvent(Visualization::Item *target, QFocusEvent *event)
{
	QGraphicsItem* i = target;
	while (i)
	{
		if (i->flags() & QGraphicsItem::ItemIsSelectable)
		{
			target->scene()->clearSelection();
			i->setSelected(true);
			break;
		}
		else i = i->parentItem();
	}

	InteractionHandler::focusInEvent(target, event);
}

void GenericHandler::filterSelectedItems(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	QList<QGraphicsItem*> selection = target->scene()->selectedItems();

	// Filter out items for which the selection is completely internal.
	for (int i = selection.size()-1; i>=0; --i)
	{
		if (  selection.at(i)->contains(selection.at(i)->mapFromScene(event->buttonDownScenePos(Qt::LeftButton)))
			&& selection.at(i)->contains(selection.at(i)->mapFromScene(event->scenePos())))
		{
			selection.at(i)->setSelected(false);
			selection.removeAt(i);
		}
	}

	// Filter out items whose parent is also included in the selection.
	for (int i = selection.size() - 1; i>=0; --i)
		for(int k = selection.size() - 1; k>=0; --k)
			if (k!=i && selection.at(k)->isAncestorOf(selection.at(i)))
			{
				selection.at(i)->setSelected(false);
				selection.removeAt(i);
				break;
			}

	// Check if there are any selected model items remaining.
	bool modelItemSelected = false;
	selection = target->scene()->selectedItems();
	for (int i = 0; i<selection.size(); ++i)
		if ( (static_cast<Visualization::Item*> (selection.at(i)))->hasNode())
		{
			modelItemSelected = true;
			break;
		}

	// If there is at least one model item, discard all items which are not model items.
	if (modelItemSelected)
		for (int i = selection.size() - 1; i>=0; --i)
			if ((static_cast<Visualization::Item*> (selection.at(i)))->hasNode() == false)
			{
				selection.at(i)->setSelected(false);
				selection.removeAt(i);
			}
}

void GenericHandler::arrangeNodesForClipboard(QList<const Model::Node*>& list)
{
	if (list.size() > 0)
	{
		// Determine if all nodes are elements of a list
		const Model::List* parent = dynamic_cast<const Model::List*> (list.first()->parent());
		if (parent)
		{
			for (int i = 1; i<list.size(); ++i)
			{
				if (list[i]->parent() != parent)
				{
					parent = nullptr;
					break;
				}
			}
		}

		if (parent)
		{
			// The selection consists only of nodes which are elements of the same list. Arrange them properly
			// according to the list's order.
			// Bubble sort
			for (int i = list.size() - 1; i > 0; --i)
				for (int k = 0; k < i; ++k)
					if (parent->indexOf(list[k]) > parent->indexOf(list[k+1])) list.swap(k, k+1);
		}
	}
}

}
