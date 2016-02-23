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

#include "../src/FilePersistencePlugin.h"
#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"

#include "../src/version_control/Merge.h"
#include "../src/version_control/GitRepository.h"
#include "VCTestProject.h"


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
	for (auto change : merge->getConflicts())
	{
		CHECK_CONDITION(expected.contains(change->nodeId()));
		expected.remove(change->nodeId());
	}
	CHECK_CONDITION(expected.isEmpty());
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

	auto listContainer = tree->find(QUuid{"{00000000-0000-0000-0000-000000000200}"});
	for (int idx = 0; idx < listContainer->children().size(); ++idx)
		CHECK_CONDITION(listContainer->child(QString::number(idx)));

	CHECK_CONDITION(listContainer->child(QString::number(1))->id().toString().endsWith("207}"));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000203}"})->label().toInt() == 6);
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000204}"})->label().toInt() == 12);
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000205}"})->label().toInt() == 10);
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000206}"})->label().toInt() == 11);
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000207}"})->label().toInt() == 1);
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000000211}"})
						 ->parent()->id().toString().endsWith("300}"));
	CHECK_CONDITION(tree->find(QUuid{"{00000000-0000-0000-0000-000000011507}"})
						 ->parent()->id().toString().endsWith("507}"));
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
	CHECK_CONDITION(merge->hasConflicts());
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
	if (!merge->hasConflicts())
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
