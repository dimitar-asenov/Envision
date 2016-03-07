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

#include "NotificationListener.h"
#include "../src/ModelBasePlugin.h"
#include "../src/model/TreeManager.h"
#include "../src/test_nodes/BinaryNode.h"
#include "../src/test_nodes/PositionExtension.h"

#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"

namespace Model {

class MODELBASE_API ModificationNotificationTests
		: public SelfTest::Test<ModelBasePlugin, ModificationNotificationTests> {
public: void test()
{
	TreeManager manager;
	NotificationListener nl{manager};

	CHECK_CONDITION(nl.root_ == nullptr);
	CHECK_INT_EQUAL(0, nl.modifiedNodes.size());
	CHECK_INT_EQUAL(0, nl.removedNodes.size());

	auto root = new TestNodes::BinaryNode{};
	manager.setRoot(root);

	CHECK_CONDITION(root == nl.root_);

	manager.beginModification(root, "make tree");
	TestNodes::BinaryNode* left = new TestNodes::BinaryNode{};
	TestNodes::BinaryNode* right = new TestNodes::BinaryNode{};
	root->setLeft(left);
	root->setRight(right);
	manager.endModification();

	CHECK_INT_EQUAL(1, nl.modifiedNodes.size());
	CHECK_INT_EQUAL(0, nl.removedNodes.size());
	CHECK_CONDITION(nl.modifiedNodes.contains( root) );

	manager.beginModification(left, "modify");
	left->name()->set("Left text");
	manager.changeModificationTarget(right);
	right->name()->set("Right text");
	manager.endModification();

	CHECK_INT_EQUAL(0, nl.removedNodes.size());
	CHECK_INT_EQUAL(4, nl.modifiedNodes.size());
	CHECK_CONDITION(nl.modifiedNodes.contains( left ));
	CHECK_CONDITION(nl.modifiedNodes.contains( left->name()));
	CHECK_CONDITION(nl.modifiedNodes.contains( right));
	CHECK_CONDITION(nl.modifiedNodes.contains( right->name()));

	nl.modifiedNodes.clear();
	manager.beginModification(nullptr);
	manager.undo();
	manager.undo();
	manager.endModification();

	CHECK_INT_EQUAL(5, nl.modifiedNodes.size());
	CHECK_CONDITION(nl.modifiedNodes.contains( right));
	CHECK_CONDITION(nl.modifiedNodes.contains( right->name()));
	CHECK_CONDITION(nl.modifiedNodes.contains( left));
	CHECK_CONDITION(nl.modifiedNodes.contains( left->name()));
	CHECK_CONDITION(nl.modifiedNodes.contains( root));

	CHECK_INT_EQUAL(10, nl.removedNodes.size());
	CHECK_CONDITION(nl.removedNodes.contains( right));
	CHECK_CONDITION(nl.removedNodes.contains( right->name()));
	CHECK_CONDITION(nl.removedNodes.contains( left));
	CHECK_CONDITION(nl.removedNodes.contains( left->name()));

	auto posLeft = left->extension<TestNodes::PositionExtension>();
	CHECK_CONDITION(nl.removedNodes.contains( posLeft->xNode()) );
	CHECK_CONDITION(nl.removedNodes.contains( posLeft->yNode()) );

	auto posRight = right->extension<TestNodes::PositionExtension>();
	CHECK_CONDITION(nl.removedNodes.contains( posRight->xNode()) );
	CHECK_CONDITION(nl.removedNodes.contains( posRight->yNode()) );

	nl.modifiedNodes.clear();
	nl.removedNodes.clear();
	manager.beginModification(nullptr);
	manager.redo();
	manager.endModification();

	CHECK_INT_EQUAL(1, nl.modifiedNodes.size());
	CHECK_INT_EQUAL(0, nl.removedNodes.size());

	manager.beginModification(root);
	root->removeRightNode();
	manager.endModification();

	CHECK_INT_EQUAL(5, nl.removedNodes.size());
	CHECK_CONDITION(nl.removedNodes.contains( right));
	CHECK_CONDITION(nl.removedNodes.contains( right->name()));
	CHECK_CONDITION(nl.removedNodes.contains( posRight->xNode()) );
	CHECK_CONDITION(nl.removedNodes.contains( posRight->yNode()) );
}};

}
