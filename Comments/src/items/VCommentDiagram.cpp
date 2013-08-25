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
#include "VisualizationBase/src/items/Line.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"

using namespace Visualization;

namespace Comments {

ITEM_COMMON_DEFINITIONS(VCommentDiagram, "item")

VCommentDiagram::VCommentDiagram(Item* parent, NodeType* node) : Super(parent, node, itemStyles().get())
{
}

VCommentDiagram::~VCommentDiagram()
{
	clearChildren();
}

void VCommentDiagram::determineChildren()
{
	QList<Model::Node*> nodes;
	nodes << node()->shapes()->nodes().toList();
	nodes << node()->connectors()->nodes().toList();
	synchronizeWithNodes(nodes, renderer());
}

void VCommentDiagram::clearChildren()
{
	for(int i = 0; i < items_.size(); ++i)
		delete items_.at(i);
}

void VCommentDiagram::updateGeometry(int, int)
{
	QSize maxsize(0,0);
	for(int i = 0; i < items_.size(); ++i)
	{
		auto child = items_.at(i);
		// only count shapes, connectors will stay within these bounds
		auto shape = dynamic_cast<CommentDiagramShape*>(child->node());
		if(shape != nullptr)
		{
			child->setPos(shape->x(), shape->y());

			QSize itemsize(shape->pos().x() + shape->size().width(), shape->pos().y() + shape->size().height());
			maxsize.setHeight(std::max(maxsize.height(), itemsize.height()));
			maxsize.setWidth(std::max(maxsize.width(), itemsize.width()));
		}
	}
	setSize(maxsize);
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

} /* namespace Comments */
