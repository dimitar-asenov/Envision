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
#include "SelfTest/src/SelfTestSuite.h"
#include "ModelBase/src/test_nodes/BinaryNode.h"
#include "ModelBase/src/test_nodes/BinaryNodePersistenceUnit.h"
#include "ModelBase/src/model/Model.h"
#include "ModelBase/src/nodes/Integer.h"
#include "ModelBase/src/nodes/Text.h"

using namespace Model;

namespace FilePersistence {

TEST(FilePersistence, SaveRootOnly)
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

		auto root = new TestNodes::BinaryNode;
		Model::Model model(root);

		model.beginModification(root->name(), "set title");
		root->name()->set("Title");
		model.endModification();
		NodeIdMap::setId(root, "{00000000-0000-0000-0000-000000000001}");
		NodeIdMap::setId(root->name(), "{00000000-0000-0000-0000-000000000002}");

		model.setName("rootOnly");
		model.save(store);

		if (i==0)
		{
			CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/rootOnly/rootOnly",
					testDir + "/rootOnly/rootOnly");
		}
		else if ( i==1 )
		{
			CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/simple/rootOnly/rootOnly",
					testDir +"/rootOnly/rootOnly");
		}

		SAFE_DELETE(store);
	}
}

TEST(FilePersistence, SaveModeNodesSingleUnitOnly)
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

		auto root = new TestNodes::BinaryNode;
		Model::Model model(root);

		model.beginModification(root, "set title");
		root->name()->set("RootNode");
		TestNodes::BinaryNode* left = new TestNodes::BinaryNode();
		root->setLeft(left);
		TestNodes::BinaryNode* right = new TestNodes::BinaryNode();
		root->setRight(right);

		left->name()->set("Left child");
		right->name()->set("Right child");
		model.endModification();
		NodeIdMap::setId(root, "{00000000-0000-0000-0000-000000000001}");
		NodeIdMap::setId(root->name(), "{00000000-0000-0000-0000-000000000002}");
		NodeIdMap::setId(root->left(), "{00000000-0000-0000-0000-000000000003}");
		NodeIdMap::setId(root->left()->name(), "{00000000-0000-0000-0000-000000000004}");
		NodeIdMap::setId(root->right(), "{00000000-0000-0000-0000-000000000005}");
		NodeIdMap::setId(root->right()->name(), "{00000000-0000-0000-0000-000000000006}");

		model.setName("2Children");
		model.save(store);

		if (i==0)
		{
			CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/2Children/2Children",
					testDir + "/2Children/2Children");
		}
		else if ( i==1 )
		{
			CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/simple/2Children/2Children",
					testDir +"/2Children/2Children");
		}

		SAFE_DELETE(store);
	}
}

TEST(FilePersistence, SaveMultipleUnits)
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

		auto root = new TestNodes::BinaryNode;
		Model::Model model(root);

		model.beginModification(root, "set title");
		root->name()->set("Root");
		TestNodes::BinaryNode* left = new TestNodes::BinaryNodePersistenceUnit();
		root->setLeft(left);
		TestNodes::BinaryNode* right = new TestNodes::BinaryNode();
		root->setRight(right);

		left->name()->set("Left child");
		TestNodes::BinaryNode* leftleft = new TestNodes::BinaryNode();
		left->setLeft(leftleft);
		leftleft->name()->set("in a new unit");
		right->name()->set("Right child");
		model.endModification();
		NodeIdMap::setId(root, "{00000000-0000-0000-0000-000000000001}");
		NodeIdMap::setId(root->name(), "{00000000-0000-0000-0000-000000000002}");
		NodeIdMap::setId(root->left(), "{00000000-0000-0000-0000-000000000003}");
		NodeIdMap::setId(root->left()->name(), "{00000000-0000-0000-0000-000000000004}");
		NodeIdMap::setId(root->left()->left(), "{00000000-0000-0000-0000-000000000005}");
		NodeIdMap::setId(root->left()->left()->name(), "{00000000-0000-0000-0000-000000000006}");
		NodeIdMap::setId(root->right(), "{00000000-0000-0000-0000-000000000007}");
		NodeIdMap::setId(root->right()->name(), "{00000000-0000-0000-0000-000000000008}");

		model.setName("units");
		model.save(store);

		if (i==0)
		{
			CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/units/units", testDir + "/units/units");
			CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/units/{00000000-0000-0000-0000-000000000003}",
					testDir + "/units/{00000000-0000-0000-0000-000000000003}");
		}
		else if ( i==1 )
		{
			CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/simple/units/units", testDir +"/units/units");
			CHECK_TEXT_FILES_EQUAL(":/FilePersistence/test/persisted/simple/units/{00000000-0000-0000-0000-000000000003}",
					testDir +"/units/{00000000-0000-0000-0000-000000000003}");
		}

		SAFE_DELETE(store);
	}
}

}
