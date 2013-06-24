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
#include "test_nodes/BinaryNodeAccessUnit.h"
#include "model/Model.h"
#include "nodes/Integer.h"
#include "nodes/Text.h"

namespace Model {

TEST(ModelBase, SingleWriteUnitCheck)
{
	auto root = new TestNodes::BinaryNode();
	Model model(root);

	model.beginModification(root, "make tree");
	TestNodes::BinaryNode* left = new TestNodes::BinaryNode();
	TestNodes::BinaryNode* right = new TestNodes::BinaryNodeAccessUnit();
	TestNodes::BinaryNode* one = new TestNodes::BinaryNodeAccessUnit();
	TestNodes::BinaryNode* two = new TestNodes::BinaryNodeAccessUnit();
	root->setLeft(left);
	root->setRight(right);
	root->left()->setLeft(one);
	root->left()->setRight(two);
	model.endModification();

	CHECK_STR_EQUAL(QString(), root->name()->get());
	CHECK_STR_EQUAL(QString(), left->name()->get());
	CHECK_STR_EQUAL(QString(), right->name()->get());
	CHECK_STR_EQUAL(QString(), one->name()->get());
	CHECK_STR_EQUAL(QString(), two->name()->get());

	model.beginModification(root, "Modify root");

	root->name()->set("This is ok");
	CHECK_STR_EQUAL("This is ok", root->name()->get());

	CHECK_FOR_EXCEPTION(ModelException, one->name()->set("This should fail"));
	CHECK_STR_EQUAL(QString(), one->name()->get());

	model.changeModificationTarget(one);
	CHECK_FOR_EXCEPTION(ModelException, root->name()->set("This should fail"));
	CHECK_STR_EQUAL("This is ok", root->name()->get());

	one->name()->set("one set");
	CHECK_STR_EQUAL("one set", one->name()->get());

	model.endModification();

	model.beginModification(nullptr);
	model.undo();
	model.endModification();

	CHECK_STR_EQUAL(QString(), root->name()->get());
	CHECK_STR_EQUAL(QString(), left->name()->get());
	CHECK_STR_EQUAL(QString(), right->name()->get());
	CHECK_STR_EQUAL(QString(), one->name()->get());
	CHECK_STR_EQUAL(QString(), two->name()->get());

	model.beginModification(nullptr);
	model.redo();
	model.endModification();

	CHECK_STR_EQUAL("This is ok", root->name()->get());
	CHECK_STR_EQUAL(QString(), left->name()->get());
	CHECK_STR_EQUAL(QString(), right->name()->get());
	CHECK_STR_EQUAL("one set", one->name()->get());
	CHECK_STR_EQUAL(QString(), two->name()->get());
}

}
