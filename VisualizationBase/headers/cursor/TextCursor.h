/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

/*
 * TextCursor.h
 *
 *  Created on: Jan 30, 2012
 *      Author: Dimitar Asenov
 */

#ifndef VisualizationBase_TEXTCURSOR_H_
#define VisualizationBase_TEXTCURSOR_H_

#include "../visualizationbase_api.h"
#include "../items/TextRenderer.h"
#include "Cursor.h"

namespace Visualization {

class VISUALIZATIONBASE_API TextCursor : public Cursor {

	public:
		TextCursor(TextRenderer* owner);

		int selectionBegin();
		int selectionEnd();
		int xBegin();
		int xEnd();

		void setVisualizationSize(const QSize& size);
		void setX(int xBegin, int xEnd);
		void setSelection(int selectionBegin, int selectionEnd);

		int selectionFirstIndex();
		int selectionLastIndex();
		bool hasSelection();
		int caretPosition();

		void selectAll();
		void setSelectedCharacters(int first, int last);
		void setSelectedByDrag(int xBegin, int xEnd);
		void setCaretPosition(int beforeCharacter);

		TextRenderer* owner();

		bool isCursorBeforeSelection();

		void update(const QFontMetrics& qfm);

	private:
		/**
		 * \brief selectionBegin_ and selectionEnd_ indicate the currently selected characters in a text.
		 *
		 * If the text has been selected from right to left then selectionEnd_ < selectionBegin_. If the entire
		 * text is selected then selectionBegin_ and selectionEnd_ will have the values 0 and text.length().
		 */
		int selectionBegin_;
		int selectionEnd_; /**< \sa selectionBegin_ */

		/**
		 * \brief xBegin_ and xEnd_ indicate the beginning and end (along the X coordinate) where the selection background
		 *        must be drawn.
		 */
		int xBegin_;
		int xEnd_; /**< \sa xBegin_ */
};

inline int TextCursor::selectionBegin() { return selectionBegin_; }
inline int TextCursor::selectionEnd() { return selectionEnd_; }
inline int TextCursor::xBegin() { return xBegin_; }
inline int TextCursor::xEnd() { return xEnd_; }

inline void TextCursor::setSelection(int selectionBegin, int selectionEnd)
	{ selectionBegin_ = selectionBegin; selectionEnd_ = selectionEnd; }

inline int TextCursor::selectionFirstIndex()
	{ return selectionBegin_ < selectionEnd_ ? selectionBegin_ : selectionEnd_; }
inline int TextCursor::selectionLastIndex()
	{ return selectionBegin_ > selectionEnd_ ? selectionBegin_ : selectionEnd_; }
inline bool TextCursor::isCursorBeforeSelection() { return selectionEnd_ < selectionBegin_; }

inline void TextCursor::setCaretPosition(int beforeCharacter) { setSelectedCharacters(beforeCharacter,beforeCharacter);}
inline int TextCursor::caretPosition() { return selectionEnd_; }

inline bool TextCursor:: hasSelection() { return selectionEnd_ != selectionBegin_; }


} /* namespace Visualization */
#endif /* VisualizationBase_TEXTCURSOR_H_ */
