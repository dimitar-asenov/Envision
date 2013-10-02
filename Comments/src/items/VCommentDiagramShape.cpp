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

VCommentDiagramShape::VCommentDiagramShape(Item* parent, NodeType* node, const StyleType* style)
: Super(parent, node, style)
{
	setAcceptHoverEvents(true);
	text_ = new VText(this, node->label());
	setTextEditable(false);
	setZValue(1);
}

VCommentDiagram* VCommentDiagramShape::diagram()
{
	return dynamic_cast<VCommentDiagram*>(Item::parent());
}

void VCommentDiagramShape::determineChildren()
{
}

void VCommentDiagramShape::updateGeometry(int, int)
{
	setSize(node()->size());
	shapeColor_ = style()->colorFromName(node()->shapeColor());
	textColor_ = style()->colorFromName(node()->textColor());
	backgroundColor_ = style()->colorFromName(node()->backgroundColor());

	// TODO: consider shape as well?
	auto bound = text_->boundingRect();
	// align it both horizontally and vertically
	int x = node()->width() / 2 - bound.width() / 2;
	int y = node()->height() / 2 - bound.height() / 2;
	text_->setPos(x, y);
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
		// Temporarily assume a thicker painter with a different color for drawing the connector points
		QBrush brush(QColor("red"));
		painter->setPen(QPen(brush, 10));

		for(int i = 0; i < 16; ++i)
		{
			QPoint point = node()->getConnectorCoordinates(i);
			painter->drawPoint(point);
		}
	}
}

void VCommentDiagramShape::moveTo(QPoint pos)
{
	if(pos.x() < 0) pos.setX(0);
	if(pos.y() < 0) pos.setY(0);

	node()->model()->beginModification(node(), "Moving shape");
	node()->setX(pos.x());
	node()->setY(pos.y());
	node()->model()->endModification();
	setUpdateNeeded(StandardUpdate);

	// update all children
	// -> connectors use new shapes' positions
	// -> shapes text is no longer editable
	for(auto child : parent()->childItems())
		child->setUpdateNeeded(StandardUpdate);
}

void VCommentDiagramShape::setTextEditable(bool editable)
{
	text_->setEditable(editable);
}

} /* namespace Comments */
