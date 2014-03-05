/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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
#include "simple/SimpleTextFileStore.h"
#include "ModelBase/src/test_nodes/PartialList.h"
#include "SelfTest/src/SelfTestSuite.h"
#include "ModelBase/src/model/Model.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/nodes/List.h"

using namespace Model;

namespace FilePersistence {

TEST(FilePersistence, LoadingTypedList)
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
		model.load(store, "typedList", false);

		auto list = dynamic_cast<TypedList<Text>*> (model.root());
		CHECK_CONDITION(list != nullptr);

		CHECK_STR_EQUAL("TypedListOfText", list->typeName() );
		CHECK_INT_EQUAL(2, list->size());

		Text* one = list->at(0);
		Text* two = list->at(1);

		CHECK_CONDITION(one != nullptr);
		CHECK_STR_EQUAL("one", one->get());

		CHECK_CONDITION(two != nullptr);
		CHECK_STR_EQUAL("two", two->get());

		SAFE_DELETE(store);
	}
}

TEST(FilePersistence, SavingTypedList)
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

		auto list = new TypedList<Text>;
		Model::Model model(list);

		model.beginModification(list, "create");
		auto one = new Text();
		one->set("one");
		list->append(one);
		auto two = new Text();
		two->set("two");
		list->append(two);
		model.endModification();

		model.setName("typedList");
		model.save(store);

		if (i==0)
		{
			CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/typedList/typedList", testDir + "/typedList/typedList");
		}
		else if ( i==1 )
		{
			CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/simple/typedList/typedList", testDir +"/typedList/typedList");
		}

		SAFE_DELETE(store);
	}
}

}
