/***********************************************************************************************************************
 * ConcurrencyTests.cpp
 *
 *  Created on: Nov 25, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "selftest/headers/SelfTestSuite.h"
#include "BinaryNode.h"
#include "BinaryWithPosition.h"
#include "BinaryNodeUnit.h"
#include "Model.h"
#include "nodes/Integer.h"
#include "nodes/Text.h"

namespace Model {

TEST(ModelBase, SingleWriteUnitCheck)
{
	Model model;

	BinaryNode* root = dynamic_cast<BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root, "make tree");
	BinaryNode* left = root->makeLeftNode();
	BinaryNode* right = root->makeRightNode("BinaryNodeUnit");
	BinaryNode* one = root->left()->makeLeftNode("BinaryNodeUnit");
	BinaryNode* two = root->left()->makeRightNode("BinaryNodeUnit");
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
