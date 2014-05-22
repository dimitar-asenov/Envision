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

#include "VCommentDiagramConnector.h"
#include "VCommentDiagramShape.h"
#include "VCommentDiagram.h"

#include "VisualizationBase/src/items/ItemStyle.h"

using namespace Visualization;

namespace Comments {

ITEM_COMMON_DEFINITIONS(VCommentDiagramConnector, "item")

VCommentDiagramConnector::VCommentDiagramConnector(Item* parent, NodeType* node)
	: Super(parent, node, itemStyles().get())
{
	// draw connectors below shapes
	setZValue(0);
}

VCommentDiagram* VCommentDiagramConnector::diagram()
{
	return DCast<VCommentDiagram>(parent());
}

void VCommentDiagramConnector::determineChildren(){}

void VCommentDiagramConnector::updateGeometry(int, int)
{
	// The connectors always connect two shapes which clearly encompass the connectors, therefore no need to compute
	// it here again.
	auto startShape = diagram()->diagramShape(node()->startShape());
	auto endShape = diagram()->diagramShape(node()->endShape());

	startPoint_ = startShape->node()->pos() + startShape->node()->connectorPoint(node()->startPoint());
	endPoint_ = endShape->node()->pos() + endShape->node()->connectorPoint(node()->endPoint());
	auto origin = QPoint(std::min(startPoint_.x(), endPoint_.x()), std::min(startPoint_.y(), endPoint_.y()));
	startPoint_ -= origin;
	endPoint_ -= origin;

	// make sure we get at least one pixel to draw inside!
	int dx = std::max(1, std::abs(startPoint_.x() - endPoint_.x()));
	int dy = std::max(1, std::abs(startPoint_.y() - endPoint_.y()));
	setSize(dx, dy);
}

void VCommentDiagramConnector::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget *)
{
	painter->setPen(QPen(Qt::black, 1.0));
	painter->drawLine(startPoint_, endPoint_);
}

} /* namespace Comments */
