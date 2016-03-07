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
#include "../src/ModelException.h"
#include "../src/model/TreeManager.h"
#include "../src/test_nodes/BinaryNode.h"
#include "../src/test_nodes/BinaryNodeAccessUnit.h"

#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"

namespace Model {

class MODELBASE_API SingleWriteUnitCheck : public SelfTest::Test<ModelBasePlugin, SingleWriteUnitCheck>
{ public: void test()
{
	auto root = new TestNodes::BinaryNode{};
	TreeManager manager{root};

	manager.beginModification(root, "make tree");
	TestNodes::BinaryNode* left = new TestNodes::BinaryNode{};
	TestNodes::BinaryNode* right = new TestNodes::BinaryNodeAccessUnit{};
	TestNodes::BinaryNode* one = new TestNodes::BinaryNodeAccessUnit{};
	TestNodes::BinaryNode* two = new TestNodes::BinaryNodeAccessUnit{};
	root->setLeft(left);
	root->setRight(right);
	root->left()->setLeft(one);
	root->left()->setRight(two);
	manager.endModification();

	CHECK_STR_EQUAL(QString{}, root->name()->get());
	CHECK_STR_EQUAL(QString{}, left->name()->get());
	CHECK_STR_EQUAL(QString{}, right->name()->get());
	CHECK_STR_EQUAL(QString{}, one->name()->get());
	CHECK_STR_EQUAL(QString{}, two->name()->get());

	manager.beginModification(root, "Modify root");

	root->name()->set("This is ok");
	CHECK_STR_EQUAL("This is ok", root->name()->get());

	CHECK_FOR_EXCEPTION(ModelException, one->name()->set("This should fail"));
	CHECK_STR_EQUAL(QString{}, one->name()->get());

	manager.changeModificationTarget(one);
	CHECK_FOR_EXCEPTION(ModelException, root->name()->set("This should fail"));
	CHECK_STR_EQUAL("This is ok", root->name()->get());

	one->name()->set("one set");
	CHECK_STR_EQUAL("one set", one->name()->get());

	manager.endModification();

	manager.beginModification(nullptr);
	manager.undo();
	manager.endModification();

	CHECK_STR_EQUAL(QString{}, root->name()->get());
	CHECK_STR_EQUAL(QString{}, left->name()->get());
	CHECK_STR_EQUAL(QString{}, right->name()->get());
	CHECK_STR_EQUAL(QString{}, one->name()->get());
	CHECK_STR_EQUAL(QString{}, two->name()->get());

	manager.beginModification(nullptr);
	manager.redo();
	manager.endModification();

	CHECK_STR_EQUAL("This is ok", root->name()->get());
	CHECK_STR_EQUAL(QString{}, left->name()->get());
	CHECK_STR_EQUAL(QString{}, right->name()->get());
	CHECK_STR_EQUAL("one set", one->name()->get());
	CHECK_STR_EQUAL(QString{}, two->name()->get());
}};

}
