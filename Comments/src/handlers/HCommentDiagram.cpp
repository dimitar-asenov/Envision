/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "HCommentDiagram.h"
#include "../items/VCommentDiagram.h"
#include "../commands/CCreateShape.h"

#include "InteractionBase/src/prompt/Prompt.h"
#include "VisualizationBase/src/items/Item.h"

namespace Comments {

HCommentDiagram::HCommentDiagram()
{
	addCommand(new CCreateShape{});
}

HCommentDiagram* HCommentDiagram::instance()
{
	static HCommentDiagram h;
	return &h;
}

void HCommentDiagram::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	auto diagram = DCast<VCommentDiagram>(target);
	diagram->toolbar()->setDiagram(diagram);
	diagram->toggleEditing();

	if (event->button() == Qt::RightButton)
	{
		event->accept();

		if (event->modifiers() & Qt::ShiftModifier)
		{
			originalSize_ = diagram->sizeInLocal();
			resizing_ = true;
			diagram->setCursor(Qt::SizeAllCursor);
		}
		else
		{
			diagram->setLastRightClick(event->pos().toPoint());
			Interaction::Prompt::show(target);
		}
	}
	if (event->button() == Qt::LeftButton)
	{
		diagram->toolbar()->clearCurrentItem();
		if (!diagram->toolbar()->selectionMode())
		{
			event->accept();
			auto diagramNode = DCast<CommentDiagram>(target->node());
			newShape_ = new CommentDiagramShape{(int)event->pos().x(),
															(int)event->pos().y(), 1, 1, diagram->toolbar()->nextShapeToAdd_};
			diagramNode->beginModification("create shape");
			diagramNode->shapes()->append(newShape_);
			diagramNode->endModification();
			diagram->toolbar()->setSelectionMode(true);
		}
	}
}

void HCommentDiagram::mouseReleaseEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	auto diagram = DCast<VCommentDiagram>(target);
	if (!resizing_ && newShape_ != nullptr)
	{
		if (event->pos() == event->buttonDownPos(Qt::LeftButton))
		{
			newShape_->beginModification("Resizing shape");
			newShape_->setWidth(100);
			newShape_->setHeight(100);
			newShape_->endModification();
		}
		diagram->selectLastShape();
	}
	diagram->setCursor(Qt::ArrowCursor);
	resizing_ = false;
	newShape_ = nullptr;
}

void HCommentDiagram::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	auto diagram = DCast<VCommentDiagram>(target);
	if (resizing_)
	{
		QPointF diff = event->pos() - event->buttonDownPos(Qt::RightButton);
		QSize newSize(originalSize_.width() + diff.x(), originalSize_.height() + diff.y());

		diagram->node()->beginModification("resize diagram");
		diagram->node()->setSize(newSize);
		diagram->node()->endModification();
		diagram->setUpdateNeeded(Visualization::Item::StandardUpdate);
	}
	else if (newShape_ != nullptr)
	{
		QPointF diff = event->pos() - event->buttonDownPos(Qt::LeftButton);
		newShape_->beginModification("Resizing shape");
		newShape_->setWidth(diff.x());
		newShape_->setHeight(diff.y());

		if (event->pos().x() < event->buttonDownPos(Qt::LeftButton).x())
		{
			newShape_->setX(event->pos().x());
			newShape_->setWidth(event->buttonDownPos(Qt::LeftButton).x() - event->pos().x());
		}
		if (event->pos().y() < event->buttonDownPos(Qt::LeftButton).y())
		{
			newShape_->setY(event->pos().y());
			newShape_->setHeight(event->buttonDownPos(Qt::LeftButton).y() - event->pos().y());
		}
		newShape_->endModification();
		diagram->selectLastShape();
	}
}

}
