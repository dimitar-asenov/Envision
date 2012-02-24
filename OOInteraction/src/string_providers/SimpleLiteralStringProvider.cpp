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
 * SimpleLiteralStringProvider.cpp
 *
 *  Created on: Feb 15, 2012
 *      Author: Dimitar Asenov
 */

#include "string_providers/SimpleLiteralStringProvider.h"
#include "string_components/StringComponents.h"

#include "VisualizationBase/headers/cursor/TextCursor.h"
#include "VisualizationBase/headers/items/Item.h"

namespace OOInteraction {

SimpleLiteralStringProvider::SimpleLiteralStringProvider(Visualization::Item* v)
: vis_(v)
{
}

int SimpleLiteralStringProvider::offset()
{
	if (!vis_ || !vis_->itemOrChildHasFocus()) return -1;

	auto tc = dynamic_cast<Visualization::TextCursor*> (vis_->scene()->mainCursor());

	return tc ? tc->caretPosition() : -1;
}

QString SimpleLiteralStringProvider::string()
{
	return stringFromComponenets(vis_);
}

void SimpleLiteralStringProvider::setOffset(int offset)
{
	if (!vis_) return;
	vis_->moveCursor( Visualization::Item::MoveRightOf, QPoint(-2,0)); // Just set the caret to the first position.

	// And then use the current cursor to set it to the correct position.
	auto tc = dynamic_cast<Visualization::TextCursor*> (vis_->scene()->mainCursor());
	tc->setCaretPosition(offset);
}

} /* namespace OOInteraction */
