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
#include "TextStyle.h"
#include "ModelBase/headers/nodes/Node.h"

namespace Visualization {

template <class T>
class VISUALIZATIONBASE_API TextRenderer : public T
{
	private:
			QString text;

			qreal xOffset;
			qreal yOffset;

			bool editable;

			/**
			 * This is the text item that is currently selected. If this is NULL a text item is not currently selected. If the
			 * current item is equal to the selected item, then additional drawing takes place. This is the vertical caret.
			 * The selected part of the text is drawn using a different font specified in the style.
			 */
			static TextRenderer* selected;

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

			void setText(const QString& newText);

		public:
			TextRenderer(Item* parent, const TextStyle *style, const QString& text = QString());
			TextRenderer(Item* parent, Model::Node *node, const TextStyle *style);

			const TextStyle* style() const;
			virtual void setStyle(const ItemStyle* style);

			QString getText();
			QString getSelectedText();

			bool isEditable();
			void setEditable(bool editable);

			virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

			void selectAll();
			void setSelectedByDrag(int xBegin, int xEnd);
			void setSelectedCharacters(int first, int last);
			void setCaretPosition(int beforeCharacter);
			static void resetSelected();

			static int caretPosition();
			static int selectionFirstInxed();
			static int selectionLastIndex();

};

}

#endif /* TEXTRENDERER_H_ */
