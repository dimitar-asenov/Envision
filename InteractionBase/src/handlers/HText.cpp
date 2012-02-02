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
 * HText.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HText.h"

#include "VisualizationBase/headers/items/TextRenderer.h"
#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/cursor/TextCursor.h"

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
		switch (event->key())
		{
			case Qt::Key_Left:
				moveCaret(target, event);
				break;
			case Qt::Key_Right:
				moveCaret(target, event);
				break;
			default:
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
				break;
		}

	}
	else GenericHandler::keyPressEvent(target, event);

}

void HText::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	Visualization::TextRenderer* tr = static_cast<Visualization::TextRenderer*> (target);

	doubleClick = false;
	if ( event->modifiers() == 0 && event->button() == Qt::LeftButton )
	{
		tr->correspondingSceneCursor<Visualization::TextCursor>()->setSelectedByDrag(event->pos().x(), event->pos().x());
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
		tr->correspondingSceneCursor<Visualization::TextCursor>()->selectAll();
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
			tr->correspondingSceneCursor<Visualization::TextCursor>()
					->setSelectedByDrag(event->buttonDownPos(Qt::LeftButton).x(), event->pos().x());
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
	tr->scene()->setMainCursor(new Visualization::TextCursor(tr));

	GenericHandler::FocusDirection dir = GenericHandler::focusDirection();

	int size = tr->text().length();

	if (size > 0)
	{
		// Here we choose which child to focus.
		if (dir == GenericHandler::FROM_LEFT)
			tr->correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(0);
		else if (dir == GenericHandler::FROM_RIGHT)
			tr->correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(size);
	}

	//GenericHandler::focusInEvent(target, event);
	InteractionHandler::focusInEvent(target, event);
}

// TextRenderer interface routines.
void HText::moveCaret(Visualization::Item *target, QKeyEvent *event)
{
	Visualization::TextRenderer* tr = static_cast<Visualization::TextRenderer*> (target);

	switch (event->key())
	{
		case Qt::Key_Left:
			{
				int position = tr->correspondingSceneCursor<Visualization::TextCursor>()->caretPosition();
				if ( tr->text().isEmpty() || position <= 0) event->ignore();
				else tr->correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(position - 1);
			}
			break;
		case Qt::Key_Right:
			{
				int position = tr->correspondingSceneCursor<Visualization::TextCursor>()->caretPosition();
				int size = tr->text().size();
				if ( tr->text().isEmpty() || position >= size) event->ignore();
				else tr->correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(position + 1);
			}
			break;
	}
}
void HText::erase(Visualization::Item *target, bool forwards, bool onlyDeleteIfSelected)
{
	Visualization::TextRenderer* tr = static_cast<Visualization::TextRenderer*> (target);

	QString newText = tr->text();
	int selFirst = tr->correspondingSceneCursor<Visualization::TextCursor>()->selectionFirstIndex();
	int selLast = tr->correspondingSceneCursor<Visualization::TextCursor>()->selectionLastIndex();

	if (selFirst != selLast)
	{
		// There is some text that is selected
		newText = newText.left(selFirst) + newText.mid(selLast);
		if ( tr->setText(newText) ) tr->correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(selFirst);
	}
	else
	{
		// No text is selected
		if (!onlyDeleteIfSelected)
		{
			int caret = tr->correspondingSceneCursor<Visualization::TextCursor>()->caretPosition();
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
						tr->correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(caret-1);
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
	int caret = tr->correspondingSceneCursor<Visualization::TextCursor>()->caretPosition();
	newText.insert(caret, textToInsert);
	if ( tr->setText(newText) )
	{
		caret += textToInsert.size();
		tr->correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(caret);
	}
}

}
