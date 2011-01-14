/***********************************************************************************************************************
 * HText.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "HText.h"

#include "VisualizationBase/headers/items/TextRenderer.h"
#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Scene.h"

#include <QtGui/QClipboard>
#include <QtGui/QApplication>

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
	if (event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier)
	{
		QString text = getText(target);
		if (!text.isEmpty()) QApplication::clipboard()->setText(text);
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

// TextRenderer interface routines.
QString HText::getText(Visualization::Item *target)
{
	Visualization::TextRenderer < Visualization::Item > *ti = dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target);
	if ( ti ) return ti->getText();

	Visualization::TextRenderer < Visualization::ModelItem > *tmi = dynamic_cast<Visualization::TextRenderer<Visualization::ModelItem>*> (target);
	if ( tmi ) return tmi->getText();

	return QString();
}

void HText::setSelected(Visualization::Item *target, int xBegin, int xEnd)
{
	Visualization::TextRenderer < Visualization::Item > *ti = dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target);
	if ( ti ) return ti->setSelected(xBegin, xEnd);

	Visualization::TextRenderer < Visualization::ModelItem > *tmi = dynamic_cast<Visualization::TextRenderer<Visualization::ModelItem>*> (target);
	if ( tmi ) return tmi->setSelected(xBegin, xEnd);
}

void HText::resetSelected(Visualization::Item *target)
{
	Visualization::TextRenderer < Visualization::Item > *ti = dynamic_cast<Visualization::TextRenderer<Visualization::Item>*> (target);
	if ( ti ) return ti->resetSelected();

	Visualization::TextRenderer < Visualization::ModelItem > *tmi = dynamic_cast<Visualization::TextRenderer<Visualization::ModelItem>*> (target);
	if ( tmi ) return tmi->resetSelected();
}

}
