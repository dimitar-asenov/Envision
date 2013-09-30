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

#include "handlers/HCommentDiagramShape.h"
#include "items/VCommentDiagram.h"
#include "items/VCommentDiagramShape.h"
#include "commands/CShapeSetProperty.h"

namespace Comments {

HCommentDiagramShape::HCommentDiagramShape()
{
	addCommand(new CShapeSetProperty());
}

HCommentDiagramShape* HCommentDiagramShape::instance()
{
	static HCommentDiagramShape h;
	return &h;
}

void HCommentDiagramShape::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(target);
	event->ignore();

	if(shape->diagram()->editing())
	{
		if(event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Delete)
		{
			event->accept();
			shape->diagram()->node()->removeShape(shape->node());
			// since the shape was selected, we need to clear it
			auto scene = target->scene();
			scene->clearFocus();
			scene->clearSelection();
			scene->setMainCursor(nullptr);
		}
	}

	if (!event->isAccepted())
		GenericHandler::keyPressEvent(target, event);
}

void HCommentDiagramShape::mousePressEvent(Visualization::Item* target, QGraphicsSceneMouseEvent *event)
{
	event->ignore();
	auto shape = dynamic_cast<VCommentDiagramShape*>(target);
	if(shape->diagram()->editing())
	{
		if(event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier)
		{
			event->accept();
			originalPos_ = shape->pos().toPoint();
			shape->setCursor(Qt::ClosedHandCursor);
		}
		else if(event->button() == Qt::RightButton)
		{
			event->accept();
			showCommandPrompt(target);
		}
	}

	if (!event->isAccepted())
		GenericHandler::mousePressEvent(target, event);
}

void HCommentDiagramShape::mouseReleaseEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *)
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(target);
	shape->setCursor(Qt::OpenHandCursor);
}

void HCommentDiagramShape::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if(event->buttons() & Qt::LeftButton)
	{
		auto shape = dynamic_cast<VCommentDiagramShape*>(target);
		shape->moveTo(originalPos_ + (event->scenePos() - event->buttonDownScenePos(Qt::LeftButton)).toPoint());
	}
}

void HCommentDiagramShape::mouseDoubleClickEvent(Visualization::Item*, QGraphicsSceneMouseEvent *)
{
}

void HCommentDiagramShape::hoverEnterEvent(Visualization::Item *target, QGraphicsSceneHoverEvent *)
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(target);
	if(shape->diagram()->editing())
		shape->setCursor(Qt::OpenHandCursor);
}

void HCommentDiagramShape::hoverLeaveEvent(Visualization::Item *target, QGraphicsSceneHoverEvent *)
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(target);
	shape->setCursor(Qt::ArrowCursor);
}

} /* namespace Comments */
