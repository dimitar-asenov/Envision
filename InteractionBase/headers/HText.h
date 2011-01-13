/***********************************************************************************************************************
 * HText.h
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef HTEXT_H_
#define HTEXT_H_

#include "interactionbase_api.h"

#include "GenericHandler.h"

namespace Interaction {

class INTERACTIONBASE_API HText : public GenericHandler
{
	protected:
		HText();

	public:
		virtual void keyPressEvent(Visualization::Item *target, QKeyEvent *event);

		virtual void mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void focusOutEvent(Visualization::Item *target, QFocusEvent *event);
		static HText* instance();
};

}

#endif /* HTEXT_H_ */
