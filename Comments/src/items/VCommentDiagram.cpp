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
#include "VCommentDiagramConnector.h"

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
	synchronizeWithNodes(node()->shapes()->nodes(), shapes_);
	synchronizeWithNodes(node()->connectors()->nodes(), connectors_);

	int static i = 0;
	if (!editing_) qDebug() << "Updating all" << ++i << (void*) this;
	// Always update children
	for (auto s : shapes_) s->setUpdateNeeded(StandardUpdate);
	for (auto c : connectors_) c->setUpdateNeeded(StandardUpdate);
}

void VCommentDiagram::updateGeometry(int, int)
{
	QSize minSize;

	if(shapes_.isEmpty()) minSize = QSize(200, 50); // If there are no shapes yet, make space for a text message
	else
	{
		// Compute minimum required size
		minSize = QSize{0,0};
		for (auto vShape : shapes_)
		{
			auto nShape = vShape->node();

			int shapeWidth = nShape->x() + nShape->width();
			int shapeHeight = nShape->y() + nShape->height();
			minSize = minSize.expandedTo(QSize(shapeWidth, shapeHeight));
		}
	}

	// override with user set size if provided
	QSize expanded = minSize.expandedTo(QSize(node()->width(), node()->height()));
	if(editing_) expanded += QSize(2*EDIT_OUTLINE_SIZE, 2*EDIT_OUTLINE_SIZE);

	if(hasShape())
	{
		if (editing_) getShape()->setOutterSize(expanded.width(), expanded.height());
		else getShape()->setInnerSize(expanded.width(), expanded.height());
	}
	else setSize(expanded);

	QPoint offset{0,0};
	if( editing_) offset+= QPoint(EDIT_OUTLINE_SIZE, EDIT_OUTLINE_SIZE);
	if( hasShape() )
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

		QPoint pos(std::min(startPoint.x(), endPoint.x()), std::min(startPoint.y(), endPoint.y()));
		vConnector->setPos(pos);
	}
}

void VCommentDiagram::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Item::paint(painter, option, widget);

	// assume a default white background
	if(!hasShape()) painter->setBrush(Qt::white);
	else painter->setBrush(Qt::NoBrush);

	if(editing_)
	{
		painter->setPen(QPen(Qt::red, EDIT_OUTLINE_SIZE));
		painter->drawRect( QRect{QPoint(EDIT_OUTLINE_SIZE/2, EDIT_OUTLINE_SIZE/2),
										 size().toSize() - QSize(EDIT_OUTLINE_SIZE, EDIT_OUTLINE_SIZE)} );
	}
	else if(!hasShape())
		painter->drawRect(QRect{QPoint(0,0), size().toSize()});

	if(shapes_.isEmpty())
	{
		painter->setPen(QPen(QColor(100, 100, 100)));
		painter->drawText(QRect{QPoint(0,0), size().toSize()}, Qt::AlignCenter | Qt::TextWordWrap,
								"This diagram does not contain any shapes yet.");
	}
}

template <class T>
void VCommentDiagram::synchronizeWithNodes(const QVector<Model::Node*>& nodes, QVector<T*>& destination)
{
	// Inserts elements that are not yet visualized and adjusts the order to match that in 'nodes'.
	for (int i = 0; i < nodes.size(); ++i)
	{
		if (i >= destination.size() )
			destination.append( new T(this, static_cast<typename T::NodeType*>(nodes[i])));	// This node is new
		else if ( destination[i]->node() == nodes[i] )	continue;	// This node is already there
		else
		{
			// This node might appear somewhere ahead, we should look for it
			bool found = false;
			for (int k = i + 1; k < destination.size(); ++k)
			{
				if ( destination[k]->node() == nodes[i] )
				{
					// We found this node, swap the visualizations
					T* t = destination[i];
					destination[i] = destination[k];
					destination[k] = t;

					found = true;
					break;
				}
			}

			// The node was not found, insert a visualization here
			if (!found ) destination.insert(i, new T(this, static_cast<typename T::NodeType*>(nodes[i])));
		}
	}

	// Remove excess items
	while (destination.size() > nodes.size())
	{
		SAFE_DELETE_ITEM(destination.last());
		destination.pop_back();
	}
}

void VCommentDiagram::toggleEditing()
{
	editing_ = !editing_;
	if(!editing_) showConnectorPoints_ = false;

	setUpdateNeeded(StandardUpdate);
}

void VCommentDiagram::setShowConnectorPoints(bool show)
{
	showConnectorPoints_ = show;
	setUpdateNeeded(VCommentDiagram::StandardUpdate);
}

} /* namespace Comments */
