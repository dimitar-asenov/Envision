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
#include "FilePersistence/headers/SystemClipboard.h"
#include "ModelBase/headers/nodes/List.h"
#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"

namespace Interaction {

CommandExecutionEngine* GenericHandler::executionEngine_ = CommandExecutionEngine::instance();
CommandPrompt* GenericHandler::prompt_ = NULL;
GenericHandler::FocusDirection GenericHandler::focusDirection_ = NOT_SPECIFIED;

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

void GenericHandler::setFocusDirection(FocusDirection direction)
{
	focusDirection_ = direction;
}

GenericHandler::FocusDirection GenericHandler::focusDirection()
{
	return focusDirection_;
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
	if (event->type() == QEvent::GraphicsSceneMouseMove ||
		 event->type() == QEvent::GraphicsSceneMousePress ||
		 event->type() == QEvent::GraphicsSceneMouseDoubleClick ||
		 event->type() == QEvent::KeyPress)
	{
		focusDirection_ = NOT_SPECIFIED;
	}
}

void GenericHandler::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	if (event->modifiers() == Qt::ControlModifier)
	{
		switch (event->key())
		{
/* Copy */	case Qt::Key_C:
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
				break;
/* Paste */	case Qt::Key_V:
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
				break;
/* Undo */	case Qt::Key_Z:
				{
					if (target->hasNode())
					{
						target->node()->model()->beginModification(NULL, "undo");
						target->node()->model()->undo();
						target->node()->model()->endModification();
						target->setUpdateNeeded();
					}
					else InteractionHandler::keyPressEvent(target, event);
				}
				break;
/* Redo */	case Qt::Key_Y:
				{
					if (target->hasNode())
					{
						target->node()->model()->beginModification(NULL, "redo");
						target->node()->model()->redo();
						target->node()->model()->endModification();
						target->setUpdateNeeded();
					}
					else InteractionHandler::keyPressEvent(target, event);
				}
				break;
			default:
				InteractionHandler::keyPressEvent(target, event);
				break;
		}
	}
	else if (event->modifiers() == 0)
	{
		bool processed = false;
		switch( event->key() )
		{
			case Qt::Key_Up:
			{
				setFocusDirection(FROM_BOTTOM);
				processed = target->focusChild(Visualization::Item::FOCUS_UP);
			}
			break;
			case Qt::Key_Down:
			{
				setFocusDirection(FROM_TOP);
				processed = target->focusChild(Visualization::Item::FOCUS_DOWN);
			}
			break;
			case Qt::Key_Left:
			{
				setFocusDirection(FROM_RIGHT);
				processed = target->focusChild(Visualization::Item::FOCUS_LEFT);
			}
			break;
			case Qt::Key_Right:
			{
				setFocusDirection(FROM_LEFT);
				processed = target->focusChild(Visualization::Item::FOCUS_RIGHT);
			}
			break;
		}

		if (!processed) InteractionHandler::keyPressEvent(target, event);
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
	if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) event->ignore();
	else InteractionHandler::mousePressEvent(target, event);
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

void GenericHandler::focusInEvent(Visualization::Item *target, QFocusEvent *event)
{
	// Here we choose which child to focus.
	if (focusDirection_ == GenericHandler::NOT_SPECIFIED) target->focusChild(Visualization::Item::FOCUS_DEFAULT);
	else if (focusDirection_ == GenericHandler::FROM_LEFT ) target->focusChild(Visualization::Item::FOCUS_LEFTMOST);
	else if (focusDirection_ == GenericHandler::FROM_RIGHT ) target->focusChild(Visualization::Item::FOCUS_RIGHTMOST);
	else if (focusDirection_ == GenericHandler::FROM_TOP ) target->focusChild(Visualization::Item::FOCUS_TOPMOST);
	else if (focusDirection_ == GenericHandler::FROM_BOTTOM ) target->focusChild(Visualization::Item::FOCUS_BOTTOMMOST);

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
					parent = NULL;
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
