/***********************************************************************************************************************
 * SaveTests.cpp
 *
 *  Created on: Dec 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "filepersistence.h"
#include "FileStore.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "ModelBase/headers/test_nodes/BinaryNode.h"
#include "ModelBase/headers/Model.h"
#include "ModelBase/headers/nodes/Integer.h"
#include "ModelBase/headers/nodes/Text.h"

#include <QtCore/QTextStream>
#include <QtCore/QFile>

namespace FilePersistence {

TEST(FilePersistence, SaveRootOnly)
{
	QString testDir = QDir::tempPath() + "/Envision/FilePersistence/tests";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root->name(), "set title");
	root->name()->set("Title");
	model.endModification();

	store.saveModel(model, "rootOnly");

	CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/rootOnly/rootOnly", testDir + "/rootOnly/rootOnly");
}

TEST(FilePersistence, SaveModeNodesSingleUnitOnly)
{
	QString testDir = QDir::tempPath() + QDir::toNativeSeparators("/Envision/FilePersistence/tests");
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root, "set title");
	root->name()->set("Root");
	TestNodes::BinaryNode* left = root->makeLeftNode();
	TestNodes::BinaryNode* right = root->makeRightNode();

	left->name()->set("Left child");
	right->name()->set("Right child");
	model.endModification();

	store.saveModel(model, "2Children");
	CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/2Children/2Children", testDir + "/2Children/2Children");
}

TEST(FilePersistence, SaveMultipleUnits)
{
	QString testDir = QDir::tempPath() + QDir::toNativeSeparators("/Envision/FilePersistence/tests");
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root, "set title");
	root->name()->set("Root");
	TestNodes::BinaryNode* left = root->makeLeftNode("BinaryNodePersistenceUnit");
	TestNodes::BinaryNode* right = root->makeRightNode();

	left->name()->set("Left child");
	TestNodes::BinaryNode* leftleft = left->makeLeftNode();
	leftleft->name()->set("in a new unit");
	right->name()->set("Right child");
	model.endModification();

	store.saveModel(model, "units");
	CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/units/units", testDir + "/units/units");
	CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/units/2", testDir + "/units/2");
}

}
