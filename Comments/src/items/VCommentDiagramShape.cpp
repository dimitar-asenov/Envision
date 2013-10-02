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

#include "VCommentDiagram.h"
#include "VCommentDiagramShape.h"
#include "VisualizationBase/src/items/ItemStyle.h"

using namespace Visualization;

namespace Comments {

ITEM_COMMON_DEFINITIONS(VCommentDiagramShape, "item")

int VCommentDiagramShape::resizePointWidth = 5;

VCommentDiagramShape::VCommentDiagramShape(Item* parent, NodeType* node, const StyleType* style)
: Super(parent, node, style)
{
	text_ = new VText(this, node->label());

	// set width and height for all resize rects, these don't change
	int penWidth = 5;
	for(int i = 0; i < 4; ++i)
	{
		resizeRects_[i].setWidth(penWidth);
		resizeRects_[i].setHeight(penWidth);
	}

	setAcceptHoverEvents(true);
	setZValue(1);
	node->updateConnectorPoints();
	setTextEditable(diagram()->editing());
}

VCommentDiagram* VCommentDiagramShape::diagram()
{
	auto diagram = dynamic_cast<VCommentDiagram*>(Item::parent());
	Q_ASSERT(diagram != nullptr);
	return diagram;
}

void VCommentDiagramShape::determineChildren()
{
}

void VCommentDiagramShape::updateGeometry(int, int)
{
	setSize(node()->size());
	shapeColor_ = style()->colorFromName(node()->shapeColor());
	backgroundColor_ = style()->colorFromName(node()->backgroundColor());
	text_->setStyle(VText::itemStyles().get(node()->textColor()));

	// TODO: consider shape as well?
	auto bound = text_->boundingRect();
	// align it both horizontally and vertically
	int x = node()->width() / 2 - bound.width() / 2;
	int y = node()->height() / 2 - bound.height() / 2;
	text_->setPos(x, y);

	node()->updateConnectorPoints();
	updateResizeRects();
}

void VCommentDiagramShape::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget *)
{
	// rectangle to draw the shape in
	QRect rect(0, 0, width(), height());
	painter->setPen(shapeColor_);
	painter->setBrush(QBrush(backgroundColor_));

	switch(node()->shapeType())
	{
		default:
		case Rectangle:
			painter->drawRect(rect);
			break;

		case Ellipse:
			painter->drawEllipse(rect);
			break;

		case Diamond:
			QPoint points[4] = {
				QPoint(width()/2, 0),
				QPoint(width(),   height()/2),
				QPoint(width()/2, height()),
				QPoint(0,         height()/2)
			};
			painter->drawConvexPolygon(points, 4);
			break;
	}

	if(diagram()->editing())
	{
		if(diagram()->showConnectorPoints())
		{
			// Temporarily assume a thicker painter with a different color for drawing the connector points
			QBrush brush(QColor("red"));
			painter->setPen(QPen(brush, 10));

			const QPoint* points = node()->connectorPoints();
			for(int i = 0; i < 16; ++i)
			{
				painter->drawPoint(points[i]);
			}
		}
		else
		{
			for(int i = 0; i < 4; ++i)
				painter->fillRect(resizeRects_[i], Qt::black);
		}
	}
}

void VCommentDiagramShape::updateResizeRects()
{
	resizeRects_[0].setRect(0,                          0,                           resizePointWidth, resizePointWidth);
	resizeRects_[1].setRect(width() - resizePointWidth, 0,                           resizePointWidth, resizePointWidth);
	resizeRects_[2].setRect(width() - resizePointWidth, height() - resizePointWidth, resizePointWidth, resizePointWidth);
	resizeRects_[3].setRect(0,                          height() - resizePointWidth, resizePointWidth, resizePointWidth);
}

void VCommentDiagramShape::moveBy(QPoint pos)
{
	QPoint dest(node()->pos() + pos);
	if(dest.x() < 0) dest.setX(0);
	if(dest.y() < 0) dest.setY(0);

	node()->model()->beginModification(node(), "Moving shape");
	node()->setX(dest.x());
	node()->setY(dest.y());
	node()->model()->endModification();

	// update all children
	// -> connectors use new shapes' positions
	// -> shapes text is no longer editable
	for(auto child : parent()->childItems())
		child->setUpdateNeeded(StandardUpdate);
}

void VCommentDiagramShape::resizeBy(QSize size)
{
	QSize dest(node()->size() + size);
	if(dest.width() < 0) dest.setWidth(0);
	if(dest.height() < 0) dest.setHeight(0);

	node()->model()->beginModification(node(), "Resizing shape");
	node()->setWidth(dest.width());
	node()->setHeight(dest.height());
	node()->model()->endModification();

	for(auto child : parent()->childItems())
		child->setUpdateNeeded(StandardUpdate);
}

void VCommentDiagramShape::setTextEditable(bool editable)
{
	text_->setEditable(editable);
}

VCommentDiagramResizeRect VCommentDiagramShape::hitsResizeRects(QPoint pos) const
{
	if(resizeRects_[0].contains(pos)) return RECT_TOP_LEFT;
	if(resizeRects_[1].contains(pos)) return RECT_TOP_RIGHT;
	if(resizeRects_[2].contains(pos)) return RECT_BOTTOM_RIGHT;
	if(resizeRects_[3].contains(pos)) return RECT_BOTTOM_LEFT;

	return RECT_NONE;
}

void VCommentDiagramShape::handleResize(enum VCommentDiagramResizeRect rect, QPoint diff)
{
	switch(rect)
	{
	case RECT_NONE:
		return;
	case RECT_TOP_LEFT:
		moveBy(diff);
		resizeBy(QSize(-diff.x(), -diff.y()));
		return;
	case RECT_TOP_RIGHT:
		moveBy(QPoint(0, diff.y()));
		resizeBy(QSize(diff.x(), -diff.y()));
		return;
	case RECT_BOTTOM_RIGHT:
		resizeBy(QSize(diff.x(), diff.y()));
		return;
	case RECT_BOTTOM_LEFT:
		moveBy(QPoint(diff.x(), 0));
		resizeBy(QSize(-diff.x(), diff.y()));
		return;
	}
}

} /* namespace Comments */
