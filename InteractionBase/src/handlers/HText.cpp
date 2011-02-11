/***********************************************************************************************************************
 * HText.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HText.h"

#include "VisualizationBase/headers/items/TextRenderer.h"
#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Scene.h"

#include "ModelBase/headers/Model.h"

#include <QtGui/QClipboard>
#include <QtGui/QApplication>

//**********************************************************************************************************************
#define TEXTRENDERER_GET(method)		(																												\
	( dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target)	) ?													\
	( (dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target))->method()	) :									\
	( (dynamic_cast<Visualization::TextRenderer<Visualization::ModelItem>*> (target))->method()	)	)
//**********************************************************************************************************************

//**********************************************************************************************************************
#define TEXTRENDERER_GET1(method, arg1)		(																									\
	( dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target)	) ?													\
	( (dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target))->method(arg1)	) :							\
	( (dynamic_cast<Visualization::TextRenderer<Visualization::ModelItem>*> (target))->method(arg1)	)	)
//**********************************************************************************************************************

//**********************************************************************************************************************
#define TEXTRENDERER_SET(method)																														\
	if (dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target))														\
		(dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target))->method();										\
	else																																						\
		(dynamic_cast<Visualization::TextRenderer<Visualization::ModelItem>*> (target))->method();
//**********************************************************************************************************************

//**********************************************************************************************************************
#define TEXTRENDERER_SET1(method, arg1)																											\
	if (dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target))														\
		(dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target))->method(arg1);									\
	else																																						\
		(dynamic_cast<Visualization::TextRenderer<Visualization::ModelItem>*> (target))->method(arg1);
//**********************************************************************************************************************

//**********************************************************************************************************************
#define TEXTRENDERER_SET2(method, arg1, arg2)																									\
	if (dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target))														\
		(dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target))->method((arg1), (arg2));					\
	else																																						\
		(dynamic_cast<Visualization::TextRenderer<Visualization::ModelItem>*> (target))->method((arg1), (arg2));
//**********************************************************************************************************************

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
	if (event->modifiers() == Qt::ControlModifier)
	{
		if (target->scene()->selectedItems().size() == 0 || (target->isSelected() && target->scene()->selectedItems().size() == 1))
		{
			switch (event->key())
			{
				case Qt::Key_C:
					{
						QString text = TEXTRENDERER_GET(getSelectedText);
						if (text.isEmpty() && !TEXTRENDERER_GET(isEditable) && target->isSelected()) text = TEXTRENDERER_GET(getText);

						if (!text.isEmpty()) QApplication::clipboard()->setText(text);
						else GenericHandler::keyPressEvent(target, event);
					}
					break;
				case Qt::Key_V:
					{
						if (TEXTRENDERER_GET(isEditable))
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
		if (TEXTRENDERER_GET(isEditable))
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
	doubleClick = false;
	if ( event->modifiers() == 0 && event->button() == Qt::LeftButton )
	{
		TEXTRENDERER_SET2(setSelectedByDrag, event->pos().x(), event->pos().x());
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
	doubleClick = true;
	if ( event->modifiers() == 0 && event->button() == Qt::LeftButton )
	{
		target->scene()->clearSelection();
		TEXTRENDERER_SET(selectAll);
	}
	else GenericHandler::mousePressEvent(target, event);
}

void HText::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->modifiers() == 0)
	{
		if (!event->buttonDownPos(Qt::LeftButton).isNull() && target->contains( event->pos() ))
		{
			TEXTRENDERER_SET2(setSelectedByDrag, event->buttonDownPos(Qt::LeftButton).x(), event->pos().x());
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
	GenericHandler::FocusDirection dir = GenericHandler::focusDirection();

	int size = TEXTRENDERER_GET(getText).length();

	if (size > 0)
	{
		// Here we choose which child to focus.
		if (dir == GenericHandler::FROM_LEFT) { TEXTRENDERER_SET1(setCaretPosition, 0) }
		else if (dir == GenericHandler::FROM_RIGHT) {TEXTRENDERER_SET1(setCaretPosition, size)} ;
	}

	GenericHandler::focusInEvent(target, event);
}

// TextRenderer interface routines.
void HText::moveCaret(Visualization::Item *target, QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Left:
			{
				int position = TEXTRENDERER_GET(caretPosition);
				if (TEXTRENDERER_GET(getText).isEmpty() || position <= 0) event->ignore();
				else TEXTRENDERER_SET1(setCaretPosition, position - 1);
			}
			break;
		case Qt::Key_Right:
			{
				int position = TEXTRENDERER_GET(caretPosition);
				int size = TEXTRENDERER_GET(getText).size();
				if (TEXTRENDERER_GET(getText).isEmpty() || position >= size) event->ignore();
				else TEXTRENDERER_SET1(setCaretPosition, position + 1);
			}
			break;
	}
}
void HText::erase(Visualization::Item *target, bool forwards, bool onlyDeleteIfSelected)
{
	QString newText = TEXTRENDERER_GET(getText);
	int selFirst = TEXTRENDERER_GET(selectionFirstInxed);
	int selLast = TEXTRENDERER_GET(selectionLastIndex);

	if (selFirst != selLast)
	{
		// There is some text that is selected
		newText = newText.left(selFirst) + newText.mid(selLast);
		if ( TEXTRENDERER_GET1(setText, newText) )
		{
			TEXTRENDERER_SET1(setCaretPosition, selFirst);
		}
	}
	else
	{
		// No text is selected
		if (!onlyDeleteIfSelected)
		{
			int caret = TEXTRENDERER_GET(caretPosition);
			if (forwards)
			{
				// delete was pressed
				newText = newText.left(caret) + newText.mid(caret+1);
				TEXTRENDERER_SET1(setText, newText);
			}
			else
			{
				// backspace was pressed
				if (caret > 0)
				{
					newText = newText.left(caret-1) + newText.mid(caret);
					if ( TEXTRENDERER_GET1(setText, newText) )
					{
						TEXTRENDERER_SET1(setCaretPosition, caret-1);
					}
				}
			}
		}
	}
}

void HText::insertText(Visualization::Item *target, const QString& textToInsert)
{
	erase(target, true, true);
	QString newText = TEXTRENDERER_GET(getText);
	int caret = TEXTRENDERER_GET(caretPosition);
	newText.insert(caret, textToInsert);
	if ( TEXTRENDERER_GET1(setText, newText) )
	{
		caret += textToInsert.size();
		TEXTRENDERER_SET1(setCaretPosition, caret);
	}
}

}
