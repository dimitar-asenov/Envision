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

#include "HCommentDiagramShape.h"
#include "../items/VCommentDiagram.h"
#include "../commands/CShapeSetProperty.h"

#include "InteractionBase/src/prompt/Prompt.h"

namespace Comments {

HCommentDiagramShape::HCommentDiagramShape()
{
	addCommand(new CShapeSetProperty{});
}

HCommentDiagramShape* HCommentDiagramShape::instance()
{
	static HCommentDiagramShape h;
	return &h;
}

void HCommentDiagramShape::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	auto shape = DCast<VCommentDiagramShape>(target);
	event->ignore();

	if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Delete)
	{
		event->accept();
		shape->diagram()->node()->beginModification("Remove shape");
		shape->diagram()->node()->removeShape(shape->node());
		shape->diagram()->node()->endModification();
	}

	if (!event->isAccepted())
		GenericHandler::keyPressEvent(target, event);
}

void HCommentDiagramShape::mousePressEvent(Visualization::Item* target, QGraphicsSceneMouseEvent *event)
{
	event->ignore();
	auto vShape = DCast<VCommentDiagramShape>(target);
	auto vDiagram = vShape->diagram();
	vDiagram->toolbar()->setDiagram(vDiagram);
	vDiagram->toggleEditing();
	if (vDiagram->toolbar()->selectionMode())
	{
		vDiagram->toolbar()->setCurrentShape(target);
		vDiagram->node()->beginModification("shape");

		QPoint clickPos(event->pos().toPoint());

		if (event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier)
		{
			event->accept();
			clickedRect_ = vShape->hitsResizeRects(clickPos);

			if (clickedRect_ != RECT_NONE)
				vShape->setCursor(Qt::SizeAllCursor);
			else
				vShape->setCursor(Qt::ClosedHandCursor);
			shapePosition_ = vShape->node()->pos();
			shapeSize_ = vShape->node()->size();
		}
		if (event->button() == Qt::LeftButton && vDiagram->showConnectorPoints())
		{
			event->accept();
			int connectorIndex = vShape->node()->connectorPointNear(clickPos);
			if (connectorIndex != -1)
			{
				int shapeIndex = vShape->node()->index();
				auto last = vDiagram->lastConnector();

				if (last.first == shapeIndex && last.second == connectorIndex)
				{
					// unselect connector if selected a second time
					shapeIndex = -1;
					connectorIndex = -1;
				}
				// make sure connectors are between two *different* shapes
				else if (last.first != -1 && last.second != -1 && last.first != shapeIndex)
				{
					vDiagram->node()->connectors()->append(
								new CommentDiagramConnector{last.first, last.second, shapeIndex, connectorIndex});
					shapeIndex = -1;
					connectorIndex = -1;
				}

				vDiagram->setLastConnector(shapeIndex, connectorIndex);
			}
		}
		else if (event->button() == Qt::RightButton && event->modifiers() == Qt::NoModifier)
		{
			event->accept();
			Interaction::Prompt::show(target);
		}

		vDiagram->node()->endModification();
	}

	if (!event->isAccepted())
		GenericHandler::mousePressEvent(target, event);
}

void HCommentDiagramShape::mouseReleaseEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *)
{
	auto shape = DCast<VCommentDiagramShape>(target);
	shape->setCursor(Qt::OpenHandCursor);
	clickedRect_ = RECT_NONE;
}

void HCommentDiagramShape::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	auto shape = DCast<VCommentDiagramShape>(target);
	if (event->buttons() & Qt::LeftButton)
	{
		//there are some rounding errors if zoomed in, if we simply compute the delta to the last known position.
		QPoint diff((event->scenePos() - event->buttonDownScenePos(Qt::LeftButton)).toPoint());

		switch (clickedRect_)
		{
			case RECT_NONE:
				moveBy(shape, diff);
				break;
			case RECT_TOP_LEFT:
				moveBy(shape, diff);
				resizeBy(shape, QSize(-diff.x(), -diff.y()));
				break;
			case RECT_TOP_RIGHT:
				moveBy(shape, QPoint(0, diff.y()));
				resizeBy(shape, QSize(diff.x(), -diff.y()));
				break;
			case RECT_BOTTOM_RIGHT:
				resizeBy(shape, QSize(diff.x(), diff.y()));
				break;
			case RECT_BOTTOM_LEFT:
				moveBy(shape, QPoint(diff.x(), 0));
				resizeBy(shape, QSize(-diff.x(), diff.y()));
				break;
		}

		shape->setUpdateNeeded(VCommentDiagramShape::StandardUpdate);
	}
}

void HCommentDiagramShape::hoverMoveEvent(Visualization::Item *target, QGraphicsSceneHoverEvent *)
{
	auto shape = DCast<VCommentDiagramShape>(target);
	shape->setCursor(Qt::OpenHandCursor);
}

void HCommentDiagramShape::hoverLeaveEvent(Visualization::Item *target, QGraphicsSceneHoverEvent *)
{
	auto shape = DCast<VCommentDiagramShape>(target);
	shape->setCursor(Qt::ArrowCursor);
}

void HCommentDiagramShape::moveBy(VCommentDiagramShape* shape, QPoint pos)
{
	QPoint dest(shapePosition_ + pos);
	if (dest.x() < 0) dest.setX(0);
	if (dest.y() < 0) dest.setY(0);

	shape->node()->beginModification("Moving shape");
	shape->node()->setX(dest.x());
	shape->node()->setY(dest.y());
	shape->node()->endModification();
}

void HCommentDiagramShape::resizeBy(VCommentDiagramShape* shape, QSize size)
{
	QSize dest(shapeSize_ + size);
	if (dest.width() < 0) dest.setWidth(0);
	if (dest.height() < 0) dest.setHeight(0);

	shape->node()->beginModification("Resizing shape");
	shape->node()->setWidth(dest.width());
	shape->node()->setHeight(dest.height());
	shape->node()->endModification();
}

}
