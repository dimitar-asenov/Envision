/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "modelbase.h"
#include "SelfTest/src/SelfTestSuite.h"
#include "test_nodes/BinaryNode.h"
#include "model/Model.h"
#include "nodes/Text.h"

namespace Model {

TEST(ModelBase, UndoRedoTextSet)
{
	auto root = new TestNodes::BinaryNode();
	Model model(root);

	CHECK_CONDITION(root->name()->get().isNull());
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(0, root->revision());

	model.beginModification(root->name(), "testing");
	root->name()->set("t1");
	model.endModification();
	CHECK_CONDITION(root->name()->get() == "t1");
	CHECK_INT_EQUAL(1, root->name()->revision());
	CHECK_INT_EQUAL(1, root->revision());

	model.beginModification(root->name(),"testing");
	root->name()->set("t222");
	model.endModification();
	CHECK_CONDITION(root->name()->get() == "t222");
	CHECK_INT_EQUAL(2, root->name()->revision());
	CHECK_INT_EQUAL(2, root->revision());

	model.beginModification(nullptr);
	model.undo();
	CHECK_CONDITION(root->name()->get() == "t1");
	CHECK_INT_EQUAL(1, root->name()->revision());
	CHECK_INT_EQUAL(1, root->revision());

	model.undo();
	CHECK_CONDITION(root->name()->get().isNull());
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(0, root->revision());

	model.redo();
	CHECK_CONDITION(root->name()->get() == "t1");
	CHECK_INT_EQUAL(1, root->name()->revision());
	CHECK_INT_EQUAL(1, root->revision());

	model.redo();
	CHECK_CONDITION(root->name()->get() == "t222");
	CHECK_INT_EQUAL(2, root->name()->revision());
	CHECK_INT_EQUAL(2, root->revision());
	model.endModification();
}

TEST(ModelBase, UndoRedoOptionalNodes)
{
	auto root = new TestNodes::BinaryNode();
	Model model(root);

	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(0, root->revision());
	CHECK_CONDITION(root->left() == nullptr);
	CHECK_CONDITION(root->right() == nullptr);

	model.beginModification(root, "testing");
	TestNodes::BinaryNode* left = new TestNodes::BinaryNode();
	root->setLeft(left);
	model.endModification();
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(1, root->revision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == nullptr);

	CHECK_INT_EQUAL(0, left->name()->revision());
	CHECK_INT_EQUAL(0, left->revision());

	model.beginModification(root, "testing");
	TestNodes::BinaryNode* right = new TestNodes::BinaryNode();
	root->setRight(right);
	model.endModification();
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(2, root->revision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == right);

	CHECK_INT_EQUAL(0, left->name()->revision());
	CHECK_INT_EQUAL(0, left->revision());

	CHECK_INT_EQUAL(0, right->name()->revision());
	CHECK_INT_EQUAL(0, right->revision());

	CHECK_CONDITION(left != right);

	model.beginModification(nullptr);
	model.undo();
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(1, root->revision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == nullptr);

	CHECK_INT_EQUAL(0, left->name()->revision());
	CHECK_INT_EQUAL(0, left->revision());

	model.undo();
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(0, root->revision());
	CHECK_CONDITION(root->left() == nullptr);
	CHECK_CONDITION(root->right() == nullptr);

	model.redo();
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(1, root->revision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == nullptr);

	CHECK_INT_EQUAL(0, left->name()->revision());
	CHECK_INT_EQUAL(0, left->revision());

	model.redo();
	CHECK_INT_EQUAL(0, root->name()->revision());
	CHECK_INT_EQUAL(2, root->revision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == right);

	CHECK_INT_EQUAL(0, left->name()->revision());
	CHECK_INT_EQUAL(0, left->revision());

	CHECK_INT_EQUAL(0, right->name()->revision());
	CHECK_INT_EQUAL(0, right->revision());

	CHECK_CONDITION(left != right);

	model.endModification();
}

TEST(ModelBase, UndoRedoGroupTextSet)
{
	auto root = new Text();
	Model model(root);

	CHECK_INT_EQUAL(0, root->revision());

	model.beginModification(root, "Modification group");
	root->set("change1");
	root->set("change2");
	model.endModification();

	CHECK_INT_EQUAL(2, root->revision());
	CHECK_CONDITION( root->get() == "change2");

	model.beginModification(nullptr);
	model.undo();
	CHECK_INT_EQUAL(0, root->revision());
	CHECK_CONDITION( root->get().isNull());

	model.redo();
	CHECK_INT_EQUAL(2, root->revision());
	CHECK_CONDITION( root->get() == "change2");

	model.endModification();
}

}
