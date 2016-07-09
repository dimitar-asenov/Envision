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

#include "PositionLayout.h"
#include "../shapes/Shape.h"
#include "../renderer/ModelRenderer.h"
#include "../node_extensions/FullDetailSize.h"
#include "../views/MainView.h"

#include "ModelBase/src/model/TreeManager.h"

namespace Visualization {

DEFINE_ITEM_COMMON( PositionLayout, "layout" )

constexpr bool ENABLE_AUTOMATIC_SEMANTIC_ZOOM = false;

// TODO: Do this properly.
// Defines the semantic zoom level to be used when abstracting an item.
// for performance reasons this is a constant but in the interest of general consistency its value should be
// scene()->defaultRenderer()->semanticZoomLevelId("project_module_class_method_abstraction");
constexpr int FULL_DECLARATION_ABSTRACTION_SEMANTIC_ZOOM_LEVEL = 5;

PositionLayout::PositionLayout(Item* parent, const StyleType* style) :
Super{parent, style}
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

bool PositionLayout::containsNode(Model::Node* node)
{
	for (int i=0; i<items.size(); ++i)
		if (items[i]->node() == node) return true;

	return false;
}

void PositionLayout::insert(Item* item)
{
	// Check whether this item has a node and a position associated with it
	ensureItemHasCompositeNode(item);

	item->setParentItem(this);
	items.append(item);
	setUpdateNeeded(StandardUpdate);
}

void PositionLayout::remove(int index, bool deleteItem)
{
	if (deleteItem) SAFE_DELETE_ITEM( items[index]);
	else if (items[index]) items[index]->setParentItem(nullptr);

	items.remove(index);
	setUpdateNeeded(StandardUpdate);
}

void PositionLayout::removeAll(Item* item, bool deleteItem)
{
	for (int i = items.size() - 1; i>=0; --i)
	{
		if (items.at(i) == item)
			items.remove(i);
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

	items.clear();
}

void PositionLayout::swap(int i, int j)
{
	Item* t = items[i];
	items[i] = items[j];
	items[j] = t;
}

void PositionLayout::synchronizeWithNodes(const QList<Model::Node*>& nodes)
{
	// Check if any node is lacking a position info
	allNodesLackPositionInfo = items.isEmpty();
	if (allNodesLackPositionInfo)
		for (auto node : nodes)
			if (auto extNode = DCast<Model::CompositeNode> (node))
				if (auto pos = extNode->extension<Position>())
					if (pos->xNode() || pos->yNode())
					{
						allNodesLackPositionInfo = false;
						break;
					}

	synchronizeCollections(this, nodes, items,
		[](Model::Node* node, Item* item){return item->node() == node;},
		[](Item* parent, Model::Node* node) -> Item*
			{
				if (ENABLE_AUTOMATIC_SEMANTIC_ZOOM)
					parent->setChildNodeSemanticZoomLevel(node, FULL_DECLARATION_ABSTRACTION_SEMANTIC_ZOOM_LEVEL);
				auto newItem = parent->renderer()->render(parent, node);
				ensureItemHasCompositeNode(newItem);
				return newItem;
			},
		[](Item* parent, Model::Node* node, Item*& item) ->bool
			{
				bool changed = parent->renderer()->sync(item, parent, node);
				ensureItemHasCompositeNode(item);
				return changed;
			},
		[](Item* item) { SAFE_DELETE_ITEM(item);});
}

int PositionLayout::toGrid(const int& pos) const
{
	int mod = (pos >=0) ? ( pos % style()->gridSize() ) : ( (-pos) % style()->gridSize() );
	int add = (pos >=0) ? ( style()->gridSize() - mod) : mod;
	return pos + ( (mod !=0) ? add : 0 );
}

bool PositionLayout::isEmpty() const
{
	for (int i = 0; i<items.size(); ++i)
		if (!items[i]->isEmpty()) return false;

	return true;
}

void PositionLayout::updateGeometry(int, int)
{
	QList<Model::TreeManager*> modifiedManagers;
	// This is a list of all managers that are modified as a result of this call. Managers are modified when the
	// elements of a PositionLayout are arranged for the first time or when the size of an element is updated.

	Model::TreeManager* lastModifiedManager = nullptr;

	// Arrange items if they were all missing positions.
	if (allNodesLackPositionInfo && !items.isEmpty())
	{
		// Get averages
		double averageWidth = 0;
		double averageHeight = 0;
		for (auto i : items)
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


		lastModifiedManager = items[0]->node()->manager();
		modifiedManagers << lastModifiedManager;

		lastModifiedManager->beginModification(items[0]->node(), "Automatically set position");

		for (int i = 0; i<items.size(); ++i)
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

			auto newManager = items[i]->node()->manager();

			if (newManager != lastModifiedManager)
			{
				if (!modifiedManagers.contains(newManager))
				{
					modifiedManagers << newManager;
					newManager->beginModification(items[i]->node(), "Automatically set position");
				}

				lastModifiedManager = newManager;
			}

			lastModifiedManager->changeModificationTarget(items[i]->node());
			positionOf(items[i])->set(x, y);
		}


		allNodesLackPositionInfo = false;
	}

	// Set the size of all items
	// TODO: Enabling this prevents undo as it always registers a new action. There should be a better time to do this.
//	for (auto item : items)
//	{
//		Model::Node* node = item->node();
//		auto fds = (static_cast<Model::CompositeNode*>(node))->extension<FullDetailSize>();

//		if (!fds->hasSize() || fds->size() != item->sizeInParent().toSize())
//		{
//			auto newManager = node->manager();

//			if (newManager != lastModifiedManager)
//			{
//				if (!modifiedManagers.contains(newManager))
//				{
//					modifiedManagers << newManager;
//					newManager->beginModification(node, "Set size");
//					qDebug() << "SETTING SIZE";
//				}

//				lastModifiedManager = newManager;
//			}

//			lastModifiedManager->changeModificationTarget(node);
//			fds->set(item->sizeInParent().toSize());
//		}

//		delete fds;
//	}

	// It is important to batch the modifications, since model::endModification() send a notification signal.
	for (auto m : modifiedManagers) m->endModification(false);

	if (ENABLE_AUTOMATIC_SEMANTIC_ZOOM && adjustChildrenSemanticZoom())
	{
		setUpdateNeeded(RepeatUpdate);
		return;
	}

	QPoint topLeft;
	QPoint bottomRight;

	for (int i = 0; i<items.size(); ++i)
	{
		auto position = positionOf(items[i]);
		int x = position->xNode() ? toGrid(position->x()) : 0;
		int y = position->yNode() ? toGrid(position->y()) : 0;

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
		auto position = positionOf(items[i]);
		int x = position->xNode() ? toGrid(position->x()) : 0;
		int y = position->yNode() ? toGrid(position->y()) : 0;
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

bool PositionLayout::isSensitiveToScale() const
{
	return ENABLE_AUTOMATIC_SEMANTIC_ZOOM;
}

void PositionLayout::determineChildren()
{
	for (auto & item : items)
		renderer()->sync(item, this, item->node());

	Super::determineChildren();
}

bool PositionLayout::adjustChildrenSemanticZoom()
{
	// An item gets abstracted if its perceived width falls below this value
	constexpr qreal ABSTRACTION_THRESHOLD = 200;

	bool changesMade = false;

	qreal geometricZoomScale = mainViewScalingFactor();
	if (geometricZoomScale != previousMainViewScalingFactor())
	{
		// Computes wherehter this Layout is entirely outside of view
		bool inViewport = false;
		for (auto views : scene()->views())
		{
			if (auto mv = dynamic_cast<MainView*>(views))
			{
				inViewport = mv->visibleRect().intersects(mapRectToScene(0, 0, widthInLocal(), heightInLocal()));
				break;
			}
		}

		for (auto item : items)
		{
			if (geometricZoomScale < 1)
			{
				if (!inViewport || (item->totalScale() * geometricZoomScale * item->widthInLocal() < ABSTRACTION_THRESHOLD))
				{
					if (item->semanticZoomLevel() != FULL_DECLARATION_ABSTRACTION_SEMANTIC_ZOOM_LEVEL)
					{
						setChildNodeSemanticZoomLevel(item->node(), FULL_DECLARATION_ABSTRACTION_SEMANTIC_ZOOM_LEVEL);
						changesMade = true;
					}
				}
				else
				{
					if (inViewport && item->semanticZoomLevel() == FULL_DECLARATION_ABSTRACTION_SEMANTIC_ZOOM_LEVEL)
					{
						clearChildNodeSemanticZoomLevel(item->node());
						changesMade = true;
					}
				}
			}
			else
			{
				// if the geometric zoom scale is larger or equal to 1 everything should be shown in full detail
				if (definesChildNodeSemanticZoomLevel(item->node()))
				{
					clearChildNodeSemanticZoomLevel(item->node());
					changesMade = true;
				}
			}
		}
	}

	return changesMade;
}

}
