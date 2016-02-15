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
#include "SelfTest/src/SelfTestSuite.h"
#include "ModelBase/src/test_nodes/BinaryNode.h"
#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Integer.h"
#include "ModelBase/src/nodes/Text.h"

using namespace Model;

namespace FilePersistence {

class LoadRootOnly : public SelfTest::Test<FilePersistencePlugin, LoadRootOnly> { public: void test()
{

	PersistentStore* store{};

	auto s = new SimpleTextFileStore{};
	s->setBaseFolder(":/FilePersistence/test/persisted");
	store = s;

	Model::TreeManager manager;
	manager.load(store, "rootOnly", false);
	TestNodes::BinaryNode* root = DCast<TestNodes::BinaryNode> (manager.root());

	CHECK_CONDITION(root);
	CHECK_STR_EQUAL("BinaryNode", root->typeName() );
	CHECK_STR_EQUAL("Title", root->name()->get() );
	CHECK_CONDITION(root->left() == nullptr);
	CHECK_CONDITION(root->right() == nullptr);

	SAFE_DELETE(store);
}};

class LoadModeNodesSingleUnitOnly : public SelfTest::Test<FilePersistencePlugin, LoadModeNodesSingleUnitOnly> {
public: void test()
{

	PersistentStore* store{};

	auto s = new SimpleTextFileStore{};
	s->setBaseFolder(":/FilePersistence/test/persisted");
	store = s;

	Model::TreeManager manager;
	manager.load(store, "2Children", false);
	TestNodes::BinaryNode* root = DCast<TestNodes::BinaryNode> (manager.root());

	CHECK_STR_EQUAL("BinaryNode", root->typeName() );
	CHECK_STR_EQUAL("RootNode", root->name()->get() );
	CHECK_CONDITION(root->left() != nullptr);
	CHECK_CONDITION(root->right() != nullptr);
	CHECK_STR_EQUAL("BinaryNode", root->left()->typeName() );
	CHECK_STR_EQUAL("Left child", root->left()->name()->get() );
	CHECK_CONDITION(root->left()->left() == nullptr);
	CHECK_CONDITION(root->left()->right() == nullptr);
	CHECK_STR_EQUAL("BinaryNode", root->right()->typeName() );
	CHECK_STR_EQUAL("Right child", root->right()->name()->get() );
	CHECK_CONDITION(root->right()->left() == nullptr);
	CHECK_CONDITION(root->right()->right() == nullptr);

	SAFE_DELETE(store);
}};

class LoadMultipleUnits : public SelfTest::Test<FilePersistencePlugin, LoadMultipleUnits> { public: void test()
{
	PersistentStore* store{};

	auto s = new SimpleTextFileStore{};
	s->setBaseFolder(":/FilePersistence/test/persisted");
	store = s;

	Model::TreeManager manager;

	manager.load(store, "units", false);
	TestNodes::BinaryNode* root = DCast<TestNodes::BinaryNode> (manager.root());

	CHECK_STR_EQUAL("BinaryNode", root->typeName() );
	CHECK_STR_EQUAL("Root", root->name()->get() );
	CHECK_CONDITION(root->left() != nullptr);
	CHECK_CONDITION(root->right() != nullptr);
	CHECK_STR_EQUAL("BinaryNodePersistenceUnit", root->left()->typeName() );
	CHECK_STR_EQUAL("Left child", root->left()->name()->get() );
	CHECK_CONDITION(root->left()->left() != nullptr);
	CHECK_CONDITION(root->left()->right() == nullptr);
	CHECK_STR_EQUAL("BinaryNode", root->left()->left()->typeName() );
	CHECK_STR_EQUAL("in a new unit", root->left()->left()->name()->get() );
	CHECK_CONDITION(root->left()->left()->left() == nullptr);
	CHECK_CONDITION(root->left()->left()->right() == nullptr);
	CHECK_STR_EQUAL("BinaryNode", root->right()->typeName() );
	CHECK_STR_EQUAL("Right child", root->right()->name()->get() );
	CHECK_CONDITION(root->right()->left() == nullptr);
	CHECK_CONDITION(root->right()->right() == nullptr);

	SAFE_DELETE(store);
}};

}
