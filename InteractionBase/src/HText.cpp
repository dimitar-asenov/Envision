/***********************************************************************************************************************
 * HText.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "HText.h"

#include "VisualizationBase/headers/items/Text.h"
#include "VisualizationBase/headers/items/VText.h"
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

HText::HText()
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
		switch (event->key())
		{
			case Qt::Key_C:
				{
					QString text = TEXTRENDERER_GET(getSelectedText);
					if (!text.isEmpty()) QApplication::clipboard()->setText(text);
				}
				break;
			case Qt::Key_V:
				{
					insertText(target, QApplication::clipboard()->text());
					target->scene()->updateTopLevelItems();
				}
				break;
			default:
				GenericHandler::keyPressEvent(target, event);
				break;
		}
	}
	else
	{
		if (isEditable(target))
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

				default:
					{
						if (!event->text().isEmpty() && event->text().at(0).isPrint() ) insertText(target, event->text());
						else GenericHandler::keyPressEvent(target, event);
					}
					break;
			}

			if (event->isAccepted()) target->scene()->updateTopLevelItems();
		}
		else GenericHandler::keyPressEvent(target, event);
	}
}

void HText::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if ( event->button() == Qt::LeftButton )
	{
		setSelected(target, event->pos().x(), event->pos().x());
		target->scene()->clearSelection();
		target->scene()->updateTopLevelItems();
	}
	else GenericHandler::mousePressEvent(target, event);
}

void HText::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (!event->buttonDownPos(Qt::LeftButton).isNull() && target->contains( event->pos() ))
	{
		setSelected(target, event->buttonDownPos(Qt::LeftButton).x(), event->pos().x());
		target->scene()->clearSelection();
	}
	else
	{
		resetSelected(target);
		GenericHandler::mouseMoveEvent(target, event);
	}

	target->scene()->updateTopLevelItems();
}

void HText::focusOutEvent(Visualization::Item *target, QFocusEvent *)
{
	resetSelected(target);
}

void HText::setNewText(Visualization::Item *target, const QString& newText)
{
	Visualization::Text *t = dynamic_cast<Visualization::Text*> (target);
	if ( t ) t->setText(newText);
	else
	{
		Visualization::VText *v = dynamic_cast<Visualization::VText*> (target);
		if ( v )
		{
			Model::Text* modText = static_cast<Model::Text*> (v->getNode());
			modText->getModel()->beginModification(modText, "Set text");
			modText->set(newText);
			modText->getModel()->endModification();
		}
	}
}

// TextRenderer interface routines.
void HText::setSelected(Visualization::Item *target, int xBegin, int xEnd)
{
	Visualization::TextRenderer < Visualization::Item > *ti = dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target);
	if ( ti ) ti->setSelected(xBegin, xEnd);
	else
	{
		Visualization::TextRenderer < Visualization::ModelItem > *tmi = dynamic_cast<Visualization::TextRenderer<Visualization::ModelItem>*> (target);
		if ( tmi ) tmi->setSelected(xBegin, xEnd);
	}
}

void HText::resetSelected(Visualization::Item *target)
{
	Visualization::TextRenderer < Visualization::Item > *ti = dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target);
	if ( ti ) ti->resetSelected();
	else
	{
		Visualization::TextRenderer < Visualization::ModelItem > *tmi = dynamic_cast<Visualization::TextRenderer<Visualization::ModelItem>*> (target);
		if ( tmi ) tmi->resetSelected();
	}
}

bool HText::isEditable(Visualization::Item *target)
{
	Visualization::TextRenderer < Visualization::Item > *ti = dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target);
	if ( ti ) return ti->isEditable();

	Visualization::TextRenderer < Visualization::ModelItem > *tmi = dynamic_cast<Visualization::TextRenderer<Visualization::ModelItem>*> (target);
	if ( tmi ) return tmi->isEditable();

	return false;
}

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
		setNewText(target, newText);
		TEXTRENDERER_SET1(setCaretPosition, selFirst);
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
				setNewText(target, newText);
			}
			else
			{
				// backspace was pressed
				if (caret > 0)
				{
					newText = newText.left(caret-1) + newText.mid(caret);
					setNewText(target, newText);
					TEXTRENDERER_SET1(setCaretPosition, caret-1);
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
	setNewText(target, newText);
	caret += textToInsert.size();
	TEXTRENDERER_SET1(setCaretPosition, caret);
}

}
