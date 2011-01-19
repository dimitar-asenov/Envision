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

namespace Interaction {

CommandExecutionEngine* GenericHandler::executionEngine_ = CommandExecutionEngine::instance();
CommandPrompt* GenericHandler::prompt_ = NULL;

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
		commandReceiver->scene()->updateTopLevelItems();
	}
	else
	{
		removeCommandPrompt();
		prompt_ = new CommandPrompt(commandReceiver);
		commandReceiver->scene()->addTopLevelItem(prompt_);
		prompt_->initializeCommand();
		commandReceiver->scene()->updateTopLevelItems();
	}
}

void GenericHandler::command(Visualization::Item *target, const QString& command)
{
	executionEngine_->execute(target, command);
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
	if (event->button() == Qt::LeftButton)
	{
		QPainterPath path;
		path.addRect( QRectF(event->scenePos(), event->scenePos()).adjusted(0,0,1,1) );
		target->scene()->setSelectionArea(path, Qt::IntersectsItemShape);
	}
}

void GenericHandler::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (!event->buttonDownPos( Qt::LeftButton).isNull())
	{
		QPainterPath path;
		path.addRect( QRectF(event->buttonDownScenePos(Qt::LeftButton), event->scenePos()) );
		target->scene()->setSelectionArea(path, Qt::IntersectsItemShape);

		// Filter out items for which the selection is completely internal.
		QList<QGraphicsItem*> selection = target->scene()->selectedItems();
		for (int i = 0; i<selection.size(); ++i)
		{
			if (  selection.at(i)->contains(selection.at(i)->mapFromScene(event->buttonDownScenePos(Qt::LeftButton)))
				&& selection.at(i)->contains(selection.at(i)->mapFromScene(event->scenePos())))
				selection.at(i)->setSelected(false);
		}

		// Filter out items whose parent is also included in the selection.
		selection = target->scene()->selectedItems();
		for (int i = 0; i<selection.size(); ++i)
		{
			QGraphicsItem* parent = selection.at(i)->parentItem();
			while (parent)
			{
				if (selection.contains(parent))
				{
					selection.at(i)->setSelected(false);
					break;
				}
				parent = parent->parentItem();
			}
		}

	}

	InteractionHandler::mouseMoveEvent(target, event);
}

}
