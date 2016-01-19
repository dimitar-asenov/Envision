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
#include "ModelBase/src/test_nodes/PartialList.h"
#include "SelfTest/src/SelfTestSuite.h"
#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/nodes/List.h"

using namespace Model;

namespace FilePersistence {

class LoadingPartialList : public Test<FilePersistencePlugin, LoadingPartialList> { public: void test()
{
	PersistentStore* store{};

	auto s = new SimpleTextFileStore{};
	s->setBaseFolder(":/FilePersistence/test/persisted");
	store = s;

	Model::TreeManager manager;
	manager.load(store, "partial", true);
	TestNodes::PartialList* root = DCast<TestNodes::PartialList> (manager.root());
	CHECK_CONDITION(root != nullptr);

	List* list = root->list();

	CHECK_CONDITION(list != nullptr);
	CHECK_STR_EQUAL("List", list->typeName() );
	CHECK_CONDITION(list->isPartiallyLoaded());
	CHECK_INT_EQUAL(0, list->size());

	SAFE_DELETE(store);
}};

class LoadingFullList : public Test<FilePersistencePlugin, LoadingFullList> { public: void test()
{
	PersistentStore* store{};

	auto s = new SimpleTextFileStore{};
	s->setBaseFolder(":/FilePersistence/test/persisted");
	store = s;

	Model::TreeManager manager;
	manager.load(store, "partial", false);
	TestNodes::PartialList* root = DCast<TestNodes::PartialList> (manager.root());
	CHECK_CONDITION(root != nullptr);

	List* list = root->list();

	CHECK_CONDITION(list != nullptr);
	CHECK_STR_EQUAL("List", list->typeName() );

	CHECK_CONDITION(!list->isPartiallyLoaded());
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
}};

}
