/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
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

#include "FilePersistencePlugin.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "simple/GenericTree.h"

#include "version_control/ChangeDescription.h"
#include "version_control/Diff.h"
#include "version_control/GitRepository.h"


namespace FilePersistence {

TEST(FilePersistencePlugin, addAndRename)
{
	Q_ASSERT(std::system("tar -xf TestProject2.tar -C projects") == 0);
	GitRepository repo("projects/Foo");
	Diff diff = repo.diff("HEAD~1", "HEAD");
	auto renameChange = diff.changes().find(QUuid("ec98da71-76d5-4371-a491-18826bce2a35")).value();
	auto insertChange = diff.changes().find(QUuid("b32af2db-f0d0-4d36-ba00-96f27595248e")).value();
	auto structChange = diff.changes().find(QUuid("ee53ebac-d6a9-420f-b729-68ca160ee94e")).value();
	CHECK_CONDITION(renameChange->hasFlags(ChangeDescription::Value));
	CHECK_CONDITION(insertChange->type() == ChangeType::Insertion);
	CHECK_CONDITION(structChange->hasFlags(ChangeDescription::Structure));
	Q_ASSERT(std::system("rm -r projects/Foo") == 0);
}

TEST(FilePersistencePlugin, stmtEdit)
{
	Q_ASSERT(std::system("tar -xf TestProject1.tar -C projects") == 0);
	GitRepository repo("projects/TestProject1");
	Diff diff = repo.diff("HEAD~1", "HEAD");
	/*
	for (auto change : diff.changes())
	{
		change->print();
	}
	*/
	CHECK_CONDITION(diff.changes().size() > 0);
	Q_ASSERT(std::system("rm -r projects/TestProject1") == 0);
}

TEST(FilePersistencePlugin, deletion)
{
	Q_ASSERT(std::system("tar -xf TestDiff_Delete.tar -C projects") == 0);
	GitRepository repo("projects/TestDiff");
	Diff diff = repo.diff("HEAD~1", "HEAD");
	auto change1 = diff.changes().find(QUuid("00000000-0000-0000-0000-000000000017")).value();
	auto change2 = diff.changes().find(QUuid("00000000-0000-0000-0000-000000000016")).value();
	CHECK_CONDITION(change1->type() == ChangeType::Deletion);
	CHECK_CONDITION(change2->hasFlags(ChangeDescription::Structure));
	CHECK_CONDITION(diff.changes().size() == 2);
	Q_ASSERT(std::system("rm -r projects/TestDiff") == 0);
}

TEST(FilePersistencePlugin, insert)
{
	Q_ASSERT(std::system("tar -xf TestDiff_Insert.tar -C projects") == 0);
	GitRepository repo("projects/TestDiff");
	Diff diff = repo.diff("HEAD~1", "HEAD");
	auto change1 = diff.changes().find(QUuid("00000000-0000-0000-0000-000000000020")).value();
	auto change2 = diff.changes().find(QUuid("00000000-0000-0000-0000-000000000001")).value();
	CHECK_CONDITION(change1->type() == ChangeType::Insertion);
	CHECK_CONDITION(change2->hasFlags(ChangeDescription::Structure));
	CHECK_CONDITION(diff.changes().size() == 2);
	Q_ASSERT(std::system("rm -r projects/TestDiff") == 0);
}

TEST(FilePersistencePlugin, intValueChange)
{
	Q_ASSERT(std::system("tar -xf TestDiff_IntValueChange.tar -C projects") == 0);
	GitRepository repo("projects/TestDiff");
	Diff diff = repo.diff("HEAD~1", "HEAD");
	auto change1 = diff.changes().find(QUuid("00000000-0000-0000-0000-000000000014")).value();
	CHECK_CONDITION(change1->hasFlags(ChangeDescription::Value));
	CHECK_CONDITION(diff.changes().size() == 1);
	Q_ASSERT(std::system("rm -r projects/TestDiff") == 0);
}

TEST(FilePersistencePlugin, listReorder)
{
	Q_ASSERT(std::system("tar -xf TestDiff_ListReorder.tar -C projects") == 0);
	GitRepository repo("projects/TestDiff");
	Diff diff = repo.diff("HEAD~1", "HEAD");
	auto change1 = diff.changes().find(QUuid("00000000-0000-0000-0000-000000000008")).value();
	auto change2 = diff.changes().find(QUuid("00000000-0000-0000-0000-000000000009")).value();
	auto change3 = diff.changes().find(QUuid("00000000-0000-0000-0000-000000000002")).value();
	CHECK_CONDITION(change1->hasFlags(ChangeDescription::Label));
	CHECK_CONDITION(change2->hasFlags(ChangeDescription::Label));
	CHECK_CONDITION(change3->hasFlags(ChangeDescription::Structure));
	CHECK_CONDITION(diff.changes().size() == 3);
	Q_ASSERT(std::system("rm -r projects/TestDiff") == 0);
}

TEST(FilePersistencePlugin, noChange)
{
	Q_ASSERT(std::system("tar -xf TestDiff_NoChange.tar -C projects") == 0);
	GitRepository repo("projects/TestDiff");
	Diff diff = repo.diff("HEAD~1", "HEAD");
	CHECK_CONDITION(diff.changes().size() == 0);
	Q_ASSERT(std::system("rm -r projects/TestDiff") == 0);
}

TEST(FilePersistencePlugin, parentChange)
{
	Q_ASSERT(std::system("tar -xf TestDiff_ParentChange.tar -C projects") == 0);
	GitRepository repo("projects/TestDiff");
	Diff diff = repo.diff("HEAD~1", "HEAD");
	auto change1 = diff.changes().find(QUuid("00000000-0000-0000-0000-000000000017")).value();
	auto change2 = diff.changes().find(QUuid("00000000-0000-0000-0000-000000000016")).value();
	auto change3 = diff.changes().find(QUuid("00000000-0000-0000-0000-000000000018")).value();
	CHECK_CONDITION(change1->type() == ChangeType::Move);
	CHECK_CONDITION(change2->hasFlags(ChangeDescription::Structure));
	CHECK_CONDITION(change3->hasFlags(ChangeDescription::Structure));
	CHECK_CONDITION(diff.changes().size() == 3);
	Q_ASSERT(std::system("rm -r projects/TestDiff") == 0);
}

TEST(FilePersistencePlugin, typeChange)
{
	Q_ASSERT(std::system("tar -xf TestDiff_TypeChange.tar -C projects") == 0);
	GitRepository repo("projects/TestDiff");
	Diff diff = repo.diff("HEAD~1", "HEAD");
	auto change1 = diff.changes().find(QUuid("00000000-0000-0000-0000-000000000015")).value();
	CHECK_CONDITION(change1->hasFlags(ChangeDescription::Type));
	CHECK_CONDITION(diff.changes().size() == 1);
	Q_ASSERT(std::system("rm -r projects/TestDiff") == 0);
}

/*
TEST(FilePersistencePlugin, noChangeDetection)
{
	Model::NodeIdType nodeID = QUuid::createUuid();
	Model::NodeIdType parentID = QUuid::createUuid();

	GenericTree tree;
	GenericPersistentUnit persitenceUnit = tree.newPersistentUnit("noChangeDetectionTest");

	GenericNode* nodeA = persitenceUnit.newNode();
	GenericNode* nodeB = persitenceUnit.newNode();
	GenericNode* parent = persitenceUnit.newNode();

	nodeA->setName("value");
	nodeA->setType("Integer");
	nodeA->setValue(GenericNode::INT_VALUE, "I_0");
	nodeA->setId(nodeID);
	nodeA->setParent(parent);

	nodeB->setName("value");
	nodeB->setType("Integer");
	nodeB->setValue(GenericNode::INT_VALUE, "I_0");
	nodeB->setId(nodeID);
	nodeB->setParent(parent);

	parent->setName("initialValue");
	parent->setType("IntegerLiteral");
	parent->setId(parentID);
	// set no value
	// set no parent

	IdToGenericNodeHash oldNodes;
	oldNodes.insert(nodeID, nodeA);

	IdToGenericNodeHash newNodes;
	newNodes.insert(nodeID, nodeB);

	Diff diff(oldNodes, newNodes);

	IdToChangeDescriptionHash changes = diff.changes();
	IdToChangeDescriptionHash::iterator iter;

	iter = changes.find(nodeID);
	ChangeDescription* nodeChange = iter.value();

	ChangeType changeType = nodeChange->type();
	CHECK_CONDITION(changeType == ChangeType::Stationary);

	ChangeDescription::UpdateFlags flags = nodeChange->flags();
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Children));

	// parent gets NOT included into diff
	CHECK_CONDITION(!changes.contains(parentID));
}

TEST(FilePersistencePlugin, orderUpdateDetection)
{
	Model::NodeIdType nodeID = QUuid::createUuid();
	Model::NodeIdType parentID = QUuid::createUuid();

	GenericTree tree;
	GenericPersistentUnit persitenceUnit = tree.newPersistentUnit("orderUpdateDetectionTest");

	GenericNode* nodeA = persitenceUnit.newNode();
	GenericNode* nodeB = persitenceUnit.newNode();
	GenericNode* parent = persitenceUnit.newNode();

	nodeA->setName("value");
	nodeA->setType("Integer");
	nodeA->setValue(GenericNode::INT_VALUE, "I_0");
	nodeA->setId(nodeID);
	nodeA->setParent(parent);

	nodeB->setName("lpKind");
	nodeB->setType("Integer");
	nodeB->setValue(GenericNode::INT_VALUE, "I_0");
	nodeB->setId(nodeID);
	nodeB->setParent(parent);

	parent->setName("initialValue");
	parent->setType("IntegerLiteral");
	parent->setId(parentID);
	// set no value
	// set no parent

	IdToGenericNodeHash oldNodes;
	oldNodes.insert(nodeID, nodeA);

	IdToGenericNodeHash newNodes;
	newNodes.insert(nodeID, nodeB);

	Diff diff(oldNodes, newNodes);

	IdToChangeDescriptionHash changes = diff.changes();
	IdToChangeDescriptionHash::iterator iter;

	iter = changes.find(nodeID);
	ChangeDescription* nodeChange = iter.value();

	ChangeType changeType = nodeChange->type();
	CHECK_CONDITION(changeType == ChangeType::Stationary);

	ChangeDescription::UpdateFlags flags = nodeChange->flags();
	CHECK_CONDITION(flags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Children));

	// parent gets included into diff
	CHECK_CONDITION(changes.contains(parentID));

	iter = changes.find(parentID);
	ChangeDescription* parentChange = iter.value();
	ChangeDescription::UpdateFlags parentFlags = parentChange->flags();
	CHECK_CONDITION(!parentFlags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(!parentFlags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(!parentFlags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(parentFlags.testFlag(ChangeDescription::Children));
}

TEST(FilePersistencePlugin, typeUpdateDetection)
{
	Model::NodeIdType nodeID = QUuid::createUuid();
	Model::NodeIdType parentID = QUuid::createUuid();

	GenericTree tree;
	GenericPersistentUnit persitenceUnit = tree.newPersistentUnit("typeUpdateDetectionTest");

	GenericNode* nodeA = persitenceUnit.newNode();
	GenericNode* nodeB = persitenceUnit.newNode();
	GenericNode* parent = persitenceUnit.newNode();

	nodeA->setName("value");
	nodeA->setType("Integer");
	nodeA->setValue(GenericNode::INT_VALUE, "I_0");
	nodeA->setId(nodeID);
	nodeA->setParent(parent);

	nodeB->setName("value");
	nodeB->setType("LongInteger");
	nodeB->setValue(GenericNode::INT_VALUE, "I_0");
	nodeB->setId(nodeID);
	nodeB->setParent(parent);

	parent->setName("initialValue");
	parent->setType("IntegerLiteral");
	parent->setId(parentID);
	// set no value
	// set no parent

	IdToGenericNodeHash oldNodes;
	oldNodes.insert(nodeID, nodeA);

	IdToGenericNodeHash newNodes;
	newNodes.insert(nodeID, nodeB);

	Diff diff(oldNodes, newNodes);

	IdToChangeDescriptionHash changes = diff.changes();
	IdToChangeDescriptionHash::iterator iter;

	iter = changes.find(nodeID);
	ChangeDescription* nodeChange = iter.value();

	ChangeType changeType = nodeChange->type();
	CHECK_CONDITION(changeType == ChangeType::Stationary);

	ChangeDescription::UpdateFlags flags = nodeChange->flags();
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(flags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Children));

	// parent gets NOT included into diff
	CHECK_CONDITION(!changes.contains(parentID));
}

TEST(FilePersistencePlugin, valueUpdateDetection)
{
	Model::NodeIdType nodeID = QUuid::createUuid();
	Model::NodeIdType parentID = QUuid::createUuid();

	GenericTree tree;
	GenericPersistentUnit persitenceUnit = tree.newPersistentUnit("valueUpdateDetectionTest");

	GenericNode* nodeA = persitenceUnit.newNode();
	GenericNode* nodeB = persitenceUnit.newNode();
	GenericNode* parent = persitenceUnit.newNode();

	nodeA->setName("value");
	nodeA->setType("Integer");
	nodeA->setValue(GenericNode::INT_VALUE, "I_0");
	nodeA->setId(nodeID);
	nodeA->setParent(parent);

	nodeB->setName("value");
	nodeB->setType("Integer");
	nodeB->setValue(GenericNode::INT_VALUE, "I_1");
	nodeB->setId(nodeID);
	nodeB->setParent(parent);

	parent->setName("initialValue");
	parent->setType("IntegerLiteral");
	parent->setId(parentID);
	// set no value
	// set no parent

	IdToGenericNodeHash oldNodes;
	oldNodes.insert(nodeID, nodeA);

	IdToGenericNodeHash newNodes;
	newNodes.insert(nodeID, nodeB);

	Diff diff(oldNodes, newNodes);

	IdToChangeDescriptionHash changes = diff.changes();
	IdToChangeDescriptionHash::iterator iter;

	iter = changes.find(nodeID);
	ChangeDescription* nodeChange = iter.value();

	ChangeType changeType = nodeChange->type();
	CHECK_CONDITION(changeType == ChangeType::Stationary);

	ChangeDescription::UpdateFlags flags = nodeChange->flags();
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(flags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Children));

	// parent gets NOT included into diff
	CHECK_CONDITION(!changes.contains(parentID));
}

TEST(FilePersistencePlugin, insertDetection)
{
	Model::NodeIdType nodeID = QUuid::createUuid();
	Model::NodeIdType parentID = QUuid::createUuid();

	GenericTree tree;
	GenericPersistentUnit persitenceUnit = tree.newPersistentUnit("insertDetectionTest");

	GenericNode* node = persitenceUnit.newNode();
	GenericNode* parent = persitenceUnit.newNode();

	node->setName("value");
	node->setType("Integer");
	node->setValue(GenericNode::INT_VALUE, "I_0");
	node->setId(nodeID);
	node->setParent(parent);

	parent->setName("initialValue");
	parent->setType("IntegerLiteral");
	parent->setId(parentID);
	// set no value
	// set no parent

	IdToGenericNodeHash oldNodes;

	IdToGenericNodeHash newNodes;
	newNodes.insert(nodeID, node);

	Diff diff(oldNodes, newNodes);

	IdToChangeDescriptionHash changes = diff.changes();
	IdToChangeDescriptionHash::iterator iter;

	iter = changes.find(nodeID);
	ChangeDescription* nodeChange = iter.value();

	ChangeType changeType = nodeChange->type();
	CHECK_CONDITION(changeType == ChangeType::Insertion);

	ChangeDescription::UpdateFlags flags = nodeChange->flags();
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Children));

	// parent gets included into diff
	CHECK_CONDITION(changes.contains(parentID));

	iter = changes.find(parentID);
	ChangeDescription* parentChange = iter.value();
	ChangeDescription::UpdateFlags parentFlags = parentChange->flags();
	CHECK_CONDITION(!parentFlags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(!parentFlags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(!parentFlags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(parentFlags.testFlag(ChangeDescription::Children));
}

TEST(FilePersistencePlugin, deleteDetection)
{
	Model::NodeIdType nodeID = QUuid::createUuid();
	Model::NodeIdType parentID = QUuid::createUuid();

	GenericTree tree;
	GenericPersistentUnit persitenceUnit = tree.newPersistentUnit("deleteDetectionTest");

	GenericNode* node = persitenceUnit.newNode();
	GenericNode* parent = persitenceUnit.newNode();

	node->setName("value");
	node->setType("Integer");
	node->setValue(GenericNode::INT_VALUE, "I_0");
	node->setId(nodeID);
	node->setParent(parent);

	parent->setName("initialValue");
	parent->setType("IntegerLiteral");
	parent->setId(parentID);
	// set no value
	// set no parent

	IdToGenericNodeHash oldNodes;
	oldNodes.insert(nodeID, node);

	IdToGenericNodeHash newNodes;

	Diff diff(oldNodes, newNodes);

	IdToChangeDescriptionHash changes = diff.changes();
	IdToChangeDescriptionHash::iterator iter;

	iter = changes.find(nodeID);
	ChangeDescription* nodeChange = iter.value();

	ChangeType changeType = nodeChange->type();
	CHECK_CONDITION(changeType == ChangeType::Deletion);

	ChangeDescription::UpdateFlags flags = nodeChange->flags();
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Children));

	// parent gets included into diff
	CHECK_CONDITION(changes.contains(parentID));

	iter = changes.find(parentID);
	ChangeDescription* parentChange = iter.value();
	ChangeDescription::UpdateFlags parentFlags = parentChange->flags();
	CHECK_CONDITION(!parentFlags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(!parentFlags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(!parentFlags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(parentFlags.testFlag(ChangeDescription::Children));
}

TEST(FilePersistencePlugin, moveDetection)
{
	Model::NodeIdType nodeID = QUuid::createUuid();
	Model::NodeIdType parentAID = QUuid::createUuid();
	Model::NodeIdType parentBID = QUuid::createUuid();

	GenericTree tree;
	GenericPersistentUnit persitenceUnit = tree.newPersistentUnit("moveDetectionTest");

	GenericNode* nodeA = persitenceUnit.newNode();
	GenericNode* nodeB = persitenceUnit.newNode();
	GenericNode* parentA = persitenceUnit.newNode();
	GenericNode* parentB = persitenceUnit.newNode();

	nodeA->setName("value");
	nodeA->setType("Integer");
	nodeA->setValue(GenericNode::INT_VALUE, "I_0");
	nodeA->setId(nodeID);
	nodeA->setParent(parentA);

	nodeB->setName("value");
	nodeB->setType("Integer");
	nodeB->setValue(GenericNode::INT_VALUE, "I_0");
	nodeB->setId(nodeID);
	nodeB->setParent(parentB);

	parentA->setName("initialValue");
	parentA->setType("IntegerLiteral");
	parentA->setId(parentAID);
	// set no value
	// set no parent

	parentB->setName("initialValue");
	parentB->setType("IntegerLiteral");
	parentB->setId(parentBID);
	// set no value
	// set no parent

	IdToGenericNodeHash oldNodes;
	oldNodes.insert(nodeID, nodeA);

	IdToGenericNodeHash newNodes;
	newNodes.insert(nodeID, nodeB);

	Diff diff(oldNodes, newNodes);

	IdToChangeDescriptionHash changes = diff.changes();
	IdToChangeDescriptionHash::iterator iter;

	iter = changes.find(nodeID);
	ChangeDescription* nodeChange = iter.value();

	ChangeType changeType = nodeChange->type();
	CHECK_CONDITION(changeType == ChangeType::Move);

	ChangeDescription::UpdateFlags flags = nodeChange->flags();
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Children));

	// BOTH parents gets included into diff
	CHECK_CONDITION(changes.contains(parentAID));

	iter = changes.find(parentAID);
	ChangeDescription* parentAChange = iter.value();
	ChangeDescription::UpdateFlags parentAFlags = parentAChange->flags();
	CHECK_CONDITION(!parentAFlags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(!parentAFlags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(!parentAFlags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(parentAFlags.testFlag(ChangeDescription::Children));


	CHECK_CONDITION(changes.contains(parentBID));

	iter = changes.find(parentBID);
	ChangeDescription* parentBChange = iter.value();
	ChangeDescription::UpdateFlags parentBFlags = parentBChange->flags();
	CHECK_CONDITION(!parentBFlags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(!parentBFlags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(!parentBFlags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(parentBFlags.testFlag(ChangeDescription::Children));
}

TEST(FilePersistencePlugin, combinationDetection)
{
	Model::NodeIdType nodeID = QUuid::createUuid();
	Model::NodeIdType parentID = QUuid::createUuid();

	GenericTree tree;
	GenericPersistentUnit persitenceUnit = tree.newPersistentUnit("moveDetectionTest");

	GenericNode* nodeA = persitenceUnit.newNode();
	GenericNode* nodeB = persitenceUnit.newNode();
	GenericNode* parentA = persitenceUnit.newNode();
	GenericNode* parentB = persitenceUnit.newNode();

	nodeA->setName("value");
	nodeA->setType("Integer");
	nodeA->setValue(GenericNode::INT_VALUE, "I_0");
	nodeA->setId(nodeID);
	nodeA->setParent(parentA);

	nodeB->setName("lpKind");
	nodeB->setType("LongInteger");
	nodeB->setValue(GenericNode::INT_VALUE, "I_1");
	nodeB->setId(nodeID);
	nodeB->setParent(parentB);

	parentA->setName("initialValue");
	parentA->setType("IntegerLiteral");
	parentA->setId(parentID);
	// set no value
	// set no parent

	parentB->setName("initialValue");
	parentB->setType("LongIntegerLiteral");
	parentB->setId(parentID);
	// set no value
	// set no parent

	IdToGenericNodeHash oldNodes;
	oldNodes.insert(nodeID, nodeA);
	oldNodes.insert(parentID, parentA);

	IdToGenericNodeHash newNodes;
	newNodes.insert(nodeID, nodeB);
	newNodes.insert(parentID, parentB);

	Diff diff(oldNodes, newNodes);

	IdToChangeDescriptionHash changes = diff.changes();
	IdToChangeDescriptionHash::iterator iter;

	iter = changes.find(nodeID);
	ChangeDescription* nodeChange = iter.value();

	ChangeType changeType = nodeChange->type();
	CHECK_CONDITION(changeType == ChangeType::Stationary);

	ChangeDescription::UpdateFlags flags = nodeChange->flags();
	CHECK_CONDITION(flags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(flags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(flags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(!flags.testFlag(ChangeDescription::Children));

	// parent gets included into diff
	CHECK_CONDITION(changes.contains(parentID));

	iter = changes.find(parentID);
	ChangeDescription* parentChange = iter.value();
	ChangeDescription::UpdateFlags parentFlags = parentChange->flags();
	CHECK_CONDITION(!parentFlags.testFlag(ChangeDescription::Order));
	CHECK_CONDITION(parentFlags.testFlag(ChangeDescription::Type));
	CHECK_CONDITION(!parentFlags.testFlag(ChangeDescription::Value));
	CHECK_CONDITION(parentFlags.testFlag(ChangeDescription::Children));
}

TEST(FilePersistencePlugin, compareUpdateFlags)
{
	ChangeDescription::UpdateFlags flags1;
	ChangeDescription::UpdateFlags flags2;
	ChangeDescription::UpdateFlags flags3;

	flags1 |= ChangeDescription::Order;
	flags1 |= ChangeDescription::Value;

	flags2 |= ChangeDescription::Order;
	flags2 |= ChangeDescription::Value;

	flags3 |= ChangeDescription::Order;
	flags3 |= ChangeDescription::Type;

	CHECK_CONDITION(ChangeDescription::compareUpdateFlags(flags1, flags2));
	CHECK_CONDITION(!ChangeDescription::compareUpdateFlags(flags1, flags3));
	CHECK_CONDITION(!ChangeDescription::compareUpdateFlags(flags2, flags3));
}
*/
} /* namespace FilePersistence */