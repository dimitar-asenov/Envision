/***********************************************************************************************************************
 * Text.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TEXT_H_
#define TEXT_H_

#include "../visualizationbase_api.h"

#include "Item.h"
#include "TextStyle.h"
#include "../VisualizationException.h"

#include <QtGui/QStaticText>

namespace Visualization {

class VISUALIZATIONBASE_API Text : public Item
{
	ITEM_COMMON(Text, Item)

	private:
		QString text;

		qreal xOffset;
		qreal yOffset;

		/**
		 * This is the text item that is currently selected. If this is NULL a text item is not currently selected. If the
		 * current item is equal to the selected item, then additional drawing takes place. This is the vertical caret.
		 * The selected part of the text is drawn using a different font specified in the style.
		 */
		static Text* selected;

		/**
		 * selectionBegin and selectionEnd indicate the character positions at which the selection begins and ends. If
		 * the user selected the text by dragging from right to left then selectionEnd < selectionBegin.
		 */
		static int selectionBegin;
		static int selectionEnd;

		/**
		 * selectionXBegin and selectionXEnd indicate the beginning and end (along the X direction) where the selection
		 * background must be drawn.
		 */
		static int selectionXBegin;
		static int selectionXEnd;

		static int caretX;

	protected:
		virtual void determineChildren();
		virtual void updateState();

	public:
		Text(Item* parent, const QString& text = QString());
		Text(Item* parent, const TextStyle *style, const QString& text = QString());

		void setText(const QString& newText);

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

inline void Text::setText(const QString& newText) { text = newText; setUpdateNeeded(); }

}

#endif /* TEXT_H_ */
