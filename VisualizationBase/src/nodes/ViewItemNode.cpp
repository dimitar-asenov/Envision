/***********************************************************************************************************************
**
** Copyright (c) 2015 ETH Zurich
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

#include "ViewItemNode.h"


#include "../nodes/InfoNode.h"
#include "../items/ViewItem.h"
#include "../utils/JsonUtil.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
template class Model::TypedList<Visualization::ViewItemNode>;

namespace Visualization {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(ViewItemNode)

ViewItemNode::ViewItemNode(Model::Node *)
	:Super{}
{
}

ViewItemNode::ViewItemNode(Model::Node *, Model::PersistentStore &, bool)
	:Super{}
{
	Q_ASSERT(false);
}

ViewItemNode* ViewItemNode::clone() const { return new ViewItemNode{*this}; }

ViewItemNode* ViewItemNode::withSpacingTarget(Model::Node *spacingTarget, ViewItemNode* spacingParent)
{
	auto result = new ViewItemNode{};
	result->setSpacingTarget(spacingTarget);
	result->setSpacingParent(spacingParent);
	return result;
}

ViewItemNode* ViewItemNode::withReference(Model::Node *reference, int purpose)
{
	auto result = new ViewItemNode{};
	result->setReference(reference);
	result->setPurpose(purpose);
	return result;
}

ViewItemNode* ViewItemNode::fromJson(QJsonObject json, const ViewItem *view)
{
	auto result = new ViewItemNode{};
	if (!json.contains("type"))
		return result;
	result->setPurpose(json["purpose"].toInt());
	if (json["type"] == "NODE")
	{
		if (auto ref = JsonUtil::nodeForId(QUuid(json["reference"].toString())))
			result->setReference(ref);
	}
	else if (json["type"] == "SPACING")
	{
		if (auto target = JsonUtil::nodeForId(QUuid(json["target"].toString())))
			result->setSpacingTarget(target);
		if (json["parentRow"].toInt() != -1)
			result->setSpacingParent(DCast<ViewItemNode>(view->nodeAt(json["parentCol"].toInt(),
																		json["parentRow"].toInt())));
	}
	else if (json["type"] == "INFO")
	{
		if (auto target = JsonUtil::nodeForId(QUuid(json["target"].toString())))
			result->setReference(new InfoNode{target, json["content"].toArray()});
	}
	return result;
}

QJsonValue ViewItemNode::toJson() const
{
	QJsonObject result;
	result.insert("purpose", purpose());
	result.insert("col", position_.x());
	result.insert("row", position_.y());
	//If it stores a normal, separately persisted node
	if (reference() && reference()->manager())
	{
		result.insert("reference", reference()->manager()->
					nodeIdMap().id(reference()).toString());
		result.insert("type", "NODE");
	}
	//If the node handles spacing only
	else if (!reference() && spacingTarget())
	{
		result.insert("target", spacingTarget()->manager()->
								nodeIdMap().id(spacingTarget()).toString());
		result.insert("parentCol", spacingParentPosition_.x());
		result.insert("parentRow", spacingParentPosition_.y());
		result.insert("type", "SPACING");
	}
	//If it stores an InfoNode, which is not separately persisted
	else if (auto infoNode = DCast<InfoNode>(reference()))
	{
		result.insert("content", infoNode->toJson());
		result.insert("target", infoNode->target()->manager()->
					   nodeIdMap().id(infoNode->target()).toString());
		result.insert("type", "INFO");
	}
	return result;
}
}
