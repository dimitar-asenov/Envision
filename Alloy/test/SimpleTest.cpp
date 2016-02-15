/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "../src/AlloyPlugin.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "ModelBase/src/model/TreeManager.h"

#include "OOModel/src/allOOModelNodes.h"

#include "VisualizationBase/src/VisualizationManager.h"

#include "OOInteraction/src/expression_editor/OOExpressionBuilder.h"

#include "VisualizationBase/src/Scene.h"

using namespace Visualization;
using namespace OOModel;
using namespace OOInteraction;

namespace Alloy {

Class* addLinkedList()
{
	auto aLinkedList = new Class{"LinkedList"};

	auto *linkedListRoot = new Field{ "root", new ReferenceExpression{"Node"}, Modifier::Private};
	aLinkedList->fields()->append(linkedListRoot);

	auto invariantMethod = new Method{"ObjectInvariant"};
	aLinkedList->methods()->append(invariantMethod);

	{
		auto callContractInvariant = new MethodCallExpression{"Contract.Invariant"};
		invariantMethod->items()->append(new ExpressionStatement{callContractInvariant});
		auto callContractForAll = new MethodCallExpression{"Contract.ForAll"};
		callContractInvariant->arguments()->append(callContractForAll);
		DCast<ReferenceExpression>(callContractForAll->callee())
				->typeArguments()->append(new ReferenceExpression{"Node"});
		callContractForAll->arguments()->append(new MethodCallExpression{"getAllNodes"});
		auto le = new LambdaExpression{};
		callContractForAll->arguments()->append(le);
		le->arguments()->append(new FormalArgument{"n", new ReferenceExpression{"Node"}});
		le->body()->append(new ReturnStatement{OOExpressionBuilder::getOOExpression(
			"this.root==null||n.next==null||n.next!=this.root")});
	}

	{
		auto callContractInvariant = new MethodCallExpression{"Contract.Invariant"};
		invariantMethod->items()->append(new ExpressionStatement{callContractInvariant});
		auto callContractForAll = new MethodCallExpression{"Contract.ForAll"};
		callContractInvariant->arguments()->append(callContractForAll);
		DCast<ReferenceExpression>(callContractForAll->callee())
				->typeArguments()->append(new ReferenceExpression{"LinkedList"});
		callContractForAll->arguments()->append(new MethodCallExpression{"getAllLinkedLists"});
		auto le = new LambdaExpression{};
		callContractForAll->arguments()->append(le);
		le->arguments()->append(new FormalArgument{"ll", new ReferenceExpression{"LinkedList"}});
		le->body()->append(new ReturnStatement{OOExpressionBuilder::getOOExpression(
			"this.root==null||ll.root==null||this==ll||this.root!=ll.root")});
	}

	auto containsMethod = new Method{"contains"};
	aLinkedList->methods()->append(containsMethod);

	FormalArgument* containsMethodArgument = new FormalArgument{};
	containsMethod->arguments()->append(containsMethodArgument);
	containsMethodArgument->setName("aNode");
	ClassTypeExpression* argType = new ClassTypeExpression{};
	containsMethodArgument->setTypeExpression(argType);
	argType->typeExpression()->ref()->setName("Node");

	FormalResult* containsResult  = new FormalResult{};
	containsResult->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN});
	containsMethod->results()->append(containsResult);

	containsMethod->items()->append(new ExpressionStatement{OOExpressionBuilder::getOOExpression(
		"Contract.Requires(aNode!=null)")});
	containsMethod->items()->append(new ExpressionStatement{OOExpressionBuilder::getOOExpression(
		"Contract.Ensures(this.root.contains(aNode))")});

	return aLinkedList;
}

Class* addNode()
{
	auto aNode = new Class{"Node"};

	auto *nodeNext = new Field{ "next", new ReferenceExpression{"Node"}, Modifier::Private};
	aNode->fields()->append(nodeNext);

	auto invariantMethod = new Method{"ObjectInvariant"};
	aNode->methods()->append(invariantMethod);

	invariantMethod->items()->append(new ExpressionStatement{OOExpressionBuilder::getOOExpression(
		"Contract.Invariant(this!=this.next)")});

	{
		auto callContractInvariant = new MethodCallExpression{"Contract.Invariant"};
		invariantMethod->items()->append(new ExpressionStatement{callContractInvariant});
		auto callContractExists = new MethodCallExpression{"Contract.Exists"};
		callContractInvariant->arguments()->append(callContractExists);
		DCast<ReferenceExpression>(callContractExists->callee())
				->typeArguments()->append(new ReferenceExpression{"LinkedList"});
		callContractExists->arguments()->append(new MethodCallExpression{"getAllLinkedLists"});
		auto le = new LambdaExpression{};
		callContractExists->arguments()->append(le);
		le->arguments()->append(new FormalArgument{"ll", new ReferenceExpression{"LinkedList"}});
		le->body()->append(new ReturnStatement{OOExpressionBuilder::getOOExpression("ll.contains(this)")});
	}

	{
		auto callContractInvariant = new MethodCallExpression{"Contract.Invariant"};
		invariantMethod->items()->append(new ExpressionStatement{callContractInvariant});
		auto callContractForAll = new MethodCallExpression{"Contract.ForAll"};
		callContractInvariant->arguments()->append(callContractForAll);
		DCast<ReferenceExpression>(callContractForAll->callee())
				->typeArguments()->append(new ReferenceExpression{"Node"});
		callContractForAll->arguments()->append(new MethodCallExpression{"getAllNodes"});
		auto le = new LambdaExpression{};
		callContractForAll->arguments()->append(le);
		le->arguments()->append(new FormalArgument{"n", new ReferenceExpression{"Node"}});
		le->body()->append(new ReturnStatement{OOExpressionBuilder::getOOExpression(
			"this.next==null||n.next==null||this==n||this.next!=n.next")});
	}

	auto containsMethod = new Method{"contains"};
	aNode->methods()->append(containsMethod);

	FormalArgument* containsMethodArgument = new FormalArgument{};
	containsMethod->arguments()->append(containsMethodArgument);
	containsMethodArgument->setName("aNode");
	ClassTypeExpression* argType = new ClassTypeExpression{};
	containsMethodArgument->setTypeExpression(argType);
	argType->typeExpression()->ref()->setName("Node");

	FormalResult* containsResult  = new FormalResult{};
	containsResult->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN});
	containsMethod->results()->append(containsResult);

	containsMethod->items()->append(new ExpressionStatement{OOExpressionBuilder::getOOExpression(
		"Contract.Requires(aNode!=null)")});
	containsMethod->items()->append(new ExpressionStatement{OOExpressionBuilder::getOOExpression(
		"Contract.Ensures(this==aNode||(this.next!=null&&this.next.contains(aNode)))")});

	return aNode;
}

class AlloyTest : public SelfTest::Test<AlloyPlugin, AlloyTest> { public: void test()
{
	CHECK_INT_EQUAL(1, 1);
	auto aLinkedList = addLinkedList();

	auto aNode = addNode();
	aLinkedList->classes()->append(aNode);

	auto manager = new Model::TreeManager{aLinkedList};

	VisualizationManager::instance().mainScene()->addTopLevelNode(aLinkedList);

	VisualizationManager::instance().mainScene()->listenToTreeManager(manager);
}};

}
