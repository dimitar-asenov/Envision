/***********************************************************************************************************************
 * HText.h
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef HTEXT_H_
#define HTEXT_H_

#include "interactionbase_api.h"

#include "VisualizationBase/headers/items/Item.h"
#include "VisualizationBase/headers/items/Text.h"
#include "VisualizationBase/headers/InteractionHandler.h"

namespace Interaction {

class INTERACTIONBASE_API HText : public Visualization::InteractionHandler
{
	protected:
		HText();

	public:
		virtual void mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		static HText* instance();
};

}

#endif /* HTEXT_H_ */
