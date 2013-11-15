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

#include "VisualizationBase/src/items/Item.h"
#include "handlers/HCommentDiagram.h"
#include "items/VCommentDiagram.h"
#include "commands/CCreateShape.h"

namespace Comments {

HCommentDiagram::HCommentDiagram()
{
	addCommand(new CCreateShape());
}

HCommentDiagram* HCommentDiagram::instance()
{
	static HCommentDiagram h;
	return &h;
}

void HCommentDiagram::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	auto diagram = DCast<VCommentDiagram>(target);
	event->ignore();

	if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_E)
	{
		event->accept();
		diagram->toggleEditing();
	}
	else if(event->key() == Qt::Key_Shift || event->modifiers() & Qt::Key_Shift)
	{
		event->accept();
		diagram->setShowConnectorPoints(true);
	}

	if (!event->isAccepted())
		GenericHandler::keyPressEvent(target, event);
}

void HCommentDiagram::keyReleaseEvent(Visualization::Item *target, QKeyEvent *event)
{
	auto diagram = DCast<VCommentDiagram>(target);

	if(diagram->showConnectorPoints() && !(event->modifiers() & Qt::Key_Shift))
	{
		event->accept();
		diagram->setShowConnectorPoints(false);
	}
}

void HCommentDiagram::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	auto diagram = DCast<VCommentDiagram>(target);
	event->ignore();

	if(diagram->editing() && event->button() == Qt::RightButton)
	{
		event->accept();

		if(event->modifiers() & Qt::ShiftModifier)
		{
			originalSize_ = diagram->size();
			resizing_ = true;
			diagram->setCursor(Qt::SizeAllCursor);
		}
		else
		{
			diagram->setLastRightClick(event->pos().toPoint());
			showCommandPrompt(target);
		}
	}
}

void HCommentDiagram::mouseReleaseEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *)
{
	auto diagram = DCast<VCommentDiagram>(target);
	diagram->setCursor(Qt::ArrowCursor);
	resizing_ = false;
}

void HCommentDiagram::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if(resizing_)
	{
		auto diagram = DCast<VCommentDiagram>(target);
		QPointF diff = event->pos() - event->buttonDownPos(Qt::RightButton);
		QSize newSize(originalSize_.width() + diff.x(), originalSize_.height() + diff.y());

		diagram->node()->beginModification("resize diagram");
		diagram->node()->setSize(newSize);
		diagram->node()->endModification();
		diagram->setUpdateNeeded(Visualization::Item::StandardUpdate);
	}
}

} /* namespace Comments */
