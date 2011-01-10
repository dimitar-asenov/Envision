/***********************************************************************************************************************
 * UndoRedoTests.cpp
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "test_nodes/BinaryNode.h"
#include "Model.h"
#include "nodes/Text.h"

namespace Model {

TEST(ModelBase, UndoRedoTextSet)
{
	Model model;
	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.createRoot("BinaryNode"));

	CHECK_CONDITION(root->name()->get().isNull());
	CHECK_INT_EQUAL(0, root->name()->getRevision());
	CHECK_INT_EQUAL(0, root->getRevision());

	model.beginModification(root->name(), "testing");
	root->name()->set("t1");
	model.endModification();
	CHECK_CONDITION(root->name()->get() == "t1");
	CHECK_INT_EQUAL(1, root->name()->getRevision());
	CHECK_INT_EQUAL(1, root->getRevision());

	model.beginModification(root->name(),"testing");
	root->name()->set("t222");
	model.endModification();
	CHECK_CONDITION(root->name()->get() == "t222");
	CHECK_INT_EQUAL(2, root->name()->getRevision());
	CHECK_INT_EQUAL(2, root->getRevision());

	model.beginModification(NULL);
	model.undo();
	CHECK_CONDITION(root->name()->get() == "t1");
	CHECK_INT_EQUAL(1, root->name()->getRevision());
	CHECK_INT_EQUAL(1, root->getRevision());

	model.undo();
	CHECK_CONDITION(root->name()->get().isNull());
	CHECK_INT_EQUAL(0, root->name()->getRevision());
	CHECK_INT_EQUAL(0, root->getRevision());

	model.redo();
	CHECK_CONDITION(root->name()->get() == "t1");
	CHECK_INT_EQUAL(1, root->name()->getRevision());
	CHECK_INT_EQUAL(1, root->getRevision());

	model.redo();
	CHECK_CONDITION(root->name()->get() == "t222");
	CHECK_INT_EQUAL(2, root->name()->getRevision());
	CHECK_INT_EQUAL(2, root->getRevision());
	model.endModification();
}

TEST(ModelBase, UndoRedoOptionalNodes)
{
	Model model;
	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.createRoot("BinaryNode"));

	CHECK_INT_EQUAL(0, root->name()->getRevision());
	CHECK_INT_EQUAL(0, root->getRevision());
	CHECK_CONDITION(root->left() == NULL);
	CHECK_CONDITION(root->right() == NULL);

	model.beginModification(root, "testing");
	TestNodes::BinaryNode* left = root->makeLeftNode();
	model.endModification();
	CHECK_INT_EQUAL(0, root->name()->getRevision());
	CHECK_INT_EQUAL(1, root->getRevision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == NULL);

	CHECK_INT_EQUAL(0, left->name()->getRevision());
	CHECK_INT_EQUAL(0, left->getRevision());

	model.beginModification(root, "testing");
	TestNodes::BinaryNode* right = root->makeRightNode();
	model.endModification();
	CHECK_INT_EQUAL(0, root->name()->getRevision());
	CHECK_INT_EQUAL(2, root->getRevision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == right);

	CHECK_INT_EQUAL(0, left->name()->getRevision());
	CHECK_INT_EQUAL(0, left->getRevision());

	CHECK_INT_EQUAL(0, right->name()->getRevision());
	CHECK_INT_EQUAL(0, right->getRevision());

	CHECK_CONDITION(left != right);

	model.beginModification(NULL);
	model.undo();
	CHECK_INT_EQUAL(0, root->name()->getRevision());
	CHECK_INT_EQUAL(1, root->getRevision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == NULL);

	CHECK_INT_EQUAL(0, left->name()->getRevision());
	CHECK_INT_EQUAL(0, left->getRevision());

	model.undo();
	CHECK_INT_EQUAL(0, root->name()->getRevision());
	CHECK_INT_EQUAL(0, root->getRevision());
	CHECK_CONDITION(root->left() == NULL);
	CHECK_CONDITION(root->right() == NULL);

	model.redo();
	CHECK_INT_EQUAL(0, root->name()->getRevision());
	CHECK_INT_EQUAL(1, root->getRevision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == NULL);

	CHECK_INT_EQUAL(0, left->name()->getRevision());
	CHECK_INT_EQUAL(0, left->getRevision());

	model.redo();
	CHECK_INT_EQUAL(0, root->name()->getRevision());
	CHECK_INT_EQUAL(2, root->getRevision());
	CHECK_CONDITION(root->left() == left);
	CHECK_CONDITION(root->right() == right);

	CHECK_INT_EQUAL(0, left->name()->getRevision());
	CHECK_INT_EQUAL(0, left->getRevision());

	CHECK_INT_EQUAL(0, right->name()->getRevision());
	CHECK_INT_EQUAL(0, right->getRevision());

	CHECK_CONDITION(left != right);

	model.endModification();
}

TEST(ModelBase, UndoRedoGroupTextSet)
{
	Model model;
	Text* root = dynamic_cast<Text*> (model.createRoot("Text"));

	CHECK_INT_EQUAL(0, root->getRevision());

	model.beginModification(root, "Modification group");
	root->set("change1");
	root->set("change2");
	model.endModification();

	CHECK_INT_EQUAL(2, root->getRevision());
	CHECK_CONDITION( root->get() == "change2");

	model.beginModification(NULL);
	model.undo();
	CHECK_INT_EQUAL(0, root->getRevision());
	CHECK_CONDITION( root->get().isNull());

	model.redo();
	CHECK_INT_EQUAL(2, root->getRevision());
	CHECK_CONDITION( root->get() == "change2");

	model.endModification();
}

}
