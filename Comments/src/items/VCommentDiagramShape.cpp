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

#include "VCommentDiagram.h"
#include "VCommentDiagramShape.h"
#include "VisualizationBase/src/items/ItemStyle.h"

using namespace Visualization;

namespace Comments {

ITEM_COMMON_DEFINITIONS(VCommentDiagramShape, "item")

VCommentDiagramShape::VCommentDiagramShape(Item* parent, NodeType* node, const StyleType* style)
	: Super{parent, node, style}, text_{new VCommentText{this, node->label()}}
{
	setAcceptHoverEvents(true);
	setZValue(1);
}

inline const std::array<const QRect, 4> VCommentDiagramShape::resizeRects() const
{
	const int size = 5;
	return {{QRect{0,								0,								size, size},
				QRect{widthInLocal() - size,	0,								size, size},
				QRect{widthInLocal() - size,	heightInLocal() - size,	size, size},
				QRect{0,								heightInLocal() - size,	size, size}}};
}

VCommentDiagram* VCommentDiagramShape::diagram()
{
	auto diagram = DCast<VCommentDiagram>(parent());
	Q_ASSERT(diagram != nullptr);
	return diagram;
}

void VCommentDiagramShape::determineChildren()
{
	outlineColor_ = style()->colorFromName(node()->shapeColor());
	fillColor_ = style()->colorFromName(node()->backgroundColor());
	text_->setStyle(VText::itemStyles().get(node()->textColor()));
	text_->setEditable(true);
	outlineType_ = node()->outlineType();
	outlineSize_ = node()->outlineSize();
}

void VCommentDiagramShape::updateGeometry(int, int)
{
	setSize(node()->size());

	auto bound = text_->sizeInParent();
	// align it both horizontally and vertically
	int x = node()->width() / 2 - bound.width() / 2;
	int y = node()->height() / 2 - bound.height() / 2;
	text_->setPos(x, y);
}

void VCommentDiagramShape::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget *)
{
	// rectangle to draw the shape in
	QRectF rect{outlineSize_/2.0, outlineSize_/2.0,
				(qreal) widthInLocal()-outlineSize_, (qreal) heightInLocal()-outlineSize_};
	QPen pen;
	pen.setColor(outlineColor_);
	pen.setStyle(outlineType_);
	pen.setWidth(outlineSize_);
	pen.setJoinStyle(Qt::RoundJoin);
	painter->setPen(pen);
	painter->setBrush(QBrush{fillColor_});

	switch (node()->shapeType())
	{
		default:
		case CommentDiagramShape::ShapeType::Rectangle:
			painter->drawRect(rect);
			break;

		case CommentDiagramShape::ShapeType::Ellipse:
			painter->drawEllipse(rect);
			break;

		case CommentDiagramShape::ShapeType::Diamond:
			QPointF points[4] = {
				{widthInLocal()/2.0, outlineSize_/2.0},
				{widthInLocal()-outlineSize_/2.0, heightInLocal()/2.0},
				{widthInLocal()/2.0, heightInLocal()-outlineSize_/2.0},
				{outlineSize_/2.0, heightInLocal()/2.0}
			};
			painter->drawConvexPolygon(points, 4);
			break;
	}

	if (diagram()->showConnectorPoints())
	{
		// Temporarily assume a thicker painter with a different color for drawing the connector points
		QBrush blackBrush(Qt::black), redBrush(Qt::red);

		int shapeIndex = node()->index();
		auto last = diagram()->lastConnector();
		for (int i = 0; i < 16; ++i)
		{
			if (last.first == shapeIndex && last.second == i)
			{
				painter->setBrush(blackBrush);
				painter->setPen(QPen{Qt::black});
				painter->drawEllipse(node()->connectorPoint(i), 7, 7);
			}
			else
			{
				painter->setBrush(redBrush);
				painter->setPen(QPen{Qt::red});
				painter->drawEllipse(node()->connectorPoint(i), 3, 3);
			}
		}
	}

	if (this->isSelected())
		for (auto r : resizeRects()) painter->fillRect(r, Qt::black);
}

VCommentDiagramResizeRect VCommentDiagramShape::hitsResizeRects(QPoint pos) const
{
	int rect = -1;
	int manhattan = 25;


	int i = 0;
	for (auto r : resizeRects())
	{
		auto m = (pos - r.center()).manhattanLength();
		if (m < manhattan)
		{
			rect = i;
			manhattan = m;
		}

		++i;
	}

	switch (rect)
	{
		case 0: return RECT_TOP_LEFT;
		case 1: return RECT_TOP_RIGHT;
		case 2: return RECT_BOTTOM_RIGHT;
		case 3: return RECT_BOTTOM_LEFT;
	}

	return RECT_NONE;
}

}
