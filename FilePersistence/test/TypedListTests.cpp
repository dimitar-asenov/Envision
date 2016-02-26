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

#include "../src/FilePersistencePlugin.h"
#include "../src/simple/SimpleTextFileStore.h"
#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"
#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Text.h"

using namespace Model;

namespace Model {
	class PersistentStore;
}

namespace FilePersistence {

class FILEPERSISTENCE_API LoadingTypedList
 : public SelfTest::Test<FilePersistencePlugin, LoadingTypedList> { public: void test()
{
	PersistentStore* store{};

	auto s = new SimpleTextFileStore{};
	s->setBaseFolder(":/FilePersistence/test/persisted");
	store = s;

	TreeManager manager;
	manager.load(store, "typedList", false);

	auto list = DCast<TypedList<Text>> (manager.root());
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
}};

class FILEPERSISTENCE_API SavingTypedList
 : public SelfTest::Test<FilePersistencePlugin, SavingTypedList> { public: void test()
{
	PersistentStore* store{};
	QString testDir;

	auto s = new SimpleTextFileStore{};
	testDir = QDir::tempPath() + "/Envision/FilePersistence/tests";
	s->setBaseFolder(testDir);
	store = s;

	auto list = new TypedList<Text>{};
	TreeManager manager{list};

	manager.beginModification(list, "create");
	auto one = new Text{};
	one->set("one");
	list->append(one);
	auto two = new Text{};
	two->set("two");
	list->append(two);
	manager.endModification();
	manager.nodeIdMap().setId(list, "{00000000-0000-0000-0000-000000000001}");
	manager.nodeIdMap().setId(one, "{00000000-0000-0000-0000-000000000002}");
	manager.nodeIdMap().setId(two, "{00000000-0000-0000-0000-000000000003}");

	manager.setName("typedList");
	manager.save(store);

	CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/typedList/typedList",
									  testDir +"/typedList/typedList");

	SAFE_DELETE(store);
}};

}
