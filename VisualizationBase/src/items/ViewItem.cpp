/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
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
#include "ViewItem.h"

#include "../src/declarative/DeclarativeItemDef.h"
#include "declarative/DynamicGridFormElement.h"
#include "items/VViewItemNode.h"
#include "nodes/SpacingNode.h"
#include "nodes/ViewItemNode.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(ViewItem, "item")

ViewItem::ViewItem(Item* parent, QString name, StyleType* style) :
		Super(parent, style), name_{name}
{
}

void ViewItem::initializeForms()
{
	addForm((new DynamicGridFormElement())
			->setSpacing(10, 10)->setMargins(10)
			->setMajorAxis(GridLayouter::ColumnMajor)
			->setNodesGetter([](Item* v)
				{ auto self = static_cast<I*>(v);
				  return self->nodesGetter(); }));
}

void ViewItem::insertColumn(int column)
{
	//Make sure we actually have enough columns
	ensureColumnExists(column);
	//Only insert the column, if the current one at index is not empty
	auto isEmpty = true;
	for (auto item : nodes_.at(column))
		isEmpty = isEmpty && item == nullptr;
	//If we have elements, insert a new column (else we can use the empty column)
	if (!isEmpty)
		nodes_.insert(column, {});
}

void ViewItem::insertNode(Model::Node* node, int column, int row)
{
	auto ref = new ViewItemNode(node, nullptr);
	//First, make sure the current grid is big enough to fit the node
	ensureColumnExists(column);
	if (nodes_[column].size() <= row)
		nodes_[column].resize(row + 1);
	//Find the first empty node after the insertion, return its index
	auto empty = emptyAfter(column, row);
	//If it is an empty node -> resize the empty node (if there is a node after)
	if (empty != -1 && nodes_[column].size() > (empty + 1)
			&& nodes_[column][empty +1])
		addEmptyNode(column, empty + 1, findVisualizationOf(nodes_[column][empty + 1])->scenePos().y());
	//Remove the empty item (we replace it with a new one!)
	if (empty != -1)
		nodes_[column].remove(empty);
	//We can either put the node at a free space if exists, or insert otherwise
	if (nodes_[column][row] == nullptr)
		nodes_[column][row] = ref;
	else
		nodes_[column].insert(row, ref);
	setUpdateNeeded(StandardUpdate);
}

void ViewItem::removeNode(Model::Node* node)
{
	auto point = positionOfNode(node);
	if (point.x() != -1)
		nodes_[point.x()].remove(point.y());
	setUpdateNeeded(StandardUpdate);
}

const QList<Model::Node*> ViewItem::allNodes() const
{
	QList<Model::Node*> result;
	for (auto column : nodes_)
		for (auto item : column)
			result.append(item);
	return result;
}

const QPoint ViewItem::positionOfNode(Model::Node *node) const
{
	for (int i = 0; i < nodes_.size(); i++)
	{
		auto index = nodes_.at(i).indexOf(node);
		if (index != -1)
			return QPoint(i, index);
	}
	return QPoint(-1, -1);
}

const QPoint ViewItem::positionOfItem(Item *item) const
{
	auto vref = DCast<VViewItemNode>(item);
	if (vref)
		return positionOfNode(vref->node());
	else return QPoint(-1, -1);
}

Model::Node* ViewItem::nodeAt(int column, int row)
{
	if (column < 0 || column >= nodes_.size())
		return nullptr;
	if (row < 0 || row >= nodes_[column].size())
		return nullptr;
	return nodes_[column][row];
}

void ViewItem::addEmptyNode(int column, int row, int desiredYPosOfNextItem)
{
	emptyNodesToBuild_.append(EmptyNodeParameters(column, row, desiredYPosOfNextItem));
	setUpdateNeeded(StandardUpdate);
}

void ViewItem::updateGeometry(int availableWidth, int availableHeight)
{
	Super::updateGeometry(availableWidth, availableHeight);
	while (!emptyNodesToBuild_.isEmpty())
	{
		auto node = emptyNodesToBuild_.takeFirst();
		auto curHeight = 0;
		if (node.row > 0)
		{
			auto previousItem = findVisualizationOf(nodeAt(node.column, node.row - 1));
			curHeight = previousItem->scenePos().y() + previousItem->heightInParent();
		}
		//TODO@cyril Would be nice not to hardcode the "-20", it takes the margin of the grid into account
		auto nodeHeight = node.desiredYPosOfNextItem - 20 - curHeight;
		auto empty = new SpacingNode(30, nodeHeight);
		ensureColumnExists(node.column);
		auto ref = new ViewItemNode(empty, nullptr);
		nodes_[node.column].insert(node.row, ref);
		setUpdateNeeded(RepeatUpdate);
	}
}

int ViewItem::emptyAfter(int column, int searchFromRow)
{
	for (int r = searchFromRow; r < nodes_[column].size(); r++)
		if (nodes_[column][r] && DCast<SpacingNode>(
								DCast<ViewItemNode>(nodes_[column][r])->target()))
			return r;
	return -1;
}

void ViewItem::ensureColumnExists(int column)
{
	if (nodes_.size() <= column)
		nodes_.resize(column + 1);
}

}
