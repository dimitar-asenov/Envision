/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
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

#include "HViewItem.h"

namespace Interaction {

HViewItem::HViewItem()
{
}

HViewItem* HViewItem::instance()
{
	static HViewItem h;
	return &h;
}

void HViewItem::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Return)
		GenericHandler::showCommandPrompt(target, "add ");
	else if (target->focusedChild() && event->modifiers() == Qt::ControlModifier
		&& (event->key() == Qt::Key_I || event->key() == Qt::Key_J
			|| event->key() == Qt::Key_K || event->key() == Qt::Key_L))
	{
		auto child = target->focusedChild();
		//TODO@cyril For some reason it does not work always without this
		target->scene()->setMainCursor(nullptr);
		QPoint pos;
		if (event->key() == Qt::Key_I) //Up
			pos = {(int)(child->xEndInParent() - child->widthInParent()/2), (int)child->y()};
		else if (event->key() == Qt::Key_K) //Down
			pos = {(int)(child->xEndInParent() - child->widthInParent()/2), (int)(child->yEndInParent() + 1)};
		else if (event->key() == Qt::Key_J) //Left
			pos = {(int)child->x(), (int)(child->yEndInParent() - child->heightInParent()/2)};
		else if (event->key() == Qt::Key_L) //Right
			pos = {(int)(child->xEndInParent() + 1), (int)(child->yEndInParent() - child->heightInParent()/2)};

		//Move the cursor, open a new command prompt
		target->moveCursor(Visualization::Item::MoveOnPosition, pos);
		GenericHandler::showCommandPrompt(target, "add ");
	}
	else GenericHandler::keyPressEvent(target, event);
}

} /* namespace Interaction */
