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
 * TextCursorData.cpp
 *
 *  Created on: Jan 30, 2012
 *      Author: Dimitar Asenov
 */

#include "cursor/TextCursor.h"
#include "cursor/CursorShapeItem.h"

namespace Visualization {

TextCursor::TextCursor(TextRenderer* owner)
	: Cursor(owner, new CursorShapeItem()), selectionBegin_(0),  selectionEnd_(0), xBegin_(0), xEnd_(0)
{
}

TextRenderer* TextCursor::owner() const
{
	return static_cast<TextRenderer*> (Cursor::owner());
}

void TextCursor::setVisualizationSize(const QSize& size)
{
	CursorShapeItem* ci = static_cast<CursorShapeItem*> (visualization());
	ci->setCursorSize(size);
}

void TextCursor::update(const QFontMetrics& qfm)
{
	int xstart = qfm.width(owner()->text().left(selectionFirstIndex()));
	int xend = qfm.width(owner()->text().left(selectionLastIndex()));
	setX(xstart, xend);

	QSize cursorSize = QSize(2, owner()->height() - 2);
	setVisualizationSize(cursorSize);
	setRegion(QRect(position(), cursorSize));
}

void TextCursor::selectAll()
{
	setSelectedCharacters(0, owner()->text().length());
}

void TextCursor::setSelectedCharacters(int first, int last)
{
	selectionBegin_ = first;
	selectionEnd_ = last;
	if (!owner()->hasFocus()) owner()->setFocus();
	owner()->setUpdateNeeded();
}

void TextCursor::setX(int xBegin, int xEnd)
{
	xBegin_ = xBegin;
	xEnd_ = xEnd;

	int caretX = isCursorBeforeSelection() ? xBegin : xEnd;
	setPosition(QPointF(owner()->scenePos() + QPoint(caretX + owner()->textXOffset(), 2)).toPoint());

	CursorShapeItem* ci = static_cast<CursorShapeItem*> (visualization());
	ci->setCursorCenter(position());
}

void TextCursor::setSelectedByDrag(int xBegin, int xEnd)
{
	selectionBegin_ = 0;
	selectionEnd_ = 0;

	QFontMetrics qfm(owner()->style()->font());
	int width = 0;
	for (int i = 1; i <= owner()->text().length(); ++i)
	{
		int new_width = qfm.width(owner()->text().left(i));
		if ( xBegin > (new_width + width + 1) / 2 ) selectionBegin_++;
		if ( xEnd > (new_width + width + 1) / 2 ) selectionEnd_++;
		width = new_width;
	}

	owner()->setFocus();
	owner()->setUpdateNeeded();
}

bool TextCursor::isSame(Cursor* c)
{
	auto tc = dynamic_cast<TextCursor*>(c);
	if (tc)
		return tc->owner() == owner()
				&& tc->selectionBegin() == selectionBegin()
				&& tc->selectionEnd() == selectionEnd();

	return false;
}

int TextCursor::cursorAtX(int x) const
{
	int pos = 0;

	QFontMetrics qfm(owner()->style()->font());
	int width = 0;
	for (int i = 1; i <= owner()->text().length(); ++i)
	{
		int new_width = qfm.width(owner()->text().left(i));
		if ( x > (new_width + width + 1) / 2 ) pos++;
		width = new_width;
	}

	return pos;
}

} /* namespace Visualization */
