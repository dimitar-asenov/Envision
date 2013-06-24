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
#include "ModelBase/src/test_nodes/PartialList.h"
#include "SelfTest/src/SelfTestSuite.h"
#include "ModelBase/src/model/Model.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/nodes/List.h"

namespace FilePersistence {

TEST(FilePersistence, LoadingList)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(&store, "partial");
	TestNodes::PartialList* root = dynamic_cast<TestNodes::PartialList*> (model.root());
	CHECK_CONDITION(root != nullptr);

	Model::List* list = root->list();

	CHECK_CONDITION(list != nullptr);
	CHECK_STR_EQUAL("List", list->typeName() );
	CHECK_CONDITION(list->isFullyLoaded() == false);

	list->loadFully(store);

	CHECK_CONDITION(list->isFullyLoaded());
	CHECK_INT_EQUAL(4, list->size());

	Model::Text* one = list->at<Model::Text>(0);
	Model::Text* two = list->at<Model::Text>(1);
	Model::Text* three = list->at<Model::Text>(2);
	Model::Text* four = list->at<Model::Text>(3);

	CHECK_CONDITION(one != nullptr);
	CHECK_STR_EQUAL("one", one->get());

	CHECK_CONDITION(two != nullptr);
	CHECK_STR_EQUAL("two", two->get());

	CHECK_CONDITION(three != nullptr);
	CHECK_STR_EQUAL("three", three->get());

	CHECK_CONDITION(four != nullptr);
	CHECK_STR_EQUAL("four", four->get());
}

TEST(FilePersistence, SaveList)
{
	QString testDir = QDir::tempPath() + "/Envision/FilePersistence/tests";
	FileStore store;
	store.setBaseFolder(testDir);

	auto root = new TestNodes::PartialList;
	Model::Model model(root);

	model.beginModification(root, "create ");
	Model::Text* one = new Model::Text();
	one->set("one");
	root->list()->append(one);
	Model::Text* two = new Model::Text();
	two->set("two");
	root->list()->append(two);
	Model::Text* three = new Model::Text();
	three->set("three");
	root->list()->append(three);
	Model::Text* four = new Model::Text();
	four->set("four");
	root->list()->append(four);
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

	model.beginModification(root->list(), "fake modification ");
	model.endModification();

	CHECK_CONDITION(root->list()->isFullyLoaded() == false);
	model.setName("partialResave");
	model.save(&store);
	CHECK_CONDITION(root->list()->isFullyLoaded() == false);

	CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/partialResave/partialResave", destDir + "/partialResave/partialResave");
}

}
