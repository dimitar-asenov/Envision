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
	if (nodes_.size() <= column)
		nodes_.resize(column + 1);
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
	//First, make sure the current grid is big enough to fit the node
	if (nodes_.size() < column)
		nodes_.resize(column);
	if (nodes_.size() <= column)
		nodes_.insert(column, {});
	if (nodes_[column].size() <= row)
		nodes_[column].resize(row + 1);
	//We can either put the node at a free space if exists, or insert otherwise
	if (nodes_.at(column).at(row) == nullptr)
		nodes_[column][row] = node;
	else
		nodes_[column].insert(row, node);
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

QVector<QVector<Model::Node*>> ViewItem::nodesGetter()
{
	return nodes_;
}

QString ViewItem::debugNodes()
{
	//Prints the node grid as a debug string
	QString result;
	int maxHeight = 0;
	for (int i = 0; i < nodes_.size(); i++)
		if (nodes_.at(i).size() > maxHeight)
			maxHeight = nodes_.at(i).size();

	for (int row = 0; row < maxHeight; row++)
	{
		for (int col = 0; col < nodes_.size(); col++)
		{
			if (nodes_.at(col).size() > row)
				result += nodes_.at(col).at(row) ? (nodes_.at(col).at(row)->typeName() + " ") : "null ";
			else
				result += "null ";
		}
		result += "\n";
	}
	return result;
}

}
