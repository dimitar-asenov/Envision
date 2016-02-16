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

#include "../src/FilePersistencePlugin.h"
#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"

#include "../src/simple/GenericTree.h"

#include "../src/version_control/ChangeDescription.h"
#include "../src/version_control/Diff.h"
#include "../src/version_control/GitRepository.h"
#include "VCTestProject.h"


namespace FilePersistence {

class FILEPERSISTENCE_API addAndRename
 : public SelfTest::Test<FilePersistencePlugin, addAndRename> { public: void test()
{
	VCTestProject p{"TestProject2"};
	Diff diff = p.repo().diff("HEAD~1", "HEAD");
	auto renameChange = diff.changes().find(QUuid{"ec98da71-76d5-4371-a491-18826bce2a35"}).value();
	auto insertChange = diff.changes().find(QUuid{"b32af2db-f0d0-4d36-ba00-96f27595248e"}).value();
	auto structChange = diff.changes().find(QUuid{"ee53ebac-d6a9-420f-b729-68ca160ee94e"}).value();
	CHECK_CONDITION(renameChange->hasFlags(ChangeDescription::Value));
	CHECK_CONDITION(insertChange->type() == ChangeType::Insertion);
	CHECK_CONDITION(structChange->hasFlags(ChangeDescription::Structure));
}};

class FILEPERSISTENCE_API stmtEdit : public SelfTest::Test<FilePersistencePlugin, stmtEdit> { public: void test()
{
	VCTestProject p{"TestProject1"};
	Diff diff = p.repo().diff("HEAD~1", "HEAD");
	/*
	for (auto change : diff.changes())
	{
		change->print();
	}
	*/
	CHECK_CONDITION(diff.changes().size() > 0);
}};

class FILEPERSISTENCE_API deletion : public SelfTest::Test<FilePersistencePlugin, deletion> { public: void test()
{
	VCTestProject p{"TestDiff_Delete", "TestDiff"};
	Diff diff = p.repo().diff("HEAD~1", "HEAD");
	auto change1 = diff.changes().find(QUuid{"00000000-0000-0000-0000-000000000017"}).value();
	auto change2 = diff.changes().find(QUuid{"00000000-0000-0000-0000-000000000016"}).value();
	CHECK_CONDITION(change1->type() == ChangeType::Deletion);
	CHECK_CONDITION(change2->hasFlags(ChangeDescription::Structure));
	CHECK_CONDITION(diff.changes().size() == 2);
}};

class FILEPERSISTENCE_API insert : public SelfTest::Test<FilePersistencePlugin, insert> { public: void test()
{
	VCTestProject p{"TestDiff_Insert", "TestDiff"};
	Diff diff = p.repo().diff("HEAD~1", "HEAD");
	auto change1 = diff.changes().find(QUuid{"00000000-0000-0000-0000-000000000020"}).value();
	auto change2 = diff.changes().find(QUuid{"00000000-0000-0000-0000-000000000001"}).value();
	CHECK_CONDITION(change1->type() == ChangeType::Insertion);
	CHECK_CONDITION(change2->hasFlags(ChangeDescription::Structure));
	CHECK_CONDITION(diff.changes().size() == 2);
}};

class FILEPERSISTENCE_API intValueChange
 : public SelfTest::Test<FilePersistencePlugin, intValueChange> { public: void test()
{
	VCTestProject p{"TestDiff_IntValueChange", "TestDiff"};
	Diff diff = p.repo().diff("HEAD~1", "HEAD");
	auto change1 = diff.changes().find(QUuid{"00000000-0000-0000-0000-000000000014"}).value();
	CHECK_CONDITION(change1->hasFlags(ChangeDescription::Value));
	CHECK_CONDITION(diff.changes().size() == 1);
}};

class FILEPERSISTENCE_API listReorder : public SelfTest::Test<FilePersistencePlugin, listReorder> { public: void test()
{
	VCTestProject p{"TestDiff_ListReorder", "TestDiff"};
	Diff diff = p.repo().diff("HEAD~1", "HEAD");
	auto change1 = diff.changes().find(QUuid{"00000000-0000-0000-0000-000000000008"}).value();
	auto change2 = diff.changes().find(QUuid{"00000000-0000-0000-0000-000000000009"}).value();
	auto change3 = diff.changes().find(QUuid{"00000000-0000-0000-0000-000000000002"}).value();
	CHECK_CONDITION(change1->hasFlags(ChangeDescription::Label));
	CHECK_CONDITION(change2->hasFlags(ChangeDescription::Label));
	CHECK_CONDITION(change3->hasFlags(ChangeDescription::Structure));
	CHECK_CONDITION(diff.changes().size() == 3);
}};

class FILEPERSISTENCE_API noChange : public SelfTest::Test<FilePersistencePlugin, noChange> { public: void test()
{
	VCTestProject p{"TestDiff_NoChange", "TestDiff"};
	Diff diff = p.repo().diff("HEAD~1", "HEAD");
	CHECK_CONDITION(diff.changes().size() == 0);
}};

class FILEPERSISTENCE_API parentChange
 : public SelfTest::Test<FilePersistencePlugin, parentChange> { public: void test()
{
	VCTestProject p{"TestDiff_ParentChange", "TestDiff"};
	Diff diff = p.repo().diff("HEAD~1", "HEAD");
	auto change1 = diff.changes().find(QUuid{"00000000-0000-0000-0000-000000000017"}).value();
	auto change2 = diff.changes().find(QUuid{"00000000-0000-0000-0000-000000000016"}).value();
	auto change3 = diff.changes().find(QUuid{"00000000-0000-0000-0000-000000000018"}).value();
	CHECK_CONDITION(change1->type() == ChangeType::Move);
	CHECK_CONDITION(change2->hasFlags(ChangeDescription::Structure));
	CHECK_CONDITION(change3->hasFlags(ChangeDescription::Structure));
	CHECK_CONDITION(diff.changes().size() == 3);
}};

class FILEPERSISTENCE_API typeChange : public SelfTest::Test<FilePersistencePlugin, typeChange> { public: void test()
{
	VCTestProject p{"TestDiff_TypeChange", "TestDiff"};
	Diff diff = p.repo().diff("HEAD~1", "HEAD");
	auto change1 = diff.changes().find(QUuid{"00000000-0000-0000-0000-000000000015"}).value();
	CHECK_CONDITION(change1->hasFlags(ChangeDescription::Type));
	CHECK_CONDITION(diff.changes().size() == 1);
}};

}
