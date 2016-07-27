/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "VCTestProject.h"
#include "../src/FilePersistencePlugin.h"
#include "../src/simple/GenericNode.h"
#include "../src/simple/GenericTree.h"
#include "../src/version_control/Commit.h"
#include "../src/version_control/GitRepository.h"
#include "../src/version_control/Merge.h"

#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"
#include "SelfTest/src/TestBase.h"
#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class FILEPERSISTENCE_API TwoDeletesNoConflict
 : public SelfTest::Test<FilePersistencePlugin, TwoDeletesNoConflict> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	auto tree = merge->mergedTree();
	merge->commit(sig, sig, "This is the result of merge test \"twodeletesNoConflict\"");
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000032}"}));
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000042}"}));
}};

class FILEPERSISTENCE_API TwoDeletesInSameListResolvable
 : public SelfTest::Test<FilePersistencePlugin, TwoDeletesInSameListResolvable> {
public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	auto tree = merge->mergedTree();
	merge->commit(sig, sig, "This is the result of merge test \"" + this->getName() + "\"");
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000013}"}));
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000042}"}));
	// check that the list is continuous.
	auto listContainer = tree->find(QUuid{"{00000000-0000-0000-0000-000000000002}"});
	for (int idx = 0; idx < listContainer->children().size(); ++idx)
		CHECK_CONDITION(listContainer->child(QString::number(idx)));
}};

class FILEPERSISTENCE_API UnorderedAndUnitsConflicting
 : public SelfTest::Test<FilePersistencePlugin, UnorderedAndUnitsConflicting> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	CHECK_CONDITION(merge->hasConflicts());
	QSet<Model::NodeIdType> expected {
		{"{00000000-0000-0000-0000-000000000047}"},
		{"{00000000-0000-0000-0000-000000000048}"},
		{"{00000000-0000-0000-0000-000000000031}"},
		{"{00000000-0000-0000-0000-000000000030}"},
		{"{00000000-0000-0000-0000-000000000051}"},
		{"{00000000-0000-0000-0000-000000000052}"}
	};
	for (auto mustConflict : expected)
		CHECK_CONDITION(merge->isNodeInConflict(mustConflict));
}};

class FILEPERSISTENCE_API ListsReorderInsertDeleteResolvable
 : public SelfTest::Test<FilePersistencePlugin, ListsReorderInsertDeleteResolvable> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	CHECK_CONDITION(!merge->hasConflicts());
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	merge->commit(sig, sig, "This is the result of merge test \"" + this->getName() + "\"");

	auto tree = merge->mergedTree();
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000021404}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000212}"}));
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000506}"}));
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000001507}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000011507}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000211}"})
						 ->parent()->id().toString().endsWith("300}"));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000011507}"})
						 ->parent()->id().toString().endsWith("507}"));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000021404}"})
						 ->parent()->id().toString().endsWith("01404}"));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000212}"})
						 ->parent()->id().toString().endsWith("200}"));
	// Label checks are labels before Removing holes from the list
	CHECK_STR_EQUAL("0", tree->find(QUuid{"{00000000-0000-0000-0000-000000000299}"})->label());
	CHECK_STR_EQUAL("1", tree->find(QUuid{"{00000000-0000-0000-0000-000000000207}"})->label());
	CHECK_STR_EQUAL("2", tree->find(QUuid{"{00000000-0000-0000-0000-000000000201}"})->label());
	CHECK_STR_EQUAL("3", tree->find(QUuid{"{00000000-0000-0000-0001-000000000201}"})->label());
	CHECK_STR_EQUAL("4", tree->find(QUuid{"{00000000-0000-0000-0002-000000000201}"})->label());
	CHECK_STR_EQUAL("5", tree->find(QUuid{"{00000000-0000-0000-0000-000000000212}"})->label());
	CHECK_STR_EQUAL("6", tree->find(QUuid{"{00000000-0000-0000-0000-000000000203}"})->label());
	CHECK_STR_EQUAL("7", tree->find(QUuid{"{00000000-0000-0000-0000-000000000202}"})->label());
	CHECK_STR_EQUAL("8", tree->find(QUuid{"{00000000-0000-0000-0001-000000000202}"})->label());
	CHECK_STR_EQUAL("9", tree->find(QUuid{"{00000000-0000-0000-0002-000000000202}"})->label());
	CHECK_STR_EQUAL("10", tree->find(QUuid{"{00000000-0000-0000-0000-000000000205}"})->label());
	CHECK_STR_EQUAL("11", tree->find(QUuid{"{00000000-0000-0000-0000-000000000206}"})->label());
	CHECK_STR_EQUAL("12", tree->find(QUuid{"{00000000-0000-0000-0000-000000000204}"})->label());
	CHECK_STR_EQUAL("13", tree->find(QUuid{"{00000000-0000-0000-0000-000000000208}"})->label());
	CHECK_STR_EQUAL("14", tree->find(QUuid{"{00000000-0000-0000-0000-000000000209}"})->label());
	CHECK_STR_EQUAL("15", tree->find(QUuid{"{00000000-0000-0000-0000-000000000210}"})->label());

}};

