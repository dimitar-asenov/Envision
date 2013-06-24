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

TEST(FilePersistence, LoadingTypedList)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(&store, "typedList");
	Model::TypedList<Model::Text>* list = dynamic_cast<Model::TypedList<Model::Text>*> (model.root());
	CHECK_CONDITION(list != nullptr);

	CHECK_STR_EQUAL("TypedListOfText", list->typeName() );
	CHECK_INT_EQUAL(2, list->size());

	Model::Text* one = list->at(0);
	Model::Text* two = list->at(1);

	CHECK_CONDITION(one != nullptr);
	CHECK_STR_EQUAL("one", one->get());

	CHECK_CONDITION(two != nullptr);
	CHECK_STR_EQUAL("two", two->get());
}

TEST(FilePersistence, SavingTypedList)
{
	QString testDir = QDir::tempPath() + "/Envision/FilePersistence/tests";
	FileStore store;
	store.setBaseFolder(testDir);

	auto list = new Model::TypedList<Model::Text>;
	Model::Model model(list);

	model.beginModification(list, "create");
	Model::Text* one = new Model::Text();
	one->set("one");
	list->append(one);
	Model::Text* two = new Model::Text();
	two->set("two");
	list->append(two);
	model.endModification();

	model.setName("typedList");
	model.save(&store);

	CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/typedList/typedList", testDir + "/typedList/typedList");
}

}
