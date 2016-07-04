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
#include "VCommentDiagramConnector.h"

#include "VisualizationBase/src/items/Line.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"
#include "VisualizationBase/src/shapes/Shape.h"

using namespace Visualization;

namespace Comments {

DEFINE_ITEM_COMMON(VCommentDiagram, "item")

VCommentDiagram::VCommentDiagram(Item* parent, NodeType* node)
	: Super{parent, node, itemStyles().get()}
{}

void VCommentDiagram::determineChildren()
{
	Visualization::Item::synchronizeCollections(this, node()->shapes()->nodes(), shapes_,
		[](Model::Node* node, Item* item){return item->node() == node;},
		[](Item* parent, Model::Node* node)
			{return new VCommentDiagramShape{parent, static_cast<CommentDiagramShape*>(node)};},
		[](Item*, Model::Node*, VCommentDiagramShape*&){return false;});

	Visualization::Item::synchronizeCollections(this, node()->connectors()->nodes(), connectors_,
		[](Model::Node* node, Item* item){return item->node() == node;},
		[](Item* parent, Model::Node* node)
			{return new VCommentDiagramConnector{parent, static_cast<CommentDiagramConnector*>(node)};},
		[](Item*, Model::Node*, VCommentDiagramConnector*&){return false;});

	// Always update children
	for (auto s : shapes_) s->setUpdateNeeded(StandardUpdate);
	for (auto c : connectors_) c->setUpdateNeeded(StandardUpdate);
}

void VCommentDiagram::updateGeometry(int, int)
{
	QSize minSize;

	if (shapes_.isEmpty()) minSize = QSize{200, 50}; // If there are no shapes yet, make space for a text message
	else
	{
		// Compute minimum required size
		minSize = QSize{0, 0};
		for (auto vShape : shapes_)
		{
			auto nShape = vShape->node();

			int shapeWidth = nShape->x() + nShape->width();
			int shapeHeight = nShape->y() + nShape->height();
			minSize = minSize.expandedTo(QSize{shapeWidth, shapeHeight});
		}
	}

	// override with user set size if provided
	QSize expanded = minSize.expandedTo(QSize{node()->width(), node()->height()});
	if (editing_) expanded += QSize{2*EDIT_OUTLINE_SIZE, 2*EDIT_OUTLINE_SIZE};

	if (hasShape())
	{
		if (editing_) getShape()->setOuterSize(expanded.width(), expanded.height());
		else getShape()->setInnerSize(expanded.width(), expanded.height());
	}
	else setSize(expanded);

	QPoint offset{0, 0};
	if ( editing_) offset+= QPoint{EDIT_OUTLINE_SIZE, EDIT_OUTLINE_SIZE};
	if ( hasShape() )
	{
		if (getShape()->contentLeft() > offset.x()) offset.setX( getShape()->contentLeft());
		if (getShape()->contentTop() > offset.y()) offset.setY( getShape()->contentTop());
	}


	// Position shapes
	for (auto vShape : shapes_)
	{
		auto nShape = vShape->node();
		vShape->setPos(nShape->pos() + offset);
	}

	// Position connectors
	for (auto vConnector : connectors_)
	{
		auto nConnector = vConnector->node();

		auto startShape = shapes_.at(nConnector->startShape());
		auto endShape = shapes_.at(nConnector->endShape());
		auto startPoint = startShape->pos() + startShape->node()->connectorPoint(nConnector->startPoint());
		auto endPoint = endShape->pos() + endShape->node()->connectorPoint(nConnector->endPoint());

		QPoint pos((std::min(startPoint.x(), endPoint.x()))-VCommentDiagramConnector::MAX_ARROW_WIDTH,
					  (std::min(startPoint.y(), endPoint.y()))-VCommentDiagramConnector::MAX_ARROW_WIDTH);
		vConnector->setPos(pos);
	}
}

void VCommentDiagram::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Item::paint(painter, option, widget);

	// assume a default white background
	if (!hasShape()) painter->setBrush(Qt::white);
	else painter->setBrush(Qt::NoBrush);

	if (!hasShape())
		painter->drawRect(QRect{QPoint{0, 0}, sizeInLocal().toSize()});

	if (shapes_.isEmpty())
	{
		painter->setPen(QPen{QColor{100, 100, 100}});
		painter->drawText(QRect{QPoint{0, 0}, sizeInLocal().toSize()}, Qt::AlignCenter | Qt::TextWordWrap,
								"This diagram does not contain any shapes yet.");
	}
}

void VCommentDiagram::toggleEditing()
{
	if (!toolbar()->isVisible())
	{
		QGraphicsView *v = scene()->views().first();
		QPointF sceneP = this->mapToScene(this->boundingRect().bottomLeft());
		toolbar_->move(v->viewport()->mapToGlobal(v->mapFromScene(sceneP)));
	}
	toolbar_->show();

	setUpdateNeeded(StandardUpdate);
}

void VCommentDiagram::setShowConnectorPoints(bool show)
{
	showConnectorPoints_ = show;
	setUpdateNeeded(VCommentDiagram::StandardUpdate);
}

CommentDiagramToolbar* VCommentDiagram::toolbar()
{
	return toolbar_;
}

void VCommentDiagram::selectLastShape()
{
	shapes_.last()->moveCursor();
	shapes_.last()->setSelected(true);
	this->toolbar_->setCurrentShape(shapes_.last());
}

CommentDiagramToolbar* VCommentDiagram::toolbar_ = new CommentDiagramToolbar{};

}
