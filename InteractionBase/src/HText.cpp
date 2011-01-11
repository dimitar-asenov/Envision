/***********************************************************************************************************************
 * HText.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "HText.h"

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
	InteractionHandler::mousePressEvent(target, event);
}

}
