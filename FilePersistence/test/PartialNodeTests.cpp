/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#include "filepersistence.h"
#include "FileStore.h"
#include "SimpleTextFileStore.h"
#include "ModelBase/src/test_nodes/PartialList.h"
#include "SelfTest/src/SelfTestSuite.h"
#include "ModelBase/src/model/Model.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/nodes/List.h"

using namespace Model;

namespace FilePersistence {

TEST(FilePersistence, LoadingList)
{
	for(int i = 0; i<2; ++i)
	{
		PersistentStore* store{};

		if (i==0)
		{
			auto s = new FileStore();
			s->setBaseFolder(":/FilePersistence/test/persisted");
			store = s;
		}
		else if (i==1)
		{
			auto s = new SimpleTextFileStore();
			s->setBaseFolder(":/FilePersistence/test/persisted/simple");
			store = s;
		}


		Model::Model model;
		model.load(store, "partial");
		TestNodes::PartialList* root = dynamic_cast<TestNodes::PartialList*> (model.root());
		CHECK_CONDITION(root != nullptr);

		List* list = root->list();

		CHECK_CONDITION(list != nullptr);
		CHECK_STR_EQUAL("List", list->typeName() );
		CHECK_CONDITION(list->isFullyLoaded() == false);

		list->loadFully(*store);

		CHECK_CONDITION(list->isFullyLoaded());
		CHECK_INT_EQUAL(4, list->size());

		Text* one = list->at<Text>(0);
		Text* two = list->at<Text>(1);
		Text* three = list->at<Text>(2);
		Text* four = list->at<Text>(3);

		CHECK_CONDITION(one != nullptr);
		CHECK_STR_EQUAL("one", one->get());

		CHECK_CONDITION(two != nullptr);
		CHECK_STR_EQUAL("two", two->get());

		CHECK_CONDITION(three != nullptr);
		CHECK_STR_EQUAL("three", three->get());

		CHECK_CONDITION(four != nullptr);
		CHECK_STR_EQUAL("four", four->get());

		SAFE_DELETE(store);
	}
}

TEST(FilePersistence, SaveList)
{
	for(int i = 0; i<2; ++i)
	{
		PersistentStore* store{};
		QString testDir;

		if (i==0)
		{
			auto s = new FileStore();
			testDir = QDir::tempPath() + "/Envision/FilePersistence/tests";
			s->setBaseFolder(testDir);
			store = s;
		}
		else if (i==1)
		{
			auto s = new SimpleTextFileStore();
			testDir = QDir::tempPath() + "/Envision/FilePersistence/tests/simple";
			s->setBaseFolder(testDir);
			store = s;
		}

		auto root = new TestNodes::PartialList;
		Model::Model model(root);

		model.beginModification(root, "create ");
		auto one = new Text();
		one->set("one");
		root->list()->append(one);
		auto two = new Text();
		two->set("two");
		root->list()->append(two);
		auto three = new Text();
		three->set("three");
		root->list()->append(three);
		auto four = new Text();
		four->set("four");
		root->list()->append(four);
		model.endModification();

		model.setName("partial");
		model.save(store);

		if (i==0)
		{
			CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/partial/partial", testDir + "/partial/partial");
		}
		else if ( i==1 )
		{
			CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/simple/partial/partial", testDir +"/partial/partial");
		}

		SAFE_DELETE(store);
	}
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

	model.beginModification(root->list(), "fake modification ");
	model.endModification();

	CHECK_CONDITION(root->list()->isFullyLoaded() == false);
	model.setName("partialResave");
	model.save(&store);
	CHECK_CONDITION(root->list()->isFullyLoaded() == false);

	CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/partialResave/partialResave", destDir + "/partialResave/partialResave");
}

}
