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

void ViewItem::insertNode(Model::Node* node, int column, int row)
{
	if (nodes_.size() <= column)
		nodes_.insert(column, {});
	nodes_[column].insert(row, node);
	scene()->scheduleUpdate();
}

void ViewItem::removeNode(Model::Node* node)
{
	for (auto column : nodes_)
		for (int i = 0; i < column.size(); i++)
			if (column.at(i) && column.at(i) == node)
				column.removeAt(i);
	scene()->scheduleUpdate();
}

const QList<Model::Node*> ViewItem::allNodes() const
{
	QList<Model::Node*> result;
	for (auto column : nodes_)
		for (auto item : column)
			result.append(item);
	return result;
}

QVector<QVector<Model::Node*>> ViewItem::nodesGetter()
{
	return nodes_;
}
}
