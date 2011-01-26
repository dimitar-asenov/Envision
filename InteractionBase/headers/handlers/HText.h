/***********************************************************************************************************************
 * HText.h
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef HTEXT_H_
#define HTEXT_H_

#include "../interactionbase_api.h"

#include "GenericHandler.h"

namespace Interaction {

class INTERACTIONBASE_API HText : public GenericHandler
{
	public:
		virtual void keyPressEvent(Visualization::Item *target, QKeyEvent *event);

		virtual void mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseReleaseEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseDoubleClickEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void focusInEvent(Visualization::Item *target, QFocusEvent *event);
		virtual void focusOutEvent(Visualization::Item *target, QFocusEvent *event);
		static HText* instance();

	protected:
		HText();

		virtual void setNewText(Visualization::Item *target, const QString& newText);
		void moveCaret(Visualization::Item *target, QKeyEvent *event);
		void erase(Visualization::Item *target, bool forwards, bool onlyDeleteIfSelected);
		void insertText(Visualization::Item *target, const QString& textToInsert);

	private:
		bool doubleClick;
};

}

#endif /* HTEXT_H_ */
