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
#include "VisualizationBase/src/items/Line.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "VisualizationBase/src/shapes/Shape.h"

using namespace Visualization;

namespace Comments {

ITEM_COMMON_DEFINITIONS(VCommentDiagram, "item")

VCommentDiagram::VCommentDiagram(Item* parent, NodeType* node)
	: Super(parent, node, itemStyles().get())
{
}

void VCommentDiagram::determineChildren()
{
	QList<Model::Node*> nodes;
	nodes << node()->shapes()->nodes().toList();
	nodes << node()->connectors()->nodes().toList();
	synchronizeWithNodes(nodes, renderer());
}

void VCommentDiagram::updateGeometry(int, int)
{
	// use a sensible default size to display usage information
	QSize minSize;
	QPoint shapeOffset(0,0);
	if(hasShape())
		shapeOffset = QPoint(getShape()->contentLeft(), getShape()->contentTop());
	// this is not needed if there are any items, so really compute the minimal size otherwise
	if(items_.size() == 0)
		minSize = QSize(200, 50);

	for(int i = 0; i < items_.size(); ++i)
	{
		auto child = items_.at(i);

		// only count shapes, connectors will stay within these bounds
		if(auto shape = dynamic_cast<CommentDiagramShape*>(child->node()))
		{
			child->setPos(shape->pos() + shapeOffset);

			int shapeWidth = shape->x()+shape->width();
			int shapeHeight = shape->y()+shape->height();
			minSize = minSize.expandedTo(QSize(shapeWidth, shapeHeight));
		}
		// but connectors still need to be positioned
		else if(auto connector = dynamic_cast<CommentDiagramConnector*>(child->node()))
		{
			auto shape1 = dynamic_cast<CommentDiagramShape*>(node()->shapes()->nodes()[connector->shape1()]);
			auto shape2 = dynamic_cast<CommentDiagramShape*>(node()->shapes()->nodes()[connector->shape2()]);
			auto point1 = shape1->pos()+shape1->getConnectorCoordinates(connector->point1());
			auto point2 = shape2->pos()+shape2->getConnectorCoordinates(connector->point2());
			QPoint pos(std::min(point1.x(), point2.x()), std::min(point1.y(), point2.y()));
			pos += shapeOffset;
			child->setPos(pos);
		}
	}
	minSize_ = minSize;
	// override with user set size if provided
	QSize expanded = minSize_.expandedTo(QSize(node()->width(), node()->height()));

	if(hasShape())
		getShape()->setInnerSize(expanded.width(), expanded.height());
	else
		setSize(expanded);

	if(minSize.width() > node()->width() || minSize.height() > node()->height())
		resize(expanded);
}

void VCommentDiagram::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Item::paint(painter, option, widget);

	QRect rect(QPoint(0,0), size().toSize());

	// assume a default white background
	if(!hasShape())
		painter->setBrush(QBrush(Qt::white));

	if(editing_)
	{
		QPen pen = painter->pen(), oldPen = painter->pen();
		pen.setWidth(10);
		pen.setColor(Qt::red);
		painter->setPen(pen);
		painter->drawRect(rect);
		painter->setPen(oldPen);
	}
	else if(!hasShape())
		painter->drawRect(rect);

	if(items_.size() == 0)
	{
		painter->setPen(QPen(QColor(100, 100, 100)));
		painter->drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, "This diagram does not contain any shapes yet.");
	}
}

void VCommentDiagram::synchronizeWithNodes(const QList<Model::Node*>& nodes, ModelRenderer* renderer)
{
	// Inserts elements that are not yet visualized and adjusts the order to match that in 'nodes'.
	for (int i = 0; i < nodes.size(); ++i)
	{
		if (i >= items_.size() ) items_.append( renderer->render(this, nodes[i]));	// This node is new
		else if ( items_[i]->node() == nodes[i] )	continue;	// This node is already there
		else
		{
			// This node might appear somewhere ahead, we should look for it
			bool found = false;
			for (int k = i + 1; k < items_.size(); ++k)
			{
				if ( items_[k]->node() == nodes[i] )
				{
					// We found this node, swap the visualizations
					swap(i, k);
					found = true;
					break;
				}
			}

			// The node was not found, insert a visualization here
			if (!found ) items_.insert(i, renderer->render(this, nodes[i]));
		}
	}

	// Remove excess items
	while (items_.size() > nodes.size())
	{
		SAFE_DELETE_ITEM(items_.last());
		items_.pop_back();
	}
}

inline void VCommentDiagram::swap(int i, int j)
{
	Item* t = items_[i];
	items_[i] = items_[j];
	items_[j] = t;
}

void VCommentDiagram::toggleEditing()
{
	editing_ = !editing_;

	for(auto child : items_)
	{
		auto shape = dynamic_cast<VCommentDiagramShape*>(child);
		if(shape != nullptr)
			shape->setTextEditable(editing_);
	}

	setUpdateNeeded(StandardUpdate);
}

void VCommentDiagram::resize(QSize size)
{
	size = size.expandedTo(minSize_);

	node()->model()->beginModification(node(), "Resizing diagram");
	node()->setSize(size);
	node()->model()->endModification();
	setUpdateNeeded(Visualization::Item::StandardUpdate);
}

} /* namespace Comments */
