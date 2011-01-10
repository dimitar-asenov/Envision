/***********************************************************************************************************************
 * ConcurrencyTests.cpp
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "test_nodes/BinaryNode.h"
#include "test_nodes/BinaryWithPosition.h"
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
	TestNodes::BinaryNode* left = root->makeLeftNode();
	TestNodes::BinaryNode* right = root->makeRightNode("BinaryNodeAccessUnit");
	TestNodes::BinaryNode* one = root->left()->makeLeftNode("BinaryNodeAccessUnit");
	TestNodes::BinaryNode* two = root->left()->makeRightNode("BinaryNodeAccessUnit");
	model.endModification();

	CHECK_STR_EQUAL(QString(), root->text()->get());
	CHECK_STR_EQUAL(QString(), left->text()->get());
	CHECK_STR_EQUAL(QString(), right->text()->get());
	CHECK_STR_EQUAL(QString(), one->text()->get());
	CHECK_STR_EQUAL(QString(), two->text()->get());

	model.beginModification(root, "Modify root");

	root->text()->set("This is ok");
	CHECK_STR_EQUAL("This is ok", root->text()->get());

	CHECK_FOR_EXCEPTION(ModelException, one->text()->set("This should fail"));
	CHECK_STR_EQUAL(QString(), one->text()->get());

	model.changeModificationTarget(one);
	CHECK_FOR_EXCEPTION(ModelException, root->text()->set("This should fail"));
	CHECK_STR_EQUAL("This is ok", root->text()->get());

	one->text()->set("one set");
	CHECK_STR_EQUAL("one set", one->text()->get());

	model.endModification();

	model.beginModification(NULL);
	model.undo();
	model.endModification();

	CHECK_STR_EQUAL(QString(), root->text()->get());
	CHECK_STR_EQUAL(QString(), left->text()->get());
	CHECK_STR_EQUAL(QString(), right->text()->get());
	CHECK_STR_EQUAL(QString(), one->text()->get());
	CHECK_STR_EQUAL(QString(), two->text()->get());

	model.beginModification(NULL);
	model.redo();
	model.endModification();

	CHECK_STR_EQUAL("This is ok", root->text()->get());
	CHECK_STR_EQUAL(QString(), left->text()->get());
	CHECK_STR_EQUAL(QString(), right->text()->get());
	CHECK_STR_EQUAL("one set", one->text()->get());
	CHECK_STR_EQUAL(QString(), two->text()->get());
}

}
