/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "layouts/PositionLayout.h"
#include "shapes/Shape.h"
#include "../renderer/ModelRenderer.h"

#include "ModelBase/src/model/Model.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS( PositionLayout, "layout" )

PositionLayout::PositionLayout(Item* parent, const StyleType* style) :
Super(parent, style)
{
}

PositionLayout::~PositionLayout()
{
	for (int i = 0; i<items.size(); i++) SAFE_DELETE_ITEM( items[i]);
}

int PositionLayout::length() const
{
	return items.size();
}

bool PositionLayout::containsNode(Model::Node* node_)
{
	for(int i=0; i<items.size(); ++i)
		if (items[i]->node() == node_) return true;

	return false;
}

void PositionLayout::insert(Item* item)
{
	// Check whether this item has a node and a position associated with it
	if ( !item->hasNode() ) throw VisualizationException("Adding an Item that has no node to a PositionLayout");

	Model::CompositeNode* extNode = dynamic_cast<Model::CompositeNode*> (item->node());
	if (!extNode)
		throw VisualizationException("Adding an Item that does not implement CompositeNode to a PositionLayout");

	Position* pos = extNode->extension<Position>();

	if (!pos)
		throw VisualizationException("Adding a Item whose node does not have a Position extension to a PositionLayout");

	item->setParentItem(this);
	items.append(item);
	positions.append(pos);
	setUpdateNeeded(StandardUpdate);
}

void PositionLayout::remove(int index, bool deleteItem)
{
	if (deleteItem) SAFE_DELETE_ITEM( items[index]);
	else if(items[index]) items[index]->setParentItem(nullptr);

	SAFE_DELETE( positions[index] );
	items.remove(index);
	positions.remove(index);
	setUpdateNeeded(StandardUpdate);
}

void PositionLayout::removeAll(Item* item, bool deleteItem)
{
	for (int i = items.size() - 1; i>=0; --i)
	{
		if (items.at(i) == item)
		{
			items.remove(i);
			SAFE_DELETE( positions[i] );
			positions.remove(i);
		}
	}

	if (deleteItem) SAFE_DELETE_ITEM(item);
	else if (item) item->setParentItem(nullptr);

	setUpdateNeeded(StandardUpdate);
}

void PositionLayout::clear(bool deleteItems)
{
	for (int i = 0; i<items.size(); ++i)
	{
		if (deleteItems) SAFE_DELETE_ITEM(items[i]);
		else if (items[i]) items[i]->setParentItem(nullptr);
	}

	for (int i = 0; i<positions.size(); ++i) SAFE_DELETE(positions[i]);
	items.clear();
	positions.clear();
}

void PositionLayout::swap(int i, int j)
{
	Item* t = items[i];
	items[i] = items[j];
	items[j] = t;

	Position* p = positions[i];
	positions[i] = positions[j];
	positions[j] = p;
}

void PositionLayout::synchronizeWithNodes(const QList<Model::Node*>& nodes, ModelRenderer* renderer)
{
	allNodesLackPositionInfo = items.isEmpty();

	// Inserts elements that are not yet visualized and adjusts the order to match that in 'nodes'.
	for (int i = 0; i < nodes.size(); ++i)
	{
		// Check if node has position info
		Model::CompositeNode* extNode = dynamic_cast<Model::CompositeNode*> (nodes[i]);
		if (extNode)
		{
			Position* pos = extNode->extension<Position>();
			if (pos)
			{
				if (pos->xNode() || pos->yNode())
					allNodesLackPositionInfo = false;
			}
		}

		if (i >= items.size() ) insert( renderer->render(this, nodes[i]));	// This node is new
		else if ( items[i]->node() == nodes[i] ) continue;	// This node is already there
		else
		{
			// This node might appear somewhere ahead, we should look for it
			bool found = false;
			for (int k = i + 1; k<items.size(); ++k)
			{
				if ( items[k]->node() == nodes[i] )
				{
					// We found this node, swap the visualizations
					swap(i, k);
					found = true;
					break;
				}
			}

			// The node was not found, insert a visualization here
			if (!found )
			{
				insert( renderer->render(this, nodes[i]) );
				swap(i, items.size()-1);
			}
		}
	}

	// Remove excess items
	while (items.size() > nodes.size()) remove(items.size()-1);
}

