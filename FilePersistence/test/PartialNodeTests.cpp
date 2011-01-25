/***********************************************************************************************************************
 * PartialNodeTests.cpp
 *
 *  Created on: Dec 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "filepersistence.h"
#include "FileStore.h"
#include "ModelBase/headers/test_nodes/PartialList.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "ModelBase/headers/Model.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/List.h"

#include <QtCore/QTextStream>
#include <QtCore/QFile>

namespace FilePersistence {

TEST(FilePersistence, LoadingList)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(&store, "partial");
	TestNodes::PartialList* root = dynamic_cast<TestNodes::PartialList*> (model.root());
	CHECK_CONDITION(root != NULL);

	Model::List* list = root->list();

	CHECK_CONDITION(list != NULL);
	CHECK_STR_EQUAL("List", list->getTypeName() );
	CHECK_STR_EQUAL("TheList", list->getReferenceName());
	CHECK_CONDITION(list->isFullyLoaded() == false);
	CHECK_INT_EQUAL(1, list->getId());

	list->loadFully(store);

	CHECK_CONDITION(list->isFullyLoaded());
	CHECK_INT_EQUAL(4, list->size());

	Model::Text* one = list->at<Model::Text>(0);
	Model::Text* two = list->at<Model::Text>(1);
	Model::Text* three = list->at<Model::Text>(2);
	Model::Text* four = list->at<Model::Text>(3);

	CHECK_CONDITION(one != NULL);
	CHECK_STR_EQUAL("one", one->get());
	CHECK_INT_EQUAL(3, one->getId());

	CHECK_CONDITION(two != NULL);
	CHECK_STR_EQUAL("two", two->get());
	CHECK_INT_EQUAL(4, two->getId())

	CHECK_CONDITION(three != NULL);
	CHECK_STR_EQUAL("three", three->get());
	CHECK_INT_EQUAL(5, three->getId())

	CHECK_CONDITION(four != NULL);
	CHECK_STR_EQUAL("four", four->get());
	CHECK_INT_EQUAL(6, four->getId());

}

TEST(FilePersistence, SaveList)
{
	QString testDir = QDir::tempPath() + "/Envision/FilePersistence/tests";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	TestNodes::PartialList* root = dynamic_cast<TestNodes::PartialList*> (model.createRoot("PartialList"));

	model.beginModification(root, "create ");
	root->list()->setReferenceName("TheList");
	root->list()->append<Model::Text>()->set("one");
	root->list()->append<Model::Text>()->set("two");
	root->list()->append<Model::Text>()->set("three");
	root->list()->append<Model::Text>()->set("four");
	model.endModification();

	model.setName("partial");
	model.save(&store);

	CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/partial/partial", testDir + "/partial/partial");
}

TEST(FilePersistence, ReSaveList)
{
	QString srcDir(":/FilePersistence/test/persisted");
	QString destDir(QDir::tempPath() + "/Envision/FilePersistence/tests");

	QFile src(srcDir + "/partial/partial");
	QFile dest(destDir + "/partialResave/partialResave");

	if (dest.exists())
	{
		bool removed = dest.remove();
		CHECK_CONDITION(removed);
	}

	if (!QDir(destDir + "/partialResave").exists())
	{
		bool createdDir = QDir().mkpath(destDir + "/partialResave");
		CHECK_CONDITION(createdDir);
	}
	bool copied = src.copy(dest.fileName());
	CHECK_CONDITION(copied);

	bool permissionOk = dest.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
	CHECK_CONDITION(permissionOk);

	Model::Model model;
	FileStore store;
	store.setBaseFolder(destDir);

	model.load(&store, "partialResave");
	TestNodes::PartialList* root = dynamic_cast<TestNodes::PartialList*> (model.root());

	CHECK_CONDITION(root->list()->isFullyLoaded() == false);
	CHECK_STR_EQUAL("TheList", root->list()->getReferenceName());
	CHECK_CONDITION(root->list()->isFullyLoaded() == false);

	model.beginModification(root, "create ");
	root->list()->setReferenceName("NewReference");
	model.endModification();

	CHECK_CONDITION(root->list()->isFullyLoaded() == false);
	model.setName("partialResave");
	model.save(&store);
	CHECK_CONDITION(root->list()->isFullyLoaded() == false);

	CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/partialResave/partialResave", destDir + "/partialResave/partialResave");
}

}
