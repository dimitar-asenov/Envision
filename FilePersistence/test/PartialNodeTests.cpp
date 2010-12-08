/***********************************************************************************************************************
 * PartialNodeTests.cpp
 *
 *  Created on: Dec 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "filepersistence.h"
#include "FileStore.h"
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

	model.load(store, "partial");
	Model::List* root = dynamic_cast<Model::List*> (model.getRoot());

	CHECK_CONDITION(root != NULL);
	CHECK_STR_EQUAL("List", root->getTypeName() );
	CHECK_STR_EQUAL("TheRoot", root->getReferenceName());
	CHECK_CONDITION(root->isFullyLoaded() == false);
	CHECK_INT_EQUAL(0, root->getId());
	CHECK_CONDITION(root->getChild(1) != NULL);

	root->loadFully(store);

	CHECK_CONDITION(root->isFullyLoaded());
	CHECK_INT_EQUAL(4, root->size());

	Model::Text* one = dynamic_cast<Model::Text*> ((*root)[0]);
	Model::Text* two = dynamic_cast<Model::Text*> ((*root)[1]);
	Model::Text* three = dynamic_cast<Model::Text*> ((*root)[2]);
	Model::Text* four = dynamic_cast<Model::Text*> ((*root)[3]);

	CHECK_CONDITION(one != NULL);
	CHECK_STR_EQUAL("one", one->get());
	CHECK_INT_EQUAL(2, one->getId());

	CHECK_CONDITION(two != NULL);
	CHECK_STR_EQUAL("two", two->get());
	CHECK_INT_EQUAL(3, two->getId())

	CHECK_CONDITION(three != NULL);
	CHECK_STR_EQUAL("three", three->get());
	CHECK_INT_EQUAL(4, three->getId())

	CHECK_CONDITION(four != NULL);
	CHECK_STR_EQUAL("four", four->get());
	CHECK_INT_EQUAL(5, four->getId())

}

}
