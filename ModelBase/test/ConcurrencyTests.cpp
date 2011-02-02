/***********************************************************************************************************************
 * ConcurrencyTests.cpp
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "test_nodes/BinaryNode.h"
#include "test_nodes/BinaryNodeAccessUnit.h"
#include "Model.h"
#include "nodes/Integer.h"
#include "nodes/Text.h"

namespace Model {

TEST(ModelBase, SingleWriteUnitCheck)
{
	Model model;

	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root, "make tree");
	TestNodes::BinaryNode* left = root->setLeft<TestNodes::BinaryNode>();
	TestNodes::BinaryNode* right = root->setRight<TestNodes::BinaryNodeAccessUnit>();
	TestNodes::BinaryNode* one = root->left()->setLeft<TestNodes::BinaryNodeAccessUnit>();
	TestNodes::BinaryNode* two = root->left()->setRight<TestNodes::BinaryNodeAccessUnit>();
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

	model.beginModification(NULL);
	model.undo();
	model.endModification();

	CHECK_STR_EQUAL(QString(), root->name()->get());
	CHECK_STR_EQUAL(QString(), left->name()->get());
	CHECK_STR_EQUAL(QString(), right->name()->get());
	CHECK_STR_EQUAL(QString(), one->name()->get());
	CHECK_STR_EQUAL(QString(), two->name()->get());

	model.beginModification(NULL);
	model.redo();
	model.endModification();

	CHECK_STR_EQUAL("This is ok", root->name()->get());
	CHECK_STR_EQUAL(QString(), left->name()->get());
	CHECK_STR_EQUAL(QString(), right->name()->get());
	CHECK_STR_EQUAL("one set", one->name()->get());
	CHECK_STR_EQUAL(QString(), two->name()->get());
}

}