class FILEPERSISTENCE_API EvalClassMove
 : public SelfTest::Test<FilePersistencePlugin, EvalClassMove> { public: void test()
{
	VCTestProject p{"Eval_ClassMove", "TestMerge"};
	auto merge = p.repo().merge("dev");
	CHECK_CONDITION(!merge->hasConflicts());
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	merge->commit(sig, sig, "Merged master and dev");
}};

class FILEPERSISTENCE_API EvalLoopFix
 : public SelfTest::Test<FilePersistencePlugin, EvalLoopFix> { public: void test()
{
	VCTestProject p{"Eval_LoopFix", "TestMerge"};
	auto merge = p.repo().merge("dev");
//	CHECK_CONDITION(merge->hasConflicts()); // This was used for the old merge algorithm
	CHECK_INT_EQUAL(1, merge->softConflicts().size());
}};

class FILEPERSISTENCE_API EvalMethodInsert
 : public SelfTest::Test<FilePersistencePlugin, EvalMethodInsert> { public: void test()
{
	VCTestProject p{"Eval_MethodInsert", "TestMerge"};
	auto merge = p.repo().merge("dev");
	CHECK_CONDITION(!merge->hasConflicts());
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	merge->commit(sig, sig, "Merged master and dev");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(tree->find(QUuid{"{cbafa8f9-7db9-476f-a8d9-07b44e7ab632}"}));
	CHECK_CONDITION(tree->find(QUuid{"{1f702787-0160-4205-9a1f-eaf653679698}"}));
	CHECK_CONDITION(tree->find(QUuid{"{cbafa8f9-7db9-476f-a8d9-07b44e7ab632}"})->label().toInt() == 2 ||
						 tree->find(QUuid{"{cbafa8f9-7db9-476f-a8d9-07b44e7ab632}"})->label().toInt() == 1);
	CHECK_CONDITION(tree->find(QUuid{"{1f702787-0160-4205-9a1f-eaf653679698}"})->label().toInt() == 1 ||
						 tree->find(QUuid{"{1f702787-0160-4205-9a1f-eaf653679698}"})->label().toInt() == 1);

}};

class FILEPERSISTENCE_API TwoChangesDifferentLists
 : public SelfTest::Test<FilePersistencePlugin, TwoChangesDifferentLists> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"TwoChangesSameList\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(!merge->hasConflicts());
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000210}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000408}"}));
}};

class FILEPERSISTENCE_API SingleVersionChange
 : public SelfTest::Test<FilePersistencePlugin, SingleVersionChange> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	// Since it is already merged (due to Fast Forward merge)
	Q_ASSERT(merge->isAlreadyMerged());
}};

class FILEPERSISTENCE_API TwoChangesSameList
 : public SelfTest::Test<FilePersistencePlugin, TwoChangesSameList> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"TwoChangesSameList\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(!merge->hasConflicts());
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000204}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-0000000002ab}"}));
}};

class FILEPERSISTENCE_API TwoChangesDifferentLists_LabelCycle
 : public SelfTest::Test<FilePersistencePlugin, TwoChangesDifferentLists_LabelCycle> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"TwoChangesSameList\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(!merge->hasConflicts());
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000203}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000406}"}));
}};

