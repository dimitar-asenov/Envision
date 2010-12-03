/***********************************************************************************************************************
 * SimpleTests.cpp
 *
 *  Created on: Dec 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "filepersistence.h"
#include "FileStore.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "BinaryNode.h"
#include "ModelBase/headers/Model.h"
#include "ModelBase/headers/nodes/Integer.h"
#include "ModelBase/headers/nodes/Text.h"

namespace FilePersistence {

TEST(FilePersistence, SaveRootOnly)
{
	Model::Model model;
	FileStore store;

	BinaryNode* root = dynamic_cast<BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root->text(), "set title");
	root->text()->set("Title");
	model.endModification();

	CHECK_STR_EQUAL("Title", root->text()->get());

	store.saveModel(model, "rootOnly");
}

TEST(FilePersistence, SaveModeNodesSingleUnitOnly)
{
	Model::Model model;
	FileStore store;

	BinaryNode* root = dynamic_cast<BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root, "set title");
	root->text()->set("Root");
	BinaryNode* left = root->makeLeftNode();
	BinaryNode* right = root->makeRightNode();

	left->text()->set("Left child");
	right->text()->set("Right child");
	model.endModification();

	CHECK_STR_EQUAL("Root", root->text()->get());

	store.saveModel(model, "2Children");
}

TEST(FilePersistence, SaveMultipleUnits)
{
	Model::Model model;
	FileStore store;

	BinaryNode* root = dynamic_cast<BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root, "set title");
	root->text()->set("Root");
	BinaryNode* left = root->makeLeftNode("BinaryNodeUnit");
	BinaryNode* right = root->makeRightNode();

	left->text()->set("Left child");
	BinaryNode* leftleft = left->makeLeftNode();
	leftleft->text()->set("in a new unit");
	right->text()->set("Right child");
	model.endModification();

	CHECK_STR_EQUAL("Root", root->text()->get());

	store.saveModel(model, "units");
}

}
