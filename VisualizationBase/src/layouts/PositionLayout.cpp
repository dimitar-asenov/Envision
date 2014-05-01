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

void PositionLayout::determineChildren()
{
	if (needsUpdate() != FullUpdate) return;

	QList<Model::Node*> nodes;
	for (int i = 0; i<items.size(); i++)
	{
		nodes.append(items[i]->node());
		SAFE_DELETE_ITEM(items[i]);
	}

	synchronizeWithNodes(nodes, renderer());

	Super::determineChildren();
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
		calculateNodesPositionInfo();
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
		if (i==0 || bottomRight.x() < x + items[i]->widthInLocal()  )
			bottomRight.setX( x + items[i]->widthInLocal() );
		if (i==0 || bottomRight.y() < y + items[i]->heightInLocal() )
			bottomRight.setY( y + items[i]->heightInLocal() );
	}

	int sizeWidth = bottomRight.x() - topLeft.x() + style()->leftInnerMargin() + style()->rightInnerMargin();
	int sizeHeight = bottomRight.y() - topLeft.y() + style()->topInnerMargin() + style()->bottomInnerMargin();
	setInnerSize(sizeWidth, sizeHeight);


	scaleAndPositionItems(topLeft, sizeWidth, sizeHeight);
}

void PositionLayout::scaleAndPositionItems(QPoint topLeft, int sizeWidth, int sizeHeight)
{
	qreal geometricZoomScale = mainViewScalingFactor();

	QVector<QRectF> areas(items.size()); // stores the computed area for each item

	QVector<qreal> scales(items.size()); // stores the computed scale for each item

	// stores a priority list of directions in which an item should grow next
	QVector<QList<int>> expandingDirections(items.size());

	QVector<qreal> widthHeightRatio(items.size()); // stores the ratio between width and height of each item

	// the possible directions in expandingDirections
	const int DIR_LU = 1;
	const int DIR_RU = 2;
	const int DIR_RB = 3;
	const int DIR_LB = 4;

	const int EXPANDING_STEP = 10; // this constant controls by how many units an item gets expanded per iteration step

	int expanding = items.size();

	// initialize the variables
	for (int i = 0; i<items.size(); ++i)
	{
		scales[i] = 1;

		expandingDirections[i].append(DIR_LU);
		expandingDirections[i].append(DIR_RU);
		expandingDirections[i].append(DIR_RB);
		expandingDirections[i].append(DIR_LB);

		int x = positions[i]->xNode() ? toGrid(positions[i]->x()) : 0;
		int y = positions[i]->yNode() ? toGrid(positions[i]->y()) : 0;
		areas[i] = QRect(xOffset() + style()->leftInnerMargin() + x - topLeft.x(),
							  yOffset() + style()->topInnerMargin() + y - topLeft.y(),
							  items[i]->widthInLocal(),
							  items[i]->heightInLocal());

		widthHeightRatio[i] = (qreal)items[i]->widthInLocal() / (qreal)items[i]->heightInLocal();
	}

	while (expanding > 0) // while there is still something to do
	{
		expanding = 0; // reset the number of expanded items in this iteration step

		// try expanding all items by one step
		for (int i =0; i<items.size(); ++i)
			while (expandingDirections[i].size() > 0) { // item can possibly be expanded further
				auto item = items[i];
				auto area = &areas[i];

				// get the next priority expanding direction
				auto expandingDirection = expandingDirections[i].takeFirst();
				int compositeDirection = DIR_LU; // this is just a dummy assignment (variable set in subsequent switch)

				// these variables store the amount the item is going to be expanded in each direction in this step
				qreal expansionLeft, expansionUp, expansionRight, expansionDown;
				expansionLeft = expansionUp = expansionRight = expansionDown = 0;

				switch (expandingDirection)
				{
				case DIR_LU:
					expansionLeft = expansionUp = EXPANDING_STEP;
					compositeDirection = DIR_RB;
					break;
				case DIR_RU:
					expansionRight = expansionUp = EXPANDING_STEP;
					compositeDirection = DIR_LB;
					break;
				case DIR_RB:
					expansionRight = expansionDown = EXPANDING_STEP;
					compositeDirection = DIR_LU;
					break;
				case DIR_LB:
					expansionLeft = expansionDown = EXPANDING_STEP;
					compositeDirection = DIR_RU;
					break;
				default:
					Q_ASSERT(false); // this should be unreachable
				}

				// constrain the expansion such that the resulting area would have the same side ratio
				if (widthHeightRatio[i] > 1)
				{
					expansionUp /= widthHeightRatio[i];
					expansionDown /= widthHeightRatio[i];
				}
				else
				{
					expansionRight *= widthHeightRatio[i];
					expansionLeft *= widthHeightRatio[i];
				}

				// try expanding the area
				area->setLeft(area->left() - expansionLeft);
				area->setTop(area->top() - expansionUp);
				area->setRight(area->right() + expansionRight);
				area->setBottom(area->bottom() + expansionDown);

				// calculate possible collisions with borders
				bool doesNotCollideWithLeftBorder = (area->left() > style()->leftInnerMargin()) || expansionLeft == 0;
				bool doesNotCollideWithTopBorder = (area->top() > style()->topInnerMargin()) || expansionUp == 0;
				bool doesNotCollideWithRightBorder = (area->right() < sizeWidth - style()->rightInnerMargin() -
																  style()->leftInnerMargin()) || expansionRight == 0;
				bool doesNotCollideWithBottomBorder = (area->bottom() < sizeHeight - style()->bottomInnerMargin() -
																	style()->topInnerMargin()) || expansionDown == 0;

				if (!areaCollides(area, areas) && doesNotCollideWithLeftBorder && doesNotCollideWithRightBorder &&
					 doesNotCollideWithTopBorder && doesNotCollideWithBottomBorder)
				{
					// the expanded item does not collide with any item or border

					bool fullScale = scaleItem(item, area, scales[i], geometricZoomScale);

					if (fullScale)
						expandingDirections[i].clear(); // if the item is fully scaled it does not need to be expanded further
					else
					{
						expanding++; // this item is continuously interested in getting larger

						// to assure a more centralized growing around the original place we try to expand in the opposing
						// direction next if this direction is still available to be explored
						auto compositeElementIndex = expandingDirections[i].indexOf(compositeDirection);
						if (compositeElementIndex > 0)
						{
							auto compositeElement = expandingDirections[i].takeAt(compositeElementIndex);
							expandingDirections[i].prepend(compositeElement);
						}

						// append the last growing direction to the end of the priority list such that the item will use
						// this direction again only after trying to grow in a different available direction first
						expandingDirections[i].append(expandingDirection);
					}

					break; // after either successful growing skip this item's grow step and go to the next item
				}
				else
				{
					// the area collided in the attempted direction

					// revert all changes to the area
					area->setLeft(area->left() + expansionLeft);
					area->setTop(area->top() + expansionUp);
					area->setRight(area->right() - expansionRight);
					area->setBottom(area->bottom() - expansionDown);
				}
			}
	}

	// set the calculated new item positions
	for(int i = 0; i<items.size(); ++i)
	{
		items[i]->setPos(areas[i].x(), areas[i].y());

		items[i]->setItemScale(scales[i]);
	}
}

