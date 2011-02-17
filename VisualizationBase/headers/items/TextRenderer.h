/***********************************************************************************************************************
 * TextRenderer.h
 *
 *  Created on: Jan 12, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include "../visualizationbase_api.h"

#include "Item.h"
#include "ItemMacros.h"
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Node.h"

namespace Visualization {

class VISUALIZATIONBASE_API TextRenderer : public Item
{
	ITEM_COMMON_CUSTOM_STYLENAME(TextRenderer, TextStyle)

	private:
			QString text;

			qreal xOffset;
			qreal yOffset;

			bool editable;

			/**
			 * selectionBegin and selectionEnd indicate the character positions at which the selection begins and ends. If
			 * the user selected the text by dragging from right to left then selectionEnd < selectionBegin.
			 *
			 * If the entire text is selected then selectionBegin=0 and selectionEnd=text.length() (or vice versa).
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
			virtual void updateGeometry(int availableWidth, int availableHeight);

		public:
			TextRenderer(Item* parent, const TextStyle *style, const QString& text = QString());

			/**
			 * Sets the text of this item.
			 *
			 * Returns true if the text has been set, false otherwise.
			 *
			 * @param newText
			 * 				the string to set as the new text.
			 */
			virtual bool setText(const QString& newText);
			QString getText();
			QString getSelectedText();

			bool isEditable();
			void setEditable(bool editable);

			virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

			void selectAll();
			void setSelectedByDrag(int xBegin, int xEnd);
			void setSelectedCharacters(int first, int last);
			void setCaretPosition(int beforeCharacter);

			static int caretPosition();
			static int selectionFirstInxed();
			static int selectionLastIndex();

};

}

#endif /* TEXTRENDERER_H_ */
