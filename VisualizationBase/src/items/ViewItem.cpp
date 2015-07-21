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
#include "nodes/ViewItemNode.h"
#include "VViewItemNode.h"
#include "overlays/ArrowOverlay.h"
#include "nodes/InfoNode.h"
#include "VInfoNode.h"
#include "Scene.h"
#include "RootItem.h"
#include "renderer/ModelRenderer.h"
#include "utils/JsonUtil.h"

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

int ViewItem::publicInterfacePurpose()
{
	int purpose = Scene::defaultRenderer()->purposeId("public_interface");
	return purpose >= 0 ? purpose
		: Scene::defaultRenderer()->registerVisualizationPurpose("public_interface");
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

Model::Node* ViewItem::insertNode(Model::Node* node, int column, int row, int purpose)
{
	auto ref = ViewItemNode::withReference(node, purpose);
	insertViewItemNode(ref, column, row);
	return ref;
}

void ViewItem::removeNode(Model::Node* node)
{
	auto point = positionOfNode(node);
	if (point.x() != -1)
	{
		notifyAboutRemoval(nodes_[point.x()][point.y()]);
		nodes_[point.x()].remove(point.y());
		setUpdateNeeded(StandardUpdate);
	}
}

QList<Model::Node*> ViewItem::allNodes() const
{
	QList<Model::Node*> result;
	for (auto column : nodes_)
		for (auto item : column)
			if (item)
				result.append(item);
	return result;
}

QPoint ViewItem::positionOfNode(Model::Node *node) const
{
	for (int i = 0; i < nodes_.size(); i++)
	{
		auto index = nodes_.at(i).indexOf(node);
		if (index != -1 && node)
			return QPoint(i, index);
	}
	return QPoint(-1, -1);
}

QPoint ViewItem::positionOfItem(Item *item) const
{
	if (item->node()) return positionOfNode(item->node());
	else return QPoint(-1, -1);
}

Model::Node* ViewItem::nodeAt(int column, int row) const
{
	if (column < 0 || column >= nodes_.size())
		return nullptr;
	if (row < 0 || row >= nodes_[column].size())
		return nullptr;
	return nodes_[column][row];
}

void ViewItem::notifyAboutRemoval(Item *item)
{
	Q_ASSERT(item);
	removeArrowsForItem(item);
}

void ViewItem::notifyAboutRemoval(Model::Node *node)
{
	Q_ASSERT(node);
	//If somebody's spacing depends on the node or a child, remove
	//the node's spacing target
	QList<ViewItemNode*> spacingNodes;
	for (auto node : allNodes())
		if (auto viewNode = DCast<ViewItemNode>(node))
			if (!viewNode->reference())
				spacingNodes.append(viewNode);

	QList<Model::Node*> toCheck{node};
	while (!toCheck.isEmpty())
	{
		auto current = toCheck.takeFirst();
		toCheck.append(current->children());
		for (auto spacing : spacingNodes)
			if (spacing->spacingParent() == current
					|| spacing->spacingTarget() == current)
			{
				spacing->setSpacingTarget(nullptr);
				spacing->setSpacingParent(nullptr);
			}
	}
	//Remove all the info nodes depending on the node
	QList<ViewItemNode*> infoNodeParents;
	for (auto node : allNodes())
	{
		auto viewNode = DCast<ViewItemNode>(node);
		if (DCast<InfoNode>(viewNode->reference()))
			infoNodeParents.append(viewNode);
	}

	toCheck.append(node);
	while (!toCheck.isEmpty())
	{
		auto current = toCheck.takeFirst();
		toCheck.append(current->children());
		for (auto parent : infoNodeParents)
		{
			auto infoNode = DCast<InfoNode>(parent->reference());
			if (infoNode->target() == current)
				removeNode(parent);
		}
	}

	//Need to remove any arrows depending on it, if exists
	if (auto item = findVisualizationOf(node))
		notifyAboutRemoval(item);
}

void ViewItem::addSpacing(int column, int row, Model::Node* spacingTarget,
						  ViewItemNode* spacingParent)
{
	insertViewItemNode(ViewItemNode::withSpacingTarget(spacingTarget, spacingParent), column, row);
}

void ViewItem::addArrow(Model::Node *from, Model::Node *to, QString layer,
						ViewItemNode *fromParent, ViewItemNode *toParent)
{
	if (!arrows_.contains(layer))
		addArrowLayer(layer);
	arrowsToAdd_.append(ArrowToAdd{fromParent, from, toParent, to, layer});
}

QList<QPair<Item*, Item*>> ViewItem::arrowsForLayer(QString layer)
{
	//First, adds all the pending arrows if any exist
	if (arrowsToAdd_.size() > 0)
	{
		auto copy = arrowsToAdd_;
		arrowsToAdd_.clear();
		for (auto line : copy)
		{
			auto fromParent = line.fromParent_ ? findVisualizationOf(line.fromParent_) : this;
			auto allFrom = fromParent->findAllVisualizationsOf(line.from_);
			auto toParent = line.toParent_ ? findVisualizationOf(line.toParent_) : this;
			auto allTo = toParent->findAllVisualizationsOf(line.to_);
			if (allFrom.size() > 0 && allTo.size() > 0)
			{
				for (auto from : allFrom)
					for (auto to : allTo)
						//TODO@cyril The node is rendered as a RootItem -> needs a hack here
						//or removing a ViewItemNode with arrows doesn't work correctly
						if (!DCast<RootItem>(from) && !DCast<RootItem>(to))
							arrows_[line.layer_].append(QPair<Item*, Item*>(from, to));
			}
			else arrowsToAdd_.append(line);
		}
	}
	//And then return the arrows in the layer
	return arrows_[layer];
}

void ViewItem::setName(const QString &name)
{
	//The arrow overlay names depend on the name -> redo them
	for (auto layer : arrows_.keys())
		scene()->removeOverlayGroup(fullLayerName(layer));
	name_ = name;
	for (auto layer : arrows_.keys())
		addArrowLayer(layer);
}

void ViewItem::determineChildren()
{
	Super::determineChildren();
	//Update all the info nodes in the view
	for (auto node : allNodes())
	{
		auto asViewItemNode = DCast<ViewItemNode>(node);
		Q_ASSERT(asViewItemNode);
		if (auto info = DCast<InfoNode>(asViewItemNode->reference()))
		{
			//If an info node is not being updated, update it
			if (auto vis = findVisualizationOf(info))
				if (vis->needsUpdate() == Item::NoUpdate)
				{
					DCast<VInfoNode>(vis)->node()->automaticUpdate();
					vis->setUpdateNeeded(StandardUpdate);
				}
		}
	}
}

void ViewItem::updateGeometry(int availableWidth, int availableHeight)
{
	Super::updateGeometry(availableWidth, availableHeight);
	bool anyChanges = false;
	for (auto node : allNodes())
	{
		auto asViewItemNode = DCast<ViewItemNode>(node);
		Q_ASSERT(asViewItemNode);
		auto item = DCast<VViewItemNode>(findVisualizationOf(asViewItemNode));
		Q_ASSERT(item);
		if (item->currentFormIndex() == 1)
			anyChanges = item->determineSpacing() || anyChanges;
	}
	if (anyChanges)
		setUpdateNeeded(RepeatUpdate);
}

void ViewItem::addArrowLayer(QString layer)
{
	auto layerName = fullLayerName(layer);
	auto arrowLayer = scene()->overlayGroup(layerName);
	if (!arrowLayer) arrowLayer = scene()->addOverlayGroup(layerName);
	arrowLayer->setOverlayConstructor2Args([](Item* from, Item* to){return makeOverlay(new ArrowOverlay(from, to));});
	arrowLayer->setDynamic2Items([this, layer](){return arrowsForLayer(layer);});
}

void ViewItem::removeArrowsForItem(Item *parent)
{
	//Remove all arrows depending on any child item of the parent
	QList<Item*> allChildren{parent};
	while (!allChildren.isEmpty())
	{
		auto current = allChildren.takeLast();
		for (auto key : arrows_.keys())
		{
			auto copy = arrows_[key];
			for (auto pair : copy)
				if (pair.first == current || pair.second == current)
					arrows_[key].removeAll(pair);
		}
		allChildren.append(current->childItems());
	}
}

void ViewItem::insertViewItemNode(ViewItemNode *node, int column, int row)
{
	//First, make sure the current grid is big enough to fit the node
	ensurePositionExists(column, row);
	//We can either put the node at a free space if exists, or insert otherwise
	if (nodes_[column][row] == nullptr)
		nodes_[column][row] = node;
	else
		nodes_[column].insert(row, node);
	setUpdateNeeded(StandardUpdate);
}

void ViewItem::ensurePositionExists(int column, int row)
{
	ensureColumnExists(column);
	if (nodes_[column].size() <= row)
		nodes_[column].resize(row + 1);
}

void ViewItem::ensureColumnExists(int column)
{
	if (nodes_.size() <= column)
		nodes_.resize(column + 1);
}

//JSON storing/loading
QJsonDocument ViewItem::toJson() const
{
	//Store all the nodes
	QJsonArray nodes;
	for (int col = 0; col < nodes_.size(); col++)
		for (int row = 0; row < nodes_[col].size(); row++)
			if (auto node = DCast<ViewItemNode>(nodes_[col][row]))
			{
				node->setPosition(QPoint(col, row));
				if (node->spacingParent())
					node->setSpacingParentPosition(positionOfNode(node->spacingParent()));
				nodes.append(node->toJson());
			}
	//Store all the arrows
	QJsonArray arrows;
	for (auto key : arrows_.keys())
		for (auto pair : arrows_[key])
			arrows.append(arrowToJson(pair, key));
	QJsonObject main;
	main.insert("nodes", nodes);
	main.insert("arrows", arrows);
	main.insert("name", name());
	QJsonDocument result;
	result.setObject(main);
	return result;
}

void ViewItem::fromJson(QJsonDocument json)
{
	nodes_.clear();
	arrows_.clear();
	QJsonObject obj = json.object();
	setName(obj["name"].toString());
	QStringList steps{"NODE", "SPACING", "INFO"};
	int step = 0;
	auto objArray = obj["nodes"].toArray();
	//Load the nodes: First normal nodes, then spacing nodes, and lastly info nodes
	for (step = 0; step < steps.size(); step++)
		for (int i = 0; i < objArray.size(); i++)
		{
			auto current = objArray[i].toObject();
			if (current["type"] == steps[step])
				insertViewItemNode(ViewItemNode::fromJson(current, this),
								   current["col"].toInt(), current["row"].toInt());
		}
	//Load the arrows
	auto arrowArray = obj["arrows"].toArray();
	for (auto arrow : arrowArray)
		arrowFromJson(arrow.toObject());
}

QJsonObject ViewItem::arrowToJson(QPair<Item*, Item*> arrow, QString layer) const
{
	QJsonObject json;
	json.insert("layer", layer);
	//The first/second nodes might be a ViewItemNode -> in that case handled by the parent
	if (arrow.first->node()->manager())
		json.insert("node1", arrow.first->node()->manager()->
								 nodeIdMap().id(arrow.first->node()).toString());
	if (arrow.second->node()->manager())
		json.insert("node2", arrow.second->node()->manager()->
								 nodeIdMap().id(arrow.second->node()).toString());
	auto parent1 = arrow.first->findAncestorOfType<VViewItemNode>();
	json.insert("parent1col", positionOfItem(parent1).x());
	json.insert("parent1row", positionOfItem(parent1).y());
	auto parent2 = arrow.second->findAncestorOfType<VViewItemNode>();
	json.insert("parent2col", positionOfItem(parent2).x());
	json.insert("parent2row", positionOfItem(parent2).y());
	return json;
}

void ViewItem::arrowFromJson(QJsonObject json)
{
	auto parent1 = DCast<ViewItemNode>(nodeAt(json["parent1col"].toInt(), json["parent1row"].toInt()));
	auto parent2 = DCast<ViewItemNode>(nodeAt(json["parent2col"].toInt(), json["parent2row"].toInt()));
	Model::Node* node1{}, *node2{};
	if (json.contains("node1"))
		node1 = JsonUtil::nodeForId(QUuid(json["node1"].toString()));
	else node1 = parent1;
	if (json.contains("node2"))
		node2 = JsonUtil::nodeForId(QUuid(json["node2"].toString()));
	else node2 = parent2;
	if (node1 && node2)
		arrowsToAdd_.append(ArrowToAdd{parent1, node1, parent2, node2, json["layer"].toString()});
}

}