int PositionLayout::toGrid(const int& pos) const
{
	int mod = (pos >=0) ? ( pos % style()->gridSize() ) : ( (-pos) % style()->gridSize() );
	int add = (pos >=0) ? ( style()->gridSize() - mod) : mod;
	return pos + ( (mod !=0) ? add : 0 );
}

bool PositionLayout::isEmpty() const
{
	for(int i = 0; i<items.size(); ++i)
		if (!items[i]->isEmpty()) return false;

	return true;
}

void PositionLayout::updateGeometry(int, int)
{
	// Arrange items if they were all missing positions.
	if (allNodesLackPositionInfo && !items.isEmpty())
	{
		// Get averages
		double averageWidth = 0;
		double averageHeight = 0;
		for(auto i : items)
		{
			averageWidth += i->width();
			averageHeight += i->height();
		}
		averageWidth /= items.size();
		averageHeight /= items.size();

		// Get 'optimal' number of rows to achieve a square
		double prevRatio = 0;
		int rows = 1;
		for (rows = 1; rows<items.size(); ++rows)
		{
			int cols = (items.size()/rows) + ((items.size()%rows)?1:0);
			double ratio = (averageWidth*cols) / (averageHeight*rows);

			if (ratio > 1) ratio = 1/ratio;

			if (ratio > prevRatio)
			{
				prevRatio = ratio;
			}
			else
			{
				if (rows > 1) --rows;
				break;
			}
		}

		int heightLimit = rows*averageHeight;

		//Compute the columns and set the positions
		int lastBottom = 0;
		int lastRight = 0;
		int colWidth = 0;


		auto model = items[0]->node()->model();
		QList<Model::Model*> models{ {model} };

		model->beginModification(items[0]->node(), "Automatically set position");
		// It is important to batch the modifications, since model::endModification() send a notification signal.

		for(int i = 0; i<items.size(); ++i)
		{
			int x = lastRight;
			int y = lastBottom;

			if (lastBottom == 0 || (lastBottom + items[i]->height() <= heightLimit))
			{
				lastBottom += 10 + items[i]->height();
				lastBottom = toGrid(lastBottom);

				if (items[i]->width() > colWidth) colWidth =  items[i]->width();
			}
			else
			{
				y = 0;
				lastBottom = 10 + items[i]->height();

				lastRight += 10 + colWidth;
				lastRight = toGrid(lastRight);
				x = lastRight;
				colWidth = items[i]->width();
			}

			auto newModel = items[i]->node()->model();

			if (newModel != model)
			{
				if (!models.contains(newModel))
				{
					models << newModel;
					newModel->beginModification(items[i]->node(), "Automatically set position");
				}

				model = newModel;
			}

			model->changeModificationTarget(items[i]->node());
			positions[i]->setX(x);
			positions[i]->setY(y);
		}

		for (auto m : models) m->endModification();
		allNodesLackPositionInfo = false;
	}

	QPoint topLeft;
	QPoint bottomRight;

	for(int i = 0; i<items.size(); ++i)
	{
		int x = positions[i]->xNode() ? toGrid(positions[i]->x()) : 0;
		int y = positions[i]->yNode() ? toGrid(positions[i]->y()) : 0;

		if (i==0 || topLeft.x() > x )
			topLeft.setX( x );
		if (i==0 || topLeft.y() > y )
			topLeft.setY( y );
		if (i==0 || bottomRight.x() < x + items[i]->width()  )
			bottomRight.setX( x + items[i]->width() );
		if (i==0 || bottomRight.y() < y + items[i]->height() )
			bottomRight.setY( y + items[i]->height() );
	}

	int sizeWidth = bottomRight.x() - topLeft.x() + style()->leftInnerMargin() + style()->rightInnerMargin();
	int sizeHeight = bottomRight.y() - topLeft.y() + style()->topInnerMargin() + style()->bottomInnerMargin();
	setInnerSize(sizeWidth, sizeHeight);

	for (int i =0; i<items.size(); ++i)
	{
		int x = positions[i]->xNode() ? toGrid(positions[i]->x()) : 0;
		int y = positions[i]->yNode() ? toGrid(positions[i]->y()) : 0;
		items[i]->setPos( xOffset() + style()->leftInnerMargin() + x - topLeft.x(),
								yOffset() + style()->topInnerMargin() + y - topLeft.y());
	}
}

int PositionLayout::focusedElementIndex() const
{
	for (int i = 0; i<items.size(); ++i)
		if ( items[i]->itemOrChildHasFocus()) return i;

	return -1;
}

}
