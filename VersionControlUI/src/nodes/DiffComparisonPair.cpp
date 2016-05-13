/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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
#include "DiffComparisonPair.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"

namespace VersionControlUI
{

DEFINE_NODE_TYPE_REGISTRATION_METHODS(DiffComparisonPair)

DiffComparisonPair::DiffComparisonPair(Model::Node* oldVersionNode, Model::Node* newVersionNode) : Super{}
{
	oldVersionNode_ = oldVersionNode;
	newVersionNode_ = newVersionNode;

	computeObjectPath();

	computeComponentName();

}

void DiffComparisonPair::setComparisonName(Model::Node* node, QString nodeObjectPath, QString componentName)
{
	comparisonName_ = nodeObjectPath;
	if (node->definesSymbol())
		comparisonName_ += node->symbolName();
	comparisonName_ += componentName;
}

void DiffComparisonPair::computeObjectPath()
{
	auto oldVersionObjectPath = computeObjectPath(oldVersionNode_);
	auto newVersionObjectPath = computeObjectPath(newVersionNode_);
	auto componentName = computeComponentName();

	Model::Node* comparisonNameNode;
	QString comparisonNameObjectPath;

	if (oldVersionObjectPath == newVersionObjectPath)
	{
		singleObjectPath_ = new Model::Text{oldVersionObjectPath+componentName};
		comparisonNameNode = oldVersionNode_;
		comparisonNameObjectPath = oldVersionObjectPath;
		twoObjectPathsDefined_ = false;
	}
	else if (oldVersionObjectPath == "" || newVersionObjectPath == "")
	{
		if (oldVersionObjectPath == "")
		{
			comparisonNameObjectPath = newVersionObjectPath;
			comparisonNameNode = newVersionNode_;
		}
		else
		{
			comparisonNameObjectPath = oldVersionObjectPath;
			comparisonNameNode = oldVersionNode_;
		}
		singleObjectPath_ = new Model::Text{comparisonNameObjectPath+componentName};
		twoObjectPathsDefined_ = false;
	}
	else
	{
		oldVersionObjectPath_ = new Model::Text{oldVersionObjectPath+componentName};
		newVersionObjectPath_ = new Model::Text{newVersionObjectPath+componentName};
		comparisonNameNode = oldVersionNode_;
		comparisonNameObjectPath = oldVersionObjectPath;
		twoObjectPathsDefined_ = true;
	}

	setComparisonName(comparisonNameNode, comparisonNameObjectPath, componentName);
}

QString DiffComparisonPair::computeObjectPath(Model::Node* node)
{
	if (!node) return "";
	auto parent = node->parent();
	QString objectPath = "";
	while (parent)
	{
		if (parent->definesSymbol())
			objectPath.prepend(parent->symbolName() + "/");
		parent = parent->parent();
	}

	return objectPath;
}

QString DiffComparisonPair::computeComponentName()
{
	Model::Node* nodeToComputeComponentType = nullptr;
	QString componentName = "";

	if (oldVersionNode_)
		nodeToComputeComponentType = oldVersionNode_;
	else
		nodeToComputeComponentType = newVersionNode_;

	auto searchCompositeNode = nodeToComputeComponentType;

	while (searchCompositeNode && !DCast<Model::CompositeNode>(searchCompositeNode))
		searchCompositeNode = searchCompositeNode->parent();

	if (searchCompositeNode)
	{
		auto compositeNode = DCast<Model::CompositeNode>(searchCompositeNode);
		if (compositeNode != nodeToComputeComponentType)
		{
			auto index = compositeNode->indexOf(nodeToComputeComponentType);
			componentName = compositeNode->meta().attribute(index).name();
		}
	}
	return componentName;
}

DiffComparisonPair::DiffComparisonPair(Model::Node *)
	:Super{}
{
	Q_ASSERT(false);
}

DiffComparisonPair::DiffComparisonPair(Model::Node *, Model::PersistentStore &, bool)
	:Super{}
{
	Q_ASSERT(false);
}

DiffComparisonPair* DiffComparisonPair::clone() const { return new DiffComparisonPair{*this}; }

QJsonValue DiffComparisonPair::toJson() const
{
	return QJsonValue{};
}


}
