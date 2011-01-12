/***********************************************************************************************************************
 * HText.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "HText.h"
#include "VisualizationBase/headers/Scene.h"

namespace Interaction {

HText::HText()
{
}

HText* HText::instance()
{
	static HText h;
	return &h;
}

void HText::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		Visualization::Text* t = static_cast<Visualization::Text*> (target);
		t->setSelected(event->pos().x(),event->pos().x());
		t->scene()->updateTopLevelItems();
	}
	else InteractionHandler::mousePressEvent(target, event);
}

void HText::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	Visualization::Text* t = static_cast<Visualization::Text*> (target);
	t->setSelected(event->buttonDownPos(Qt::LeftButton).x(),event->pos().x());
	t->scene()->updateTopLevelItems();
	InteractionHandler::mouseMoveEvent(target, event);
}

void HText::focusOutEvent(Visualization::Item *, QFocusEvent *)
{
	Visualization::Text::resetSelected();
}

}
