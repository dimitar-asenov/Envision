/***********************************************************************************************************************
 * HText.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HText.h"

#include "VisualizationBase/headers/items/TextRenderer.h"
#include "VisualizationBase/headers/Scene.h"

#include "ModelBase/headers/Model.h"

#include <QtGui/QClipboard>
#include <QtGui/QApplication>

namespace Interaction {

HText::HText() : doubleClick(false)
{
}

HText* HText::instance()
{
	static HText h;
	return &h;
}

void HText::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	Visualization::TextRenderer* tr = static_cast<Visualization::TextRenderer*> (target);

	if (event->modifiers() == Qt::ControlModifier)
	{
		if (target->scene()->selectedItems().size() == 0)
		{
			switch (event->key())
			{
				case Qt::Key_C:
					{
						QString text = tr->selectedText();

						if (!text.isEmpty()) QApplication::clipboard()->setText(text);
						else GenericHandler::keyPressEvent(target, event);
					}
					break;
				case Qt::Key_V:
					{
						if ( tr->isEditable() )
						{
							erase(target, true, true);
							target->updateSubtree();
							insertText(target, QApplication::clipboard()->text());
						}
					}
					break;
				default:
					GenericHandler::keyPressEvent(target, event);
					break;
			}
		}
		else GenericHandler::keyPressEvent(target, event);
	}
	else if ( ! (event->modifiers() & Qt::ControlModifier))
	{
		if ( tr->isEditable())
		{
			switch (event->key())
			{
				case Qt::Key_Backspace:
					erase(target, false, false);
					break;
				case Qt::Key_Delete:
					erase(target, true, false);
					break;
				case Qt::Key_Left:
					moveCaret(target, event);
					break;
				case Qt::Key_Right:
					moveCaret(target, event);
					break;
				case Qt::Key_Tab:
					GenericHandler::keyPressEvent(target, event);
					break;

				default:
					{
						if (!event->text().isEmpty() && event->text().at(0).isPrint() ) insertText(target, event->text());
						else GenericHandler::keyPressEvent(target, event);
					}
					break;
			}
		}
		else GenericHandler::keyPressEvent(target, event);
	}
	else GenericHandler::keyPressEvent(target, event);

}

void HText::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	Visualization::TextRenderer* tr = static_cast<Visualization::TextRenderer*> (target);

	doubleClick = false;
	if ( event->modifiers() == 0 && event->button() == Qt::LeftButton )
	{
		tr->setSelectedByDrag(event->pos().x(), event->pos().x());
	}

	GenericHandler::mousePressEvent(target, event);
}

void HText::mouseReleaseEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if ( event->modifiers() == 0 && doubleClick ) doubleClick = false;
	else GenericHandler::mouseReleaseEvent(target, event);
}

void HText::mouseDoubleClickEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	Visualization::TextRenderer* tr = static_cast<Visualization::TextRenderer*> (target);

	doubleClick = true;
	if ( event->modifiers() == 0 && event->button() == Qt::LeftButton )
	{
		target->scene()->clearSelection();
		tr->selectAll();
	}
	else GenericHandler::mousePressEvent(target, event);
}

void HText::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	Visualization::TextRenderer* tr = static_cast<Visualization::TextRenderer*> (target);

	if (event->modifiers() == 0)
	{
		if (!event->buttonDownPos(Qt::LeftButton).isNull() && target->contains( event->pos() ))
		{
			tr->setSelectedByDrag(event->buttonDownPos(Qt::LeftButton).x(), event->pos().x());
			target->scene()->clearSelection();
		}
		else
		{
			target->setUpdateNeeded();
			GenericHandler::mouseMoveEvent(target, event);
		}

		target->scene()->scheduleUpdate();
	}
	else GenericHandler::mouseMoveEvent(target, event);
}

void HText::focusOutEvent(Visualization::Item *target, QFocusEvent *)
{
	target->setUpdateNeeded();
}

void HText::focusInEvent(Visualization::Item *target, QFocusEvent *event)
{
	Visualization::TextRenderer* tr = static_cast<Visualization::TextRenderer*> (target);

	GenericHandler::FocusDirection dir = GenericHandler::focusDirection();

	int size = tr->text().length();

	if (size > 0)
	{
		// Here we choose which child to focus.
		if (dir == GenericHandler::FROM_LEFT) tr->setCaretPosition(0);
		else if (dir == GenericHandler::FROM_RIGHT) tr->setCaretPosition(size);
	}

	GenericHandler::focusInEvent(target, event);
}

// TextRenderer interface routines.
void HText::moveCaret(Visualization::Item *target, QKeyEvent *event)
{
	Visualization::TextRenderer* tr = static_cast<Visualization::TextRenderer*> (target);

	switch (event->key())
	{
		case Qt::Key_Left:
			{
				int position = tr->caretPosition();
				if ( tr->text().isEmpty() || position <= 0) event->ignore();
				else tr->setCaretPosition(position - 1);
			}
			break;
		case Qt::Key_Right:
			{
				int position = tr->caretPosition();
				int size = tr->text().size();
				if ( tr->text().isEmpty() || position >= size) event->ignore();
				else tr->setCaretPosition(position + 1);
			}
			break;
	}
}
void HText::erase(Visualization::Item *target, bool forwards, bool onlyDeleteIfSelected)
{
	Visualization::TextRenderer* tr = static_cast<Visualization::TextRenderer*> (target);

	QString newText = tr->text();
	int selFirst = tr->selectionFirstInxed();
	int selLast = tr->selectionLastIndex();

	if (selFirst != selLast)
	{
		// There is some text that is selected
		newText = newText.left(selFirst) + newText.mid(selLast);
		if ( tr->setText(newText) ) tr->setCaretPosition(selFirst);
	}
	else
	{
		// No text is selected
		if (!onlyDeleteIfSelected)
		{
			int caret = tr->caretPosition();
			if (forwards)
			{
				// delete was pressed
				newText = newText.left(caret) + newText.mid(caret+1);
				tr->setText(newText);
			}
			else
			{
				// backspace was pressed
				if (caret > 0)
				{
					newText = newText.left(caret-1) + newText.mid(caret);
					if ( tr->setText(newText) )
					{
						tr->setCaretPosition(caret-1);
					}
				}
			}
		}
	}
}

void HText::insertText(Visualization::Item *target, const QString& textToInsert)
{
	Visualization::TextRenderer* tr = static_cast<Visualization::TextRenderer*> (target);

	erase(target, true, true);
	QString newText = tr->text();
	int caret = tr->caretPosition();
	newText.insert(caret, textToInsert);
	if ( tr->setText(newText) )
	{
		caret += textToInsert.size();
		tr->setCaretPosition(caret);
	}
}

}