class FILEPERSISTENCE_API TwoChangesSameList_LabelCycleResolvable
 : public SelfTest::Test<FilePersistencePlugin, TwoChangesSameList_LabelCycleResolvable> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"TwoChangesSameList_LabelCycleResolvable\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(!merge->hasConflicts());
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000203}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000206}"}));
}};

class FILEPERSISTENCE_API TwoChangesSameList_DelInsNodes
 : public SelfTest::Test<FilePersistencePlugin, TwoChangesSameList_DelInsNodes> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"TwoChangesSameList_DelInsNodes\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(!merge->hasConflicts());
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-00000000020b}"}));
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-00000000020c}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-00000000020f}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-00000000020e}"}));
	CHECK_STR_EQUAL("1", tree->find(QUuid{"{00000000-0000-0000-0000-00000000020a}"})->label());
	CHECK_STR_EQUAL("2", tree->find(QUuid{"{00000000-0000-0000-0000-00000000020f}"})->label());
	CHECK_STR_EQUAL("3", tree->find(QUuid{"{00000000-0000-0000-0000-00000000020e}"})->label());
	CHECK_STR_EQUAL("4", tree->find(QUuid{"{00000000-0000-0000-0000-00000000020d}"})->label());
}};

class FILEPERSISTENCE_API BothChangeSameList_Conflicts
 : public SelfTest::Test<FilePersistencePlugin, BothChangeSameList_Conflicts> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"BothChangeSameList_Conflicts\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(merge->hasConflicts());	//	(Lbl change VS Deletion) of 205
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000201}"}));
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000209}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000205}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-00000000020a}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-00000000020b}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-00000000020c}"}));
	CHECK_STR_EQUAL("1", tree->find(QUuid{"{00000000-0000-0000-0000-000000000202}"})->label());
	CHECK_STR_EQUAL("2", tree->find(QUuid{"{00000000-0000-0000-0000-00000000020c}"})->label());
	CHECK_STR_EQUAL("3", tree->find(QUuid{"{00000000-0000-0000-0000-000000000203}"})->label());
	CHECK_STR_EQUAL("4", tree->find(QUuid{"{00000000-0000-0000-0000-000000000204}"})->label());
	CHECK_STR_EQUAL("5", tree->find(QUuid{"{00000000-0000-0000-0000-000000000205}"})->label());
	CHECK_STR_EQUAL("6", tree->find(QUuid{"{00000000-0000-0000-0000-000000000206}"})->label());
	CHECK_STR_EQUAL("7", tree->find(QUuid{"{00000000-0000-0000-0000-000000000207}"})->label());
	CHECK_STR_EQUAL("8", tree->find(QUuid{"{00000000-0000-0000-0000-00000000020b}"})->label());
	CHECK_STR_EQUAL("9", tree->find(QUuid{"{00000000-0000-0000-0000-00000000020a}"})->label());
	CHECK_STR_EQUAL("10", tree->find(QUuid{"{00000000-0000-0000-0000-000000000208}"})->label());

	// Soft conflicts
	CHECK_INT_EQUAL(1, merge->softConflicts().size());
	CHECK_CONDITION(merge->softConflicts().last().nodesInConflict().contains(
							 QUuid{"{00000000-0000-0000-0000-00000000020a}"}));
	CHECK_CONDITION(merge->softConflicts().last().nodesInConflict().contains(
							 QUuid{"{00000000-0000-0000-0000-00000000020b}"}));
}};

class FILEPERSISTENCE_API MoveChangeBySingleVersion
 : public SelfTest::Test<FilePersistencePlugin, MoveChangeBySingleVersion> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"MoveChangeBySingleVersion\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(!merge->hasConflicts());
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000001403}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000021404}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000001403}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000599"});
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000021404}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000001404"});
}};

class FILEPERSISTENCE_API MoveChangeBetweenListsResolvable
 : public SelfTest::Test<FilePersistencePlugin, MoveChangeBetweenListsResolvable> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"MoveChangeBetweenListsResolvable\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(!merge->hasConflicts());
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000404}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000404}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000500"});
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-00000000050a}"}));
}};

