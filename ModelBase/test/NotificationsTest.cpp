/***********************************************************************************************************************
 * NotificationsTest.cpp
 *
 *  Created on: Nov 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "selftest/headers/SelfTestSuite.h"
#include "BinaryNode.h"
#include "Model.h"
#include "NotificationListener.h"

namespace Model {

TEST(ModelBase, ModificationNotificationTests)
{
	Model model;
	NotificationListener nl(model);

	CHECK_CONDITION(nl.root == NULL);
	CHECK_INT_EQUAL(0, nl.modifiedNodes.size());

	BinaryNode* root = dynamic_cast<BinaryNode*> (model.createRoot("BinaryNode"));

	CHECK_CONDITION(root == nl.root);

	model.beginModification(root, "make tree");
	BinaryNode* left = root->makeLeftNode();
	BinaryNode* right = root->makeRightNode();
	model.endModification();

	CHECK_INT_EQUAL(1, nl.modifiedNodes.size());
	CHECK_CONDITION(nl.modifiedNodes[0] == root);

	model.beginModification(left, "modify");
	left->text()->set("Left text");
	model.changeModificationTarget(right);
	right->text()->set("Right text");
	model.endModification();

	CHECK_INT_EQUAL(2, nl.modifiedNodes.size());
	CHECK_CONDITION(nl.modifiedNodes[0] == left);
	CHECK_CONDITION(nl.modifiedNodes[1] == right);

	nl.modifiedNodes.clear();
	model.beginModification(NULL);
	model.undo();
	model.undo();
	model.endModification();

	CHECK_INT_EQUAL(3, nl.modifiedNodes.size());
	CHECK_CONDITION(nl.modifiedNodes[0] == right);
	CHECK_CONDITION(nl.modifiedNodes[1] == left);
	CHECK_CONDITION(nl.modifiedNodes[2] == root);
}

}
