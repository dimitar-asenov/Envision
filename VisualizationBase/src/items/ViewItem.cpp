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

#include "../declarative/DeclarativeItem.hpp"
#include "../declarative/DynamicGridFormElement.h"
#include "../nodes/ViewItemNode.h"
#include "VViewItemNode.h"
#include "../overlays/ArrowOverlay.h"
#include "../nodes/InfoNode.h"
#include "VInfoNode.h"
#include "../Scene.h"
#include "RootItem.h"
#include "../renderer/ModelRenderer.h"
#include "ModelBase/src/model/AllTreeManagers.h"
#include "../VisualizationManager.h"
#include "../ViewItemManager.h"

#include "ModelBase/src/model/TreeManager.h"

namespace Visualization {

DEFINE_ITEM_COMMON(ViewItem, "item")

ViewItem::ViewItem(Item* parent, QString name, StyleType* style) :
		Super{parent, style}, name_{name}
{
	Q_ASSERT(isValidName(name));
}

void ViewItem::initializeForms()
{
	addForm((new DynamicGridFormElement{})
			->setSpacing(10, 10)->setMargins(10)
			->setNodesGetter([](Item* v)
				{ auto self = static_cast<I*>(v);
				  return self->nodesGetter(); })
			->setMajorAxisGetter([](Item* v)
				{ auto self = static_cast<I*>(v);
				  return self->majorAxis_; }));
}

bool ViewItem::isValidName(const QString &name)
{
	if (name == "Empty slot")
		return false;
	if (auto scene = VisualizationManager::instance().mainScene())
		for (auto vector : scene->viewItems()->viewItems())
			for (auto item : vector)
				if (item && item->name() == name) return false;
	return true;
}

int ViewItem::publicInterfacePurpose()
{
	int purpose = Scene::defaultRenderer()->purposeId("public_interface");
	return purpose >= 0 ? purpose
		: Scene::defaultRenderer()->registerVisualizationPurpose("public_interface");
}

void ViewItem::insertMajor(int major)
{
	//Make sure we actually have enough major elements
	ensureMajorExists(major);

	//Only insert the major element, if the current one at index is not empty
	auto isEmpty = true;
	for (auto item : nodes_.at(major))
		isEmpty = isEmpty && item == nullptr;

	//If we have minor elements, insert a new major element (else we can use the existing empty major element)
	if (!isEmpty)
		nodes_.insert(major, {});
}

ViewItemNode* ViewItem::insertNode(Model::Node* node, MajorMinorIndex pos, int purpose)
{
	auto ref = ViewItemNode::withReference(node, purpose);
	insertViewItemNode(ref, pos);
	return ref;
}

void ViewItem::removeNode(Model::Node* node)
{
	auto index = positionOfNode(node);
	if (index.major_ != -1)
	{
		auto removed = nodes_[index.major_][index.minor_];
		nodes_[index.major_].remove(index.minor_);
		cleanupRemovedNode(removed);
		setUpdateNeeded(StandardUpdate);
	}
}

QList<Model::Node*> ViewItem::allNodes() const
{
	QList<Model::Node*> result;
	for (auto major : nodes_)
		for (auto item : major)
			if (item)
				result.append(item);
	return result;
}

MajorMinorIndex ViewItem::positionOfNode(Model::Node *node) const
{
	if (!node) return {-1, -1};

	for (int major = 0; major < nodes_.size(); major++)
		for (int minor = 0; minor < nodes_[major].size(); minor++)
			if (nodes_[major][minor] == node ||
					DCast<ViewItemNode>(nodes_[major][minor])->reference() == node)
				return {major, minor};

	return {-1, -1};
}

MajorMinorIndex ViewItem::positionOfItem(Item *item) const
{
	if (item->node()) return positionOfNode(item->node());
	else return {-1, -1};
}

Model::Node* ViewItem::nodeAt(MajorMinorIndex index) const
{
	if (index.major_ < 0 || index.major_ >= nodes_.size())
		return nullptr;
	if (index.minor_ < 0 || index.minor_ >= nodes_[index.major_].size())
		return nullptr;
	return nodes_[index.major_][index.minor_];
}

void ViewItem::cleanupRemovedItem(Item *item)
{
	Q_ASSERT(item);

	//Handle all the spacing nodes depending on the item
	for (auto current : allNodes())
		if (auto topLevelItem = item->findAncestorOfType<VViewItemNode>())
			if (auto viewNode = DCast<ViewItemNode>(current))
				if (topLevelItem->node() == viewNode->spacingParent())
				{
					viewNode->setSpacingParent(nullptr);
					viewNode->setSpacingTarget(nullptr);
				}

	removeArrowsForItem(item);
}

void ViewItem::cleanupRemovedNode(Model::Node *node)
{
	Q_ASSERT(node);

	//Remove all the nodes referencing the deleted node
	for (auto current : allNodes())
		if (auto viewNode = DCast<ViewItemNode>(current))
			if (viewNode->reference() == node)
				removeNode(viewNode);

	//Remove all the info nodes that reference the deleted node
	for (auto infoNode : referencesOfType<InfoNode>())
		if (infoNode->target() == node)
			removeNode(infoNode);
}

void ViewItem::addSpacing(MajorMinorIndex index, Model::Node* spacingTarget,
						  ViewItemNode* spacingParent)
{
	insertViewItemNode(ViewItemNode::withSpacingTarget(spacingTarget, spacingParent), index);
}

void ViewItem::addArrow(Model::Node *from, Model::Node *to, QString layer,
						ViewItemNode *fromParent, ViewItemNode *toParent)
{
	arrowsToAdd_.append(ArrowToAdd{fromParent, from, toParent, to, layer});
	setUpdateNeeded(StandardUpdate);
}

void ViewItem::setArrowStyle(QString layer, QString styleName)
{
	arrowStyles_.insert(layer, styleName);
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

			bool addedArrow = false;
			for (auto from : allFrom)
				for (auto to : allTo)
					//TODO@cyril The node is rendered as a RootItem -> needs a hack here
					//or removing a ViewItemNode with arrows doesn't work correctly
					if (!DCast<RootItem>(from) && !DCast<RootItem>(to))
					{
						arrows_[line.layer_].append(QPair<Item*, Item*>(from, to));
						addedArrow = true;
					}
			if (!addedArrow) arrowsToAdd_.append(line);
		}
	}
	//And then return the arrows in the layer
	return arrows_[layer];
}

