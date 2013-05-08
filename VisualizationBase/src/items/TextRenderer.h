/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#pragma once

#include "../visualizationbase_api.h"

#include "Item.h"
#include "ItemMacros.h"
#include "TextStyle.h"
#include "ModelBase/src/nodes/Node.h"

namespace Visualization {

class VISUALIZATIONBASE_API TextRenderer : public Reflect<Item>
{
	ITEM_COMMON_CUSTOM_STYLENAME(TextRenderer, TextStyle)

	public:
		TextRenderer(Item* parent, const StyleType *style, const QString& text = QString());

		/**
		 * \brief Sets the text of this item to \a newText.
		 *
		 * Returns true if the text has been set, false otherwise.
		 */
		virtual bool setText(const QString& newText);

		/**
		 * \brief Returns the text that is currently cached in this TextRenderer item.
		 *
		 * This is the last string that was visualized by this TextRenderer. This could be different compared to the
		 * value of the text represented by this Item. For example if this is an instance of
		 * ItemWithNode<TextRenderer, Text> and the text of the node has changed, this change is not automatically
		 * reflected. Only when the TextRenderer is updated will the new text be cached.
		 */
		QString text();
		QString selectedText();

		bool isEditable();
		void setEditable(bool editable);

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

		virtual bool moveCursor(CursorMoveDirection dir = MoveDefault, QPoint reference = QPoint()) override;

		qreal textXOffset();
		qreal textYOffset();

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

		/**
		 * Returns the text of that is represented by the current item.
		 *
		 * This is an up-to-date value that may differ from the value of text(). E.G. If this is an instance of
		 * ItemWithNode<TextRenderer, Text> and the text of the node has just changed, this method will returned the
		 * current value of the node, whereas text() will still return the old value.
		 *
		 * When the TextRenderer is updated the value returned by currentText() is cached and will be subsequently
		 * returned by text() and visualized on the screen.
		 */
		virtual QString currentText() = 0;

	private:
		QStaticText staticText_;

		qreal textXOffset_; //Base line
		qreal textYOffset_;

		bool editable;

		const static int MIN_TEXT_WIDTH = 10;

		QRectF bound();
		QRectF bound(QFontMetrics &qfm);
};

inline QString TextRenderer::text() { return staticText_.text(); }
inline bool TextRenderer::isEditable() { return editable; }
inline void TextRenderer::setEditable(bool editable_) { editable = editable_; }

inline qreal TextRenderer::textXOffset() { return textXOffset_; }
inline qreal TextRenderer::textYOffset() { return textYOffset_; }

}
