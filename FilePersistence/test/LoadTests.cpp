/***********************************************************************************************************************
 * SaveTests.cpp
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

#include <QtCore/QTextStream>
#include <QtCore/QFile>

namespace FilePersistence {

TEST(FilePersistence, LoadRootOnly)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(store, "rootOnly");
	BinaryNode* root = dynamic_cast<BinaryNode*> (model.getRoot());

	CHECK_STR_EQUAL("BinaryNode", root->getTypeName() );
	CHECK_STR_EQUAL("Title", root->text()->get() );
	CHECK_CONDITION(root->left() == NULL);
	CHECK_CONDITION(root->right() == NULL);
}

TEST(FilePersistence, LoadModeNodesSingleUnitOnly)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(store, "2Children");
	BinaryNode* root = dynamic_cast<BinaryNode*> (model.getRoot());

	CHECK_STR_EQUAL("BinaryNode", root->getTypeName() );
	CHECK_STR_EQUAL("Root", root->text()->get() );
	CHECK_CONDITION(root->left() != NULL);
	CHECK_CONDITION(root->right() != NULL);
	CHECK_STR_EQUAL("BinaryNode", root->left()->getTypeName() );
	CHECK_STR_EQUAL("Left child", root->left()->text()->get() );
	CHECK_CONDITION(root->left()->left() == NULL);
	CHECK_CONDITION(root->left()->right() == NULL);
	CHECK_STR_EQUAL("BinaryNode", root->right()->getTypeName() );
	CHECK_STR_EQUAL("Right child", root->right()->text()->get() );
	CHECK_CONDITION(root->right()->left() == NULL);
	CHECK_CONDITION(root->right()->right() == NULL);
}

TEST(FilePersistence, LoadMultipleUnits)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(store, "units");
	BinaryNode* root = dynamic_cast<BinaryNode*> (model.getRoot());

	CHECK_STR_EQUAL("BinaryNode", root->getTypeName() );
	CHECK_STR_EQUAL("Root", root->text()->get() );
	CHECK_CONDITION(root->left() != NULL);
	CHECK_CONDITION(root->right() != NULL);
	CHECK_STR_EQUAL("BinaryNodeUnit", root->left()->getTypeName() );
	CHECK_STR_EQUAL("Left child", root->left()->text()->get() );
	CHECK_CONDITION(root->left()->left() != NULL);
	CHECK_CONDITION(root->left()->right() == NULL);
	CHECK_STR_EQUAL("BinaryNode", root->left()->left()->getTypeName() );
	CHECK_STR_EQUAL("in a new unit", root->left()->left()->text()->get() );
	CHECK_CONDITION(root->left()->left()->left() == NULL);
	CHECK_CONDITION(root->left()->left()->right() == NULL);
	CHECK_STR_EQUAL("BinaryNode", root->right()->getTypeName() );
	CHECK_STR_EQUAL("Right child", root->right()->text()->get() );
	CHECK_CONDITION(root->right()->left() == NULL);
	CHECK_CONDITION(root->right()->right() == NULL);
}

}
