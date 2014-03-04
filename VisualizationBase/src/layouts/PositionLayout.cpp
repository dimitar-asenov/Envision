/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include <views/MainView.h>

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
			averageWidth += i->widthInParent();
			averageHeight += i->heightInParent();
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

			if (lastBottom == 0 || (lastBottom + items[i]->heightInParent() <= heightLimit))
			{
				lastBottom += 10 + items[i]->heightInParent();
				lastBottom = toGrid(lastBottom);

				if (items[i]->widthInParent() > colWidth) colWidth =  items[i]->widthInParent();
			}
			else
			{
				y = 0;
				lastBottom = 10 + items[i]->heightInParent();

				lastRight += 10 + colWidth;
				lastRight = toGrid(lastRight);
				x = lastRight;
				colWidth = items[i]->widthInParent();
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

		for (auto m : models) m->endModification(false);
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
		if (i==0 || bottomRight.x() < x + items[i]->widthInParent()  )
			bottomRight.setX( x + items[i]->widthInParent() );
		if (i==0 || bottomRight.y() < y + items[i]->heightInParent() )
			bottomRight.setY( y + items[i]->heightInParent() );
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

	// ---------------------------------------

	qreal geometricZoomScale = mainViewScalingFactor();

	QVector<QRect> areas(items.size()); // stores the computed areas for each item
	QVector<bool> doneExpanding(items.size()); // stores a flag whether the algorithm should still try to expand an item

	const int EXPANDING_STEP = 10; // this constant controls by how many units an item gets expanded per iteration step

	int expanded = items.size();

	// initialize the variables
	for (int i =0; i<items.size(); ++i)
	{
		doneExpanding[i] = false;

		areas[i] = QRect(items[i]->x(), items[i]->y(), items[i]->width(), items[i]->height());
	}

	while (expanded > 1) // while there is still something to do
	{
		expanded = 0; // reset the number of expanded items in this iteration step

		// try expanding all items by one step
		for (int i =0; i<items.size(); ++i)
		{
			if (doneExpanding[i]) continue; // item does not have to be arranged further

			// using an alias is supposed to make the code more readable
			auto item = items[i];
			auto area = &areas[i];


			// possibly adjust the x-coordinate of the current item
			auto newX = area->x() - EXPANDING_STEP / 2;
			auto movedBeforeCollidingX = false;

			if (newX >= style()->leftInnerMargin())
			{
				area->setX(newX);

				movedBeforeCollidingX = true;
			}

			// possibly adjust the y-coordinate of the current item
			auto newY = area->y() - EXPANDING_STEP / 2;
			auto movedBeforeCollidingY = false;

			if (newY >= style()->topInnerMargin())
			{
				area->setY(newY);

				movedBeforeCollidingY = true;
			}


			// try expanding the size
			area->setWidth(area->width() + EXPANDING_STEP);
			area->setHeight(area->height() + EXPANDING_STEP);

			// scale the item according to the computed area
			if (scaleItem(item, area, geometricZoomScale)) doneExpanding[i] = true;

			// adjust the actual area used by the item after scaling to make sure the area is only representing the
			// actual area used by the item without the additional parts it does not use
			area->setWidth(item->width() * item->scale());
			area->setHeight(item->height() * item->scale());

			// test whether the current area collides with any other areas
			if (areaCollides(area, areas))
			{
				// if there was a collision this item cannot be expanded further
				doneExpanding[i] = true;

				// reset the x-coordinate if it was moved
				if (movedBeforeCollidingX)
				{
					area->setX(area->x() + EXPANDING_STEP / 2);
				}

				// reset the y-coordinate if it was moved
				if (movedBeforeCollidingY)
				{
					area->setY(area->y() + EXPANDING_STEP / 2);
				}

				// reset the size of the area
				area->setWidth(area->width() - EXPANDING_STEP);
				area->setHeight(area->height() - EXPANDING_STEP);

				// scale the item according to the computed area
				scaleItem(item, area, geometricZoomScale);

				// adjust the actual area used by the item after scaling to make sure the area is only representing the
				// actual area used by the item without the additional parts it does not use
				area->setWidth(item->width() * item->scale());
				area->setHeight(item->height() * item->scale());
			}
			else expanded++; // there was no collision so the number of expanded items in this iteration step is increased
		}
	}

	// set the calculated new item positions
	for(int i = 0; i<items.size(); ++i)
	{
		items[i]->setPos(areas[i].x(), areas[i].y());
	}

	// calculate the new size of the container
	for(int i = 0; i<items.size(); ++i)
	{
		int x = positions[i]->xNode() ? (areas[i].x()) : 0;
		int y = positions[i]->yNode() ? (areas[i].y()) : 0;

		if (i==0 || topLeft.x() > x )
			topLeft.setX( x );
		if (i==0 || topLeft.y() > y )
			topLeft.setY( y );
		if (i==0 || bottomRight.x() < x + areas[i].width()   )
			bottomRight.setX( x + areas[i].width());
		if (i==0 || bottomRight.y() < y + areas[i].height() )
			bottomRight.setY( y + areas[i].height());
	}

	sizeWidth = bottomRight.x() - topLeft.x() + style()->leftInnerMargin() + style()->rightInnerMargin();
	sizeHeight = bottomRight.y() - topLeft.y() + style()->topInnerMargin() + style()->bottomInnerMargin();
	setInnerSize(sizeWidth, sizeHeight);
}

bool PositionLayout::scaleItem(Item* item, QRect* area, qreal geometricZoomScale)
{
	bool fullScale = false; // stores whether the area can get as big as it is allowed to with the provided area

	// calculate the parent item's scale
	qreal pScale = item->totalScale() / item->scale();

	// compute the ratios between the available space and the minimal used space
	qreal scaleX = (qreal)area->width() / item->width();
	qreal scaleY = (qreal)area->height() / item->height();

	// take the smaller ratio to guarantee that the item is not using more space than what area permits
	qreal scale = scaleX < scaleY ? scaleX : scaleY;

	// calculate the total perceived scale for the chosen new scale
	qreal totalScale = geometricZoomScale * scale * pScale;

	// calculate the maximum scale (used to hardcap the scale of an item)
	qreal maxScale = geometricZoomScale < 1 ? 1 : geometricZoomScale;

	if (totalScale >= maxScale)
	{
		// if the item's scale would be larger than the maximum scale set it to the maximum value it could be
		scale = maxScale / geometricZoomScale / pScale;

		//  the item is as big as it can get
		fullScale = true;
	}

	// set the items scale
	item->setScale(scale);

	return fullScale;
}

bool PositionLayout::areaCollides(QRect* area, QVector<QRect>& areas)
{
	for (int i =0; i<areas.size(); ++i)
	{
		if (area == &areas[i]) continue;

		if (area->intersects(areas[i]))
		{
			return true;
		}
	}

	return false;
}

int PositionLayout::focusedElementIndex() const
{
	for (int i = 0; i<items.size(); ++i)
		if ( items[i]->itemOrChildHasFocus()) return i;

	return -1;
}

}
