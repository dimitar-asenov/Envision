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
#include "SelfTest/src/SelfTestSuite.h"

#include "ModelBase/src/persistence/PersistedNode.h"
#include "ModelBase/src/persistence/PersistedValue.h"

using namespace Model;

namespace FilePersistence {

TEST(FilePersistence, LoadDataMultipleUnits)
{
	QString testDir = ":/FilePersistence/test/persisted";
	FileStore store;
	store.setBaseFolder(testDir);

	typedef PersistedValue< QString >* String;
	typedef PersistedValue< QList<PersistedNode*> >* Composite;

	// Root Node
	Composite root = dynamic_cast<Composite> (store.loadCompleteNodeSubtree("units", nullptr));
	CHECK_CONDITION(root);
	CHECK_STR_EQUAL("BinaryNode", root->type() );
	CHECK_STR_EQUAL("units", root->name());
	CHECK_INT_EQUAL(0, root->id());
	CHECK_CONDITION(!root->partialHint());
	CHECK_CONDITION(!root->isNewPersistenceUnit());
	CHECK_INT_EQUAL(3, root->value().size());

	// Root Node children
	String rootName = dynamic_cast<String> (root->value().at(0));
	CHECK_CONDITION(rootName);
	CHECK_STR_EQUAL("Text", rootName->type() );
	CHECK_STR_EQUAL("name", rootName->name());
	CHECK_INT_EQUAL(1, rootName->id());
	CHECK_CONDITION(!rootName->partialHint());
	CHECK_CONDITION(!rootName->isNewPersistenceUnit());
	CHECK_STR_EQUAL("Root", rootName->value());

	Composite left = dynamic_cast<Composite> (root->value().at(1));
	CHECK_CONDITION(left);
	CHECK_STR_EQUAL("BinaryNodePersistenceUnit", left->type() );
	CHECK_STR_EQUAL("left", left->name());
	CHECK_INT_EQUAL(2, left->id());
	CHECK_CONDITION(!left->partialHint());
	CHECK_CONDITION(left->isNewPersistenceUnit());
	CHECK_INT_EQUAL(2, left->value().size());

	Composite right = dynamic_cast<Composite> (root->value().at(2));
	CHECK_CONDITION(right);
	CHECK_STR_EQUAL("BinaryNode", right->type() );
	CHECK_STR_EQUAL("right", right->name());
	CHECK_INT_EQUAL(6, right->id());
	CHECK_CONDITION(!right->partialHint());
	CHECK_CONDITION(!right->isNewPersistenceUnit());
	CHECK_INT_EQUAL(1, right->value().size());

	// Left Node children
	String leftName = dynamic_cast<String> (left->value().at(0));
	CHECK_CONDITION(leftName);
	CHECK_STR_EQUAL("Text", leftName->type() );
	CHECK_STR_EQUAL("name", leftName->name());
	CHECK_INT_EQUAL(3, leftName->id());
	CHECK_CONDITION(!leftName->partialHint());
	CHECK_CONDITION(!leftName->isNewPersistenceUnit());
	CHECK_STR_EQUAL("Left child", leftName->value());

	Composite leftleft = dynamic_cast<Composite> (left->value().at(1));
	CHECK_CONDITION(leftleft);
	CHECK_STR_EQUAL("BinaryNode", leftleft->type() );
	CHECK_STR_EQUAL("left", leftleft->name());
	CHECK_INT_EQUAL(4, leftleft->id());
	CHECK_CONDITION(!leftleft->partialHint());
	CHECK_CONDITION(!leftleft->isNewPersistenceUnit());
	CHECK_INT_EQUAL(1, leftleft->value().size());

	String leftleftName = dynamic_cast<String> (leftleft->value().at(0));
	CHECK_CONDITION(leftleftName);
	CHECK_STR_EQUAL("Text", leftleftName->type() );
	CHECK_STR_EQUAL("name", leftleftName->name());
	CHECK_INT_EQUAL(5, leftleftName->id());
	CHECK_CONDITION(!leftleftName->partialHint());
	CHECK_CONDITION(!leftleftName->isNewPersistenceUnit());
	CHECK_STR_EQUAL("in a new unit", leftleftName->value());

	// Right Node children
	String rightName = dynamic_cast<String> (right->value().at(0));
	CHECK_CONDITION(rightName);
	CHECK_STR_EQUAL("Text", rightName->type() );
	CHECK_STR_EQUAL("name", rightName->name());
	CHECK_INT_EQUAL(7, rightName->id());
	CHECK_CONDITION(!rightName->partialHint());
	CHECK_CONDITION(!rightName->isNewPersistenceUnit());
	CHECK_STR_EQUAL("Right child", rightName->value());
}

}
