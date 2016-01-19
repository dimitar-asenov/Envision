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

#include "../src/ModelBasePlugin.h"
#include "SelfTest/src/SelfTestSuite.h"
#include "../src/test_nodes/BinaryNode.h"
#include "../src/test_nodes/BinaryNodeAccessUnit.h"
#include "../src/model/TreeManager.h"
#include "../src/nodes/NameText.h"
#include "../src/nodes/Integer.h"
#include "../src/nodes/Reference.h"

namespace Model {

class CompositeMetaData : public Test<ModelBasePlugin, CompositeMetaData> { public: void test()
{
	AttributeChain& metaExt = TestNodes::BinaryNode::getMetaData();
	AttributeChain& metaUnit = TestNodes::BinaryNodeAccessUnit::getMetaData();

	CHECK_INT_EQUAL(2, metaExt.numLevels());
	CHECK_INT_EQUAL(3, metaUnit.numLevels());

	CHECK_INT_EQUAL(5, metaExt.size());
	CHECK_INT_EQUAL(0, metaUnit.size());
	CHECK_CONDITION( metaUnit.level(0) == &CompositeNode::getMetaData());
	CHECK_CONDITION( metaUnit.level(1) == &metaExt);

	CHECK_STR_EQUAL("name", metaExt[0].name());
	CHECK_STR_EQUAL("left", metaExt[1].name());
	CHECK_STR_EQUAL("right", metaExt[2].name());
	CHECK_STR_EQUAL("_ext_PositionExtension_x", metaExt[3].name());
	CHECK_STR_EQUAL("_ext_PositionExtension_y", metaExt[4].name());

	CHECK_STR_EQUAL("NameText", metaExt[0].type());
	CHECK_STR_EQUAL("BinaryNode", metaExt[1].type());
	CHECK_STR_EQUAL("BinaryNode", metaExt[2].type());
	CHECK_STR_EQUAL("Integer", metaExt[3].type());
	CHECK_STR_EQUAL("Integer", metaExt[4].type());

	CHECK_CONDITION(metaExt[0].optional() == false);
	CHECK_CONDITION(metaExt[1].optional() == true);
	CHECK_CONDITION(metaExt[2].optional() == true);
	CHECK_CONDITION(metaExt[3].optional() == false);
	CHECK_CONDITION(metaExt[4].optional() == false);
	
	CHECK_CONDITION(metaExt[0].partial() == false);
	CHECK_CONDITION(metaExt[1].partial() == false);
	CHECK_CONDITION(metaExt[2].partial() == false);
	CHECK_CONDITION(metaExt[3].partial() == false);
	CHECK_CONDITION(metaExt[4].partial() == false);
}};

class SimpleTreeManagerCreation : public Test<ModelBasePlugin, SimpleTreeManagerCreation> { public: void test()
{
	TreeManager manager;
	CHECK_CONDITION( manager.root() == nullptr );

	auto root = new TestNodes::BinaryNode{};
	manager.setRoot(root);
	CHECK_CONDITION( manager.root() == root );

	CHECK_CONDITION( root->manager() == &manager );

	CHECK_CONDITION( root->name()->manager() == &manager );
}};

class RemoveOptional : public Test<ModelBasePlugin, RemoveOptional> { public: void test()
{
	auto root = new TestNodes::BinaryNode{};
	TreeManager manager{root};

	manager.beginModification(root, "Making left node");
	TestNodes::BinaryNode* left = new TestNodes::BinaryNode{};
	root->setLeft(left);
	manager.endModification();
	CHECK_CONDITION( root->left() == left );
	CHECK_CONDITION( root->left() != nullptr );

	manager.beginModification(root, "Removing left node");
	root->removeLeftNode();
	manager.endModification();
	CHECK_CONDITION( root->left() == nullptr);

	manager.beginModification(root, "Making left node");
	root->setLeft(new TestNodes::BinaryNode{});
	manager.endModification();
	CHECK_CONDITION( root->left() != left );
	CHECK_CONDITION( root->left() != nullptr );
}};

class ChildNodeRetrieval : public Test<ModelBasePlugin, ChildNodeRetrieval> { public: void test()
{
	auto root = new TestNodes::BinaryNode{};
	TreeManager manager{root};

	manager.beginModification(root, "Making nodes");
	TestNodes::BinaryNode* left = new TestNodes::BinaryNode{};
	root->setLeft(left);
	TestNodes::BinaryNode* right = new TestNodes::BinaryNode{};
	root->setRight(right);
	manager.endModification();

	CHECK_CONDITION(root->hasAttribute("name"));
	CHECK_CONDITION(root->hasAttribute("left"));
	CHECK_CONDITION(root->hasAttribute("right"));

	CHECK_CONDITION(root->hasAttribute("another") == false);

	CHECK_CONDITION(root->get("name") == root->name());
	CHECK_CONDITION(root->get("left") == left);
	CHECK_CONDITION(root->get("right") == right);
}};

class ProperRegistration : public Test<ModelBasePlugin, ProperRegistration> { public: void test()
{
	auto root = new TestNodes::BinaryNode{};
	TreeManager manager{root};
	CHECK_CONDITION(root->typeId() > 0);

	TreeManager manager2;
	auto t = new Text{};
	manager2.setRoot(t);

	CHECK_CONDITION(t->typeId() > 0);
	CHECK_CONDITION(t->typeId() != root->typeId());

	TreeManager manager3;
	auto i = new Integer{};
	manager3.setRoot(i);
	CHECK_CONDITION(i->typeId() > 0);
	CHECK_CONDITION(i->typeId() != root->typeId());
	CHECK_CONDITION(i->typeId() != t->typeId());
}};

}