QStringList ViewItem::arrowLayers() const
{
	auto result = arrows_.keys();
	for (auto name : disabledArrowLayers_)
		if (!result.contains(name))
			result.append(name);
	return result;
}

void ViewItem::setMajorAxis(GridLayouter::MajorAxis majorAxis)
{
	majorAxis_ = majorAxis;
	setUpdateNeeded(StandardUpdate);
}

void ViewItem::setName(const QString &name)
{
	Q_ASSERT(isValidName(name));
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
					info->automaticUpdate();
					vis->setUpdateNeeded(StandardUpdate);
				}
		}
	}
	//Must make sure to add the necessary arrow layers here, as sometimes they are added
	//before the view is added to the scene
	for (auto arrow : arrowsToAdd_)
		if (!arrows_.contains(arrow.layer_) && scene())
			addArrowLayer(arrow.layer_, !disabledArrowLayers_.contains(arrow.layer_));
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

void ViewItem::addArrowLayer(QString layer, bool enabled)
{
	auto layerName = fullLayerName(layer);
	auto arrowLayer = scene()->overlayGroup(layerName);
	if (!arrowLayer) arrowLayer = scene()->addOverlayGroup(layerName);
	arrowLayer->setOverlayConstructor2Args([this, layer](Item* from, Item* to){
		return makeOverlay(
					new ArrowOverlay{from, to, ArrowOverlay::itemStyles().get(arrowStyles_.value(layer, ""))}
					);
	});
	arrowLayer->setDynamic2Items([this, layer](){return arrowsForLayer(layer);});
	if (enabled) arrowLayer->show();
	else arrowLayer->hide();
}

void ViewItem::removeArrowsForItem(Item *parent)
{
	//Remove all arrows where the arrow's item is a child of the removed item
	for (auto key : arrows_.keys())
	{
		auto copy = arrows_[key];
		for (auto pair : copy)
			if (parent == pair.first || parent == pair.second ||
				parent->isAncestorOf(pair.first) || parent->isAncestorOf(pair.second))
				arrows_[key].removeAll(pair);
	}
}

void ViewItem::insertViewItemNode(ViewItemNode *node, MajorMinorIndex index)
{
	//First, make sure the current grid is big enough to fit the node
	ensurePositionExists(index);

	//We can either put the node at a free space if exists, or insert otherwise
	if (nodes_[index.major_][index.minor_] == nullptr)
		nodes_[index.major_][index.minor_] = node;
	else
		nodes_[index.major_].insert(index.minor_, node);
	setUpdateNeeded(StandardUpdate);
}

