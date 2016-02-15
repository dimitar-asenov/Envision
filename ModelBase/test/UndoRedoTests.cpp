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
#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"
#include "../src/test_nodes/BinaryNode.h"
#include "../src/model/TreeManager.h"
#include "../src/nodes/Text.h"

namespace Model {

class MODELBASE_API UndoRedoTextSet : public SelfTest::Test<ModelBasePlugin, UndoRedoTextSet> { public: void test()
{
	auto root = new TestNodes::BinaryNode{};
	TreeManager manager{root};

	CHECK_CONDITION(root->name()->get().isNull());
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(0, root->revision());

	manager.beginModification(root->name(), "testing");
	root->name()->set("t1");
	manager.endModification();
	CHECK_CONDITION(root->name()->get() == "t1");
	CHECK_INT_EQUAL(1, root->name()->revision());
	CHECK_INT_EQUAL(1, root->revision());

	manager.beginModification(root->name(), "testing");
	root->name()->set("t222");
	manager.endModification();
	CHECK_CONDITION(root->name()->get() == "t222");
	CHECK_INT_EQUAL(2, root->name()->revision());
	CHECK_INT_EQUAL(2, root->revision());

	manager.beginModification(nullptr);
	manager.undo();
	CHECK_CONDITION(root->name()->get() == "t1");
	CHECK_INT_EQUAL(1, root->name()->revision());
	CHECK_INT_EQUAL(1, root->revision());

	manager.undo();
	CHECK_CONDITION(root->name()->get().isNull());
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(0, root->revision());

	manager.redo();
	CHECK_CONDITION(root->name()->get() == "t1");
	CHECK_INT_EQUAL(1, root->name()->revision());
	CHECK_INT_EQUAL(1, root->revision());

	manager.redo();
	CHECK_CONDITION(root->name()->get() == "t222");
	CHECK_INT_EQUAL(2, root->name()->revision());
	CHECK_INT_EQUAL(2, root->revision());
	manager.endModification();
}};

class MODELBASE_API UndoRedoOptionalNodes : public SelfTest::Test<ModelBasePlugin, UndoRedoOptionalNodes>
{ public: void test()
{
	auto root = new TestNodes::BinaryNode{};
	TreeManager manager{root};

	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(0, root->revision());
	CHECK_CONDITION(root->left() == nullptr);
	CHECK_CONDITION(root->right() == nullptr);

	manager.beginModification(root, "testing");
	TestNodes::BinaryNode* left = new TestNodes::BinaryNode{};
	root->setLeft(left);
	manager.endModification();
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(1, root->revision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == nullptr);

	CHECK_INT_EQUAL(0, left->name()->revision());
	CHECK_INT_EQUAL(0, left->revision());

	manager.beginModification(root, "testing");
	TestNodes::BinaryNode* right = new TestNodes::BinaryNode{};
	root->setRight(right);
	manager.endModification();
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(2, root->revision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == right);

	CHECK_INT_EQUAL(0, left->name()->revision());
	CHECK_INT_EQUAL(0, left->revision());

	CHECK_INT_EQUAL(0, right->name()->revision());
	CHECK_INT_EQUAL(0, right->revision());

	CHECK_CONDITION(left != right);

	manager.beginModification(nullptr);
	manager.undo();
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(1, root->revision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == nullptr);

	CHECK_INT_EQUAL(0, left->name()->revision());
	CHECK_INT_EQUAL(0, left->revision());

	manager.undo();
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(0, root->revision());
	CHECK_CONDITION(root->left() == nullptr);
	CHECK_CONDITION(root->right() == nullptr);

	manager.redo();
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(1, root->revision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == nullptr);

	CHECK_INT_EQUAL(0, left->name()->revision());
	CHECK_INT_EQUAL(0, left->revision());

	manager.redo();
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(2, root->revision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == right);

	CHECK_INT_EQUAL(0, left->name()->revision());
	CHECK_INT_EQUAL(0, left->revision());

	CHECK_INT_EQUAL(0, right->name()->revision());
	CHECK_INT_EQUAL(0, right->revision());

	CHECK_CONDITION(left != right);

	manager.endModification();
}};

class MODELBASE_API UndoRedoGroupTextSet : public SelfTest::Test<ModelBasePlugin, UndoRedoGroupTextSet>
{ public: void test()
{
	auto root = new Text{};
	TreeManager manager{root};

	CHECK_INT_EQUAL(0, root->revision());

	manager.beginModification(root, "Modification group");
	root->set("change1");
	root->set("change2");
	manager.endModification();

	CHECK_INT_EQUAL(2, root->revision());
	CHECK_CONDITION( root->get() == "change2");

	manager.beginModification(nullptr);
	manager.undo();
	CHECK_INT_EQUAL(0, root->revision());
	CHECK_CONDITION( root->get().isNull());

	manager.redo();
	CHECK_INT_EQUAL(2, root->revision());
	CHECK_CONDITION( root->get() == "change2");

	manager.endModification();
}};

}
