/***********************************************************************************************************************
 * NotificationsTest.cpp
 *
 *  Created on: Nov 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "test_nodes/BinaryNode.h"
#include "Model.h"
#include "NotificationListener.h"

namespace Model {

TEST(ModelBase, ModificationNotificationTests)
{
	Model model;
	NotificationListener nl(model);

	CHECK_CONDITION(nl.root == NULL);
	CHECK_INT_EQUAL(0, nl.modifiedNodes.size());

	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.createRoot("BinaryNode"));

	CHECK_CONDITION(root == nl.root);

	model.beginModification(root, "make tree");
	TestNodes::BinaryNode* left = root->setLeft<TestNodes::BinaryNode>();
	TestNodes::BinaryNode* right = root->setRight<TestNodes::BinaryNode>();
	model.endModification();

	CHECK_INT_EQUAL(1, nl.modifiedNodes.size());
	CHECK_CONDITION(nl.modifiedNodes[0] == root);

	model.beginModification(left, "modify");
	left->name()->set("Left text");
	model.changeModificationTarget(right);
	right->name()->set("Right text");
	model.endModification();

	CHECK_INT_EQUAL(4, nl.modifiedNodes.size());
	CHECK_CONDITION(nl.modifiedNodes[0] == left);
	CHECK_CONDITION(nl.modifiedNodes[1] == left->name());
	CHECK_CONDITION(nl.modifiedNodes[2] == right);
	CHECK_CONDITION(nl.modifiedNodes[3] == right->name());

	nl.modifiedNodes.clear();
	model.beginModification(NULL);
	model.undo();
	model.undo();
	model.endModification();

	CHECK_INT_EQUAL(5, nl.modifiedNodes.size());
	CHECK_CONDITION(nl.modifiedNodes[0] == right);
	CHECK_CONDITION(nl.modifiedNodes[1] == right->name());
	CHECK_CONDITION(nl.modifiedNodes[2] == left);
	CHECK_CONDITION(nl.modifiedNodes[3] == left->name());
	CHECK_CONDITION(nl.modifiedNodes[4] == root);
}

}
