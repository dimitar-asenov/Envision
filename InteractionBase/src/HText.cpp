/***********************************************************************************************************************
 * HText.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "HText.h"

#include <QtCore/QDebug>

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
	qDebug() << "here";
	Visualization::Text* t = static_cast<Visualization::Text*> (target);
	t->setSelected(event->pos().x(),event->pos().x()+1);
	t->updateSubtreeState();
	t->update();
}

}
