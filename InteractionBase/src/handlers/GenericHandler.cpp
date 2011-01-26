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
#include "VisualizationBase/headers/items/ModelItem.h"
#include "FilePersistence/headers/SystemClipboard.h"
#include "ModelBase/headers/nodes/List.h"

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

					for (int i = 0; i<selected.size(); ++i)
					{
						Visualization::ModelItem* item = dynamic_cast<Visualization::ModelItem*> (selected.at(i));
						if (item) nodesToCopy.append(item->getNode());
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
						Visualization::ModelItem* item = dynamic_cast<Visualization::ModelItem*> (target);
						if (item && item->getNode()->typeName() == clipboard.currentNodeType())
						{
							item->getNode()->model()->beginModification(item->getNode(), "paste");
							item->getNode()->load(clipboard);
							item->getNode()->model()->endModification();
							target->setUpdateNeeded();
						}
						else InteractionHandler::keyPressEvent(target, event);
					}
					else InteractionHandler::keyPressEvent(target, event);
				}
				break;
/* Undo */	case Qt::Key_Z:
				{
					Visualization::ModelItem* item = dynamic_cast<Visualization::ModelItem*> (target);
					if (item)
					{
						item->getNode()->model()->beginModification(NULL, "undo");
						item->getNode()->model()->undo();
						item->getNode()->model()->endModification();
						target->setUpdateNeeded();
					}
					else InteractionHandler::keyPressEvent(target, event);
				}
				break;
/* Redo */	case Qt::Key_Y:
				{
					Visualization::ModelItem* item = dynamic_cast<Visualization::ModelItem*> (target);
					if (item)
					{
						item->getNode()->model()->beginModification(NULL, "redo");
						item->getNode()->model()->redo();
						item->getNode()->model()->endModification();
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
	InteractionHandler::mousePressEvent(target, event);
}

void GenericHandler::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (!event->buttonDownPos( Qt::LeftButton).isNull())
	{
		QPainterPath path;
		path.addRect( QRectF(event->buttonDownScenePos(Qt::LeftButton), event->scenePos()) );
		target->scene()->setSelectionArea(path, Qt::IntersectsItemShape);
		filterSelectedItems(target, event);
		target->scene()->scheduleUpdate();
	}

	InteractionHandler::mouseMoveEvent(target, event);
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
		if (dynamic_cast<Visualization::ModelItem*> (selection.at(i)))
		{
			modelItemSelected = true;
			break;
		}

	// If there is at least one model item, discard all items which are not model items.
	if (modelItemSelected)
		for (int i = selection.size() - 1; i>=0; --i)
			if (dynamic_cast<Visualization::ModelItem*> (selection.at(i)) == NULL)
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
		const Model::List* parent = dynamic_cast<const Model::List*> (list.first());
		if (parent)
		{
			for (int i = 1; i<list.size(); ++i)
				if (list[i]->parent() != parent)
				{
					parent = NULL;
					break;
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
