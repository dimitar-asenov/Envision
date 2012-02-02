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
 * HLayoutProvider.cpp
 *
 *  Created on: Jan 30, 2012
 *      Author: Dimitar Asenov
 */

#include "handlers/HLayoutProvider.h"

#include "VisualizationBase/headers/items/LayoutProviderBase.h"
#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/cursor/LayoutCursor.h"

#include "ModelBase/headers/Model.h"

#include <QtGui/QApplication>

namespace Interaction {

HLayoutProvider::HLayoutProvider()
{
}

HLayoutProvider* HLayoutProvider::instance()
{
	static HLayoutProvider h;
	return &h;
}

void HLayoutProvider::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	if ( ! (event->modifiers() & Qt::ControlModifier))
	{
		switch (event->key())
		{
			case Qt::Key_Up:
				moveCursor(target, event);
				break;
			case Qt::Key_Down:
				moveCursor(target, event);
				break;
			case Qt::Key_Left:
				moveCursor(target, event);
				break;
			case Qt::Key_Right:
				moveCursor(target, event);
				break;

			default:
				GenericHandler::keyPressEvent(target, event);
				break;
		}

	}
	else GenericHandler::keyPressEvent(target, event);

}

void HLayoutProvider::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
//	Visualization::LayoutProviderBase* tr = static_cast<Visualization::LayoutProviderBase*> (target);
//
//	if ( event->modifiers() == 0 && event->button() == Qt::LeftButton )
//	{
//		tr->correspondingSceneCursor<Visualization::LayoutCursor>()->setSelectedByDrag(event->pos().x(), event->pos().x());
//	}

	GenericHandler::mousePressEvent(target, event);
}

void HLayoutProvider::focusInEvent(Visualization::Item *target, QFocusEvent *event)
{
//	Visualization::LayoutProviderBase* tr = static_cast<Visualization::LayoutProviderBase*> (target);
//	Visualization::LayoutCursor* cv = new Visualization::LayoutCursor(tr);
//	cv->setVisualizationSize(QSize(5,5));
//	tr->scene()->setMainCursor(cv);
//
//	GenericHandler::FocusDirection dir = GenericHandler::focusDirection();
//
//	int size = tr->text().length();
//
//	if (size > 0)
//	{
//		// Here we choose which child to focus.
//		if (dir == GenericHandler::FROM_LEFT)
//			tr->correspondingSceneCursor<Visualization::LayoutCursor>()->setCaretPosition(0);
//		else if (dir == GenericHandler::FROM_RIGHT)
//			tr->correspondingSceneCursor<Visualization::LayoutCursor>()->setCaretPosition(size);
//	}

	GenericHandler::focusInEvent(target, event);
}

// TextRenderer interface routines.
void HLayoutProvider::moveCursor(Visualization::Item */*target*/, QKeyEvent */*event*/)
{
//	Visualization::LayoutProviderBase* tr = static_cast<Visualization::LayoutProviderBase*> (target);
//
//	switch (event->key())
//	{
//		case Qt::Key_Up:
//			//...
//			break;
//		case Qt::Key_Down:
//			//...
//			break;
//		case Qt::Key_Left:
//			{
//				int position = tr->correspondingSceneCursor<Visualization::LayoutCursor>()->caretPosition();
//				if ( tr->text().isEmpty() || position <= 0) event->ignore();
//				else tr->correspondingSceneCursor<Visualization::LayoutCursor>()->setCaretPosition(position - 1);
//			}
//			break;
//		case Qt::Key_Right:
//			{
//				int position = tr->correspondingSceneCursor<Visualization::LayoutCursor>()->caretPosition();
//				int size = tr->text().size();
//				if ( tr->text().isEmpty() || position >= size) event->ignore();
//				else tr->correspondingSceneCursor<Visualization::LayoutCursor>()->setCaretPosition(position + 1);
//			}
//			break;
//	}
}

}