bool PositionLayout::scaleItem(Item* item, QRectF* area, qreal& scale, qreal geometricZoomScale)
{
	bool fullScale = false; // stores whether the area can get as big as it is allowed to with the provided area

	// calculate the parent item's scale
	qreal pScale = item->totalScale() / item->scale();

	// compute the ratios between the available space and the minimal used space
	qreal scaleX = (qreal)area->width() / item->widthInLocal();
	qreal scaleY = (qreal)area->height() / item->heightInLocal();

	// take the smaller ratio to guarantee that the item is not using more space than what area permits
	scale = scaleX < scaleY ? scaleX : scaleY;

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

	return fullScale;
}

bool PositionLayout::areaCollides(QRectF* area, QVector<QRectF>& areas)
{
	for (int i = 0; i<areas.size(); ++i)
	{
		if (area == &areas[i]) continue;

		if (area->intersects(areas[i]))
		{
			return true;
		}
	}

	return false;
}

void PositionLayout::calculateNodesPositionInfo()
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

int PositionLayout::focusedElementIndex() const
{
	for (int i = 0; i<items.size(); ++i)
		if ( items[i]->itemOrChildHasFocus()) return i;

	return -1;
}

bool PositionLayout::itemGeometryChangesWithZoom() const
{
	return true;
}

}