class FILEPERSISTENCE_API MoveVsInsertConflict
 : public SelfTest::Test<FilePersistencePlugin, MoveVsInsertConflict> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"MoveVsInsertConflict\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(merge->hasConflicts());
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000001501}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000404}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000404}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000400"});
}};

class FILEPERSISTENCE_API AtomicCycleMoveMove
 : public SelfTest::Test<FilePersistencePlugin, AtomicCycleMoveMove> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"AtomicCycleMoveMove\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(!merge->hasConflicts());
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000001507}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000011507}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000011507}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000507"});
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000001507}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000011507"});
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000021404}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000021404}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000001404"});
}};

class FILEPERSISTENCE_API AtomicCycleMoveDel
 : public SelfTest::Test<FilePersistencePlugin, AtomicCycleMoveDel> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"AtomicCycleMoveDel\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(!merge->hasConflicts());
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000001507}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000011507}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000011507}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000507"});
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000021404}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000021404}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000001404"});
}};

class FILEPERSISTENCE_API AtomicCycleMoveIns
 : public SelfTest::Test<FilePersistencePlugin, AtomicCycleMoveIns> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"AtomicCycleMoveIns\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(!merge->hasConflicts());
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000001507}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000011507}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000011507}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000507"});
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000001507}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000011507"});
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000021404}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000021404}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000001404"});
}};

class FILEPERSISTENCE_API MoveCycleSimple
 : public SelfTest::Test<FilePersistencePlugin, MoveCycleSimple> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"MoveCycleSimple\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(merge->hasConflicts());
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000404}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000599}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000599}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000500"});
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000404}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000400"});
}};

class FILEPERSISTENCE_API MoveAndListChangesResolvable
 : public SelfTest::Test<FilePersistencePlugin, MoveAndListChangesResolvable> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"MoveAndListChangesResolvable\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(merge->hasConflicts());
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000403}"}));
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000406}"}));
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000502}"}));
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000505}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000001507}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-00000000050a}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000404}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000599}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000599}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000500"});
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000404}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000400"});
}};

class FILEPERSISTENCE_API MoveInAndListChangesResolvable
 : public SelfTest::Test<FilePersistencePlugin, MoveInAndListChangesResolvable> { public: void test()
{
	VCTestProject p{"TestMerge_"+this->getName(), "TestMerge"};
	auto merge = p.repo().merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	Q_ASSERT(!merge->isAlreadyMerged());
	merge->commit(sig, sig, "This is the result of merge test \"MoveInAndListChangesResolvable\"");
	auto tree = merge->mergedTree();
	CHECK_CONDITION(!merge->hasConflicts());
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000001404}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000001507}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000011507}"}));
	CHECK_CONDITION(!tree->find(QUuid{"{00000000-0000-0000-0000-000000000501}"}));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000001507}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000407"});
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000011507}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000507"});
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000001404}"})->parentId() ==
						 QUuid{"00000000-0000-0000-0000-000000000500"});
}};
/**
 * The RunMerge test is not an actual test but rather is used to run the merge algorithm on the repo
 * found in /tmp/EnvisionVC/TestMerge.
 * This test should probably eventually be replaced by a command line interface.
 */
class FILEPERSISTENCE_API RunMerge
 : public SelfTest::Test<FilePersistencePlugin, RunMerge> { public: void test()
{
	if (!QFile{"/tmp/EnvisionVC/TestMerge/.git"}.exists())
	{
		CHECK_CONDITION(true);
		return;
	}
	GitRepository repo{"/tmp/EnvisionVC/TestMerge"};
	auto merge = repo.merge("dev");
	if (!merge->isAlreadyMerged() && !merge->hasConflicts())
	{
		Signature sig;
		sig.name_ = "Chuck TESTa";
		sig.eMail_ = "chuck@mergetest.com";
		merge->commit(sig, sig, "This is the result of merge test \"WorkflowTest\"");
	}
	CHECK_CONDITION(true);
	exit(0);
}};

}
