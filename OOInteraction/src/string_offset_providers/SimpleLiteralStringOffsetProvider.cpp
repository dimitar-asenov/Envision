/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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
 * SimpleLiteralStringOffsetProvider.cpp
 *
 *  Created on: Feb 15, 2012
 *      Author: Dimitar Asenov
 */

#include "string_offset_providers/SimpleLiteralStringOffsetProvider.h"

#include "VisualizationBase/src/cursor/TextCursor.h"

namespace OOInteraction {

SimpleLiteralStringOffsetProvider::SimpleLiteralStringOffsetProvider(Visualization::Item* v)
: StringOffsetProvider(v), vis_(v)
{
}

int SimpleLiteralStringOffsetProvider::offset(Qt::Key /*key*/)
{
	if (!vis_ || !vis_->itemOrChildHasFocus()) return -1;

	auto tc = dynamic_cast<Visualization::TextCursor*> (vis_->scene()->mainCursor());

	return tc ? tc->caretPosition() : -1;
}

void SimpleLiteralStringOffsetProvider::setOffset(int offset)
{
	if (!vis_) return;
	vis_->moveCursor( Visualization::Item::MoveOnPosition, QPoint(0,0)); // Just set the caret to the first position.

	// And then use the current cursor to set it to the correct position.
	auto tc = dynamic_cast<Visualization::TextCursor*> (vis_->scene()->mainCursor());
	tc->setCaretPosition(offset);
}

bool SimpleLiteralStringOffsetProvider::isIndivisible()
{
	auto v = dynamic_cast<Visualization::TextRenderer*> (vis_);
	if (!v && vis_)
	{
		auto ci = vis_->childItems();
		while (ci.length() == 1)
		{
			v =  dynamic_cast<Visualization::TextRenderer*> (ci.first());
			if (v) break;

			ci = ci.first()->childItems();
		}
	}

	return !v || v->text().length() != stringFromComponenets(vis_).length();
}

} /* namespace OOInteraction */
