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
#include "ModelBase/headers/TypedListInstantiations.h"

#include <QtCore/QTextStream>
#include <QtCore/QFile>

namespace FilePersistence {

TEST(FilePersistence, LoadingTypedList)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(&store, "typedList");
	Model::TypedList<Model::Text>* list = dynamic_cast<Model::TypedList<Model::Text>*> (model.root());
	CHECK_CONDITION(list != NULL);

	CHECK_STR_EQUAL("TypedListOfText", list->typeName() );
	CHECK_STR_EQUAL("TypedList", list->referenceName() );
	CHECK_INT_EQUAL(0, list->id());
	CHECK_INT_EQUAL(2, list->size());

	Model::Text* one = list->at(0);
	Model::Text* two = list->at(1);

	CHECK_CONDITION(one != NULL);
	CHECK_STR_EQUAL("one", one->get());
	CHECK_INT_EQUAL(2, one->id());

	CHECK_CONDITION(two != NULL);
	CHECK_STR_EQUAL("two", two->get());
	CHECK_INT_EQUAL(3, two->id())
}

TEST(FilePersistence, SavingTypedList)
{
	QString testDir = QDir::tempPath() + "/Envision/FilePersistence/tests";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	Model::TypedList<Model::Text>* list = dynamic_cast<Model::TypedList<Model::Text>*> (model.createRoot("TypedListOfText"));

	model.beginModification(list, "create");
	list->setReferenceName("TypedList");
	list->append<Model::Text>()->set("one");
	list->append<Model::Text>()->set("two");
	model.endModification();

	model.setName("typedList");
	model.save(&store);

	CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/typedList/typedList", testDir + "/typedList/typedList");
}

}