void ViewItem::ensurePositionExists(MajorMinorIndex index)
{
	ensureMajorExists(index.major_);
	if (nodes_[index.major_].size() <= index.minor_)
		nodes_[index.major_].resize(index.minor_ + 1);
}

void ViewItem::ensureMajorExists(int major)
{
	if (nodes_.size() <= major)
		nodes_.resize(major + 1);
}

//JSON storing/loading
QJsonDocument ViewItem::toJson() const
{
	//Store all the nodes
	QJsonArray nodes;
	for (int major = 0; major < nodes_.size(); major++)
		for (int minor = 0; minor < nodes_[major].size(); minor++)
			if (auto node = DCast<ViewItemNode>(nodes_[major][minor]))
			{
				node->setPosition({major, minor});
				if (node->spacingParent())
					node->setSpacingParentPosition(positionOfNode(node->spacingParent()));
				nodes.append(node->toJson());
			}

	//Store all the arrows
	QJsonArray arrows;
	for (auto key : arrows_.keys())
		for (auto pair : arrows_[key])
			arrows.append(arrowToJson(pair, key));

	//Store arrow layer styles
	QJsonObject arrowLayerStyles;
	for (QString key : arrowStyles_.keys())
		arrowLayerStyles.insert(key, arrowStyles_[key]);

	//Remember which arrow layers were disabled
	QJsonArray disabledLayers;
	for (auto name : arrows_.keys())
		if (scene()->overlayGroup(fullLayerName(name))
			&& !scene()->overlayGroup(fullLayerName(name))->isVisible())
			disabledLayers.append(name);

	QJsonObject main;
	main.insert("nodes", nodes);
	main.insert("arrows", arrows);
	main.insert("arrowLayerStyles", arrowLayerStyles);
	main.insert("disabledLayers", disabledLayers);
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
									{current["majorIndex"].toInt(), current["minorIndex"].toInt()});
		}
	//Load the arrows
	auto arrowArray = obj["arrows"].toArray();
	for (auto arrow : arrowArray)
		arrowFromJson(arrow.toObject());

	//Load the arrow layer styles
	auto arrowLayerStylesObject = obj["arrowLayerStyles"].toObject();
		for (auto it = arrowLayerStylesObject.begin(); it != arrowLayerStylesObject.end(); it++)
			arrowStyles_.insert(it.key(), it.value().toString());

	//Load which arrow layers were disabled
	for (auto name : obj["disabledLayers"].toArray())
		disabledArrowLayers_.append(name.toString());
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
	json.insert("parent1major", positionOfItem(parent1).major_);
	json.insert("parent1minor", positionOfItem(parent1).minor_);
	auto parent2 = arrow.second->findAncestorOfType<VViewItemNode>();
	json.insert("parent2major", positionOfItem(parent2).major_);
	json.insert("parent2minor", positionOfItem(parent2).minor_);
	return json;
}

void ViewItem::arrowFromJson(QJsonObject json)
{
	auto parent1 = DCast<ViewItemNode>(nodeAt({json["parent1major"].toInt(), json["parent1minor"].toInt()}));
	auto parent2 = DCast<ViewItemNode>(nodeAt({json["parent2major"].toInt(), json["parent2minor"].toInt()}));
	Model::Node* node1{}, *node2{};
	if (json.contains("node1"))
		node1 = Model::AllTreeManagers::instance().nodeForId(QUuid{json["node1"].toString()});
	else node1 = parent1;
	if (json.contains("node2"))
		node2 = Model::AllTreeManagers::instance().nodeForId(QUuid{json["node2"].toString()});
	else node2 = parent2;
	if (node1 && node2)
		addArrow(node1, node2, json["layer"].toString(), parent1, parent2);
}

template <typename NodeType>
inline QList<NodeType*> ViewItem::referencesOfType() const
{
	QList<NodeType*> result;
	for (auto node : allNodes())
		if (auto viewNode = DCast<ViewItemNode>(node))
			if (auto reference = DCast<NodeType>(viewNode->reference()))
				result.append(reference);
	return result;
}

QVector<QVector<Model::Node*>> ViewItem::nodesGetter()
{
	return nodes_;
}

}
