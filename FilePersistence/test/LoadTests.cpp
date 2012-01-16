/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * LoadTests.cpp
 *
 *  Created on: Dec 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "filepersistence.h"
#include "FileStore.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "ModelBase/headers/test_nodes/BinaryNode.h"
#include "ModelBase/headers/Model.h"
#include "ModelBase/headers/nodes/Integer.h"
#include "ModelBase/headers/nodes/Text.h"

#include <QtCore/QTextStream>
#include <QtCore/QFile>

namespace FilePersistence {

TEST(FilePersistence, LoadRootOnly)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(&store, "rootOnly");
	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.root());

	CHECK_CONDITION(root);
	CHECK_STR_EQUAL("BinaryNode", root->typeName() );
	CHECK_STR_EQUAL("Title", root->name()->get() );
	CHECK_CONDITION(root->left() == nullptr);
	CHECK_CONDITION(root->right() == nullptr);
}

TEST(FilePersistence, LoadModeNodesSingleUnitOnly)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(&store, "2Children");
	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.root());

	CHECK_STR_EQUAL("BinaryNode", root->typeName() );
	CHECK_STR_EQUAL("Root", root->name()->get() );
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
}

TEST(FilePersistence, LoadMultipleUnits)
{
	QString testDir = ":/FilePersistence/test/persisted";
	Model::Model model;
	FileStore store;
	store.setBaseFolder(testDir);

	model.load(&store, "units");
	TestNodes::BinaryNode* root = dynamic_cast<TestNodes::BinaryNode*> (model.root());

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
}

}
