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

#include "VCommentDiagramConnector.h"
#include "VisualizationBase/src/items/ItemStyle.h"

using namespace Visualization;

namespace Comments {

ITEM_COMMON_DEFINITIONS(VCommentDiagramConnector, "item")

VCommentDiagramConnector::VCommentDiagramConnector(Item* parent, NodeType* node) : Super(parent, node, itemStyles().get())
{
	parent_ = dynamic_cast<VCommentDiagram*>(parent);
	Q_ASSERT(parent_ != nullptr);
}

void VCommentDiagramConnector::determineChildren(){}

void VCommentDiagramConnector::updateGeometry(int, int)
{
	// The connectors always connect two shapes which clearly encompass the connectors, therefore no need to compute
	// it here again.
	auto shape1 = parent_->node()->shapes()->at(node()->shape1());
	auto shape2 = parent_->node()->shapes()->at(node()->shape2());

	point1_ = shape1->pos()+shape1->getConnectorCoordinates(node()->point1());
	point2_ = shape2->pos()+shape2->getConnectorCoordinates(node()->point2());
	auto origin = QPoint(std::min(point1_.x(), point2_.x()), std::min(point1_.y(), point2_.y()));
	point1_ -= origin;
	point2_ -= origin;

	int dx = std::max(1., std::abs(point1_.x() - point2_.x()));
	int dy = std::max(1., std::abs(point1_.y() - point2_.y()));
	// make sure we get at least one pixel to draw inside!
	setSize(dx, dy);
}

void VCommentDiagramConnector::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget *)
{
	painter->setPen(QPen(Qt::black, 1.0));
	painter->drawLine(point1_, point2_);
}

} /* namespace Comments */
