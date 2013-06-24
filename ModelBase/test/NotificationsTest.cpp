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

#include "modelbase.h"
#include "SelfTest/src/SelfTestSuite.h"
#include "test_nodes/BinaryNode.h"
#include "model/Model.h"
#include "NotificationListener.h"

namespace Model {

TEST(ModelBase, ModificationNotificationTests)
{
	Model model;
	NotificationListener nl(model);

	CHECK_CONDITION(nl.root == nullptr);
	CHECK_INT_EQUAL(0, nl.modifiedNodes.size());

	auto root = new TestNodes::BinaryNode();
	model.setRoot(root);

	CHECK_CONDITION(root == nl.root);

	model.beginModification(root, "make tree");
	TestNodes::BinaryNode* left = new TestNodes::BinaryNode();
	TestNodes::BinaryNode* right = new TestNodes::BinaryNode();
	root->setLeft(left);
	root->setRight(right);
	model.endModification();

	CHECK_INT_EQUAL(1, nl.modifiedNodes.size());
	CHECK_CONDITION(nl.modifiedNodes[0] == root);

	model.beginModification(left, "modify");
	left->name()->set("Left text");
	model.changeModificationTarget(right);
	right->name()->set("Right text");
	model.endModification();

	CHECK_INT_EQUAL(4, nl.modifiedNodes.size());
	CHECK_CONDITION(nl.modifiedNodes[0] == left);
	CHECK_CONDITION(nl.modifiedNodes[1] == left->name());
	CHECK_CONDITION(nl.modifiedNodes[2] == right);
	CHECK_CONDITION(nl.modifiedNodes[3] == right->name());

	nl.modifiedNodes.clear();
	model.beginModification(nullptr);
	model.undo();
	model.undo();
	model.endModification();

	CHECK_INT_EQUAL(5, nl.modifiedNodes.size());
	CHECK_CONDITION(nl.modifiedNodes[0] == right);
	CHECK_CONDITION(nl.modifiedNodes[1] == right->name());
	CHECK_CONDITION(nl.modifiedNodes[2] == left);
	CHECK_CONDITION(nl.modifiedNodes[3] == left->name());
	CHECK_CONDITION(nl.modifiedNodes[4] == root);
}

}
