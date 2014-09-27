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

#include "AlloyIntegrationPlugin.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "ModelBase/src/model/TreeManager.h"

#include "OOModel/src/allOOModelNodes.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/items/RootItem.h"

#include "OOInteraction/src/expression_editor/OOExpressionBuilder.h"

using namespace Visualization;
using namespace OOModel;
using namespace OOInteraction;

namespace Alloy {

Class* addLinkedList()
{
	auto aLinkedList = new Class("LinkedList");

	auto *linkedListRoot = new Field( "root", new ReferenceExpression("Node"), Modifier::Private);
	aLinkedList->fields()->append(linkedListRoot);

	auto invariantMethod = new Method("ObjectInvariant");
	aLinkedList->methods()->append(invariantMethod);

	auto containsMethod = new Method("contains");
	aLinkedList->methods()->append(containsMethod);

	FormalArgument* containsMethodArgument = new FormalArgument();
	containsMethod->arguments()->append(containsMethodArgument);
	containsMethodArgument->setName("aNode");
	ClassTypeExpression* argType = new ClassTypeExpression();
	containsMethodArgument->setTypeExpression(argType);
	argType->typeExpression()->ref()->setName("Node");

	FormalResult* containsResult  = new FormalResult();
	containsResult->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN));
	containsMethod->results()->append(containsResult);

	containsMethod->items()->append(new ExpressionStatement(OOExpressionBuilder::getOOExpression(
		"Contract.Requires(aNode!=null)")));
	containsMethod->items()->append(new ExpressionStatement(OOExpressionBuilder::getOOExpression(
		"Contract.Ensures(this.root.contains(aNode))")));

	return aLinkedList;
}

Class* addNode()
{
	auto aNode = new Class("Node");

	auto *nodeNext = new Field( "next", new ReferenceExpression("Node"), Modifier::Private);
	aNode->fields()->append(nodeNext);
	auto *numberFieldNode = new Field( "number", new PrimitiveTypeExpression(PrimitiveType::INT), Modifier::Private);
	aNode->fields()->append(numberFieldNode);

	auto invariantMethod = new Method("ObjectInvariant");
	aNode->methods()->append(invariantMethod);

	invariantMethod->items()->append(new ExpressionStatement(OOExpressionBuilder::getOOExpression(
		"Contract.Invariant(this!=this.next)")));
	invariantMethod->items()->append(new ExpressionStatement(OOExpressionBuilder::getOOExpression(
		"Contract.Invariant(this.number==this.next.number-1)")));
	invariantMethod->items()->append(new ExpressionStatement(OOExpressionBuilder::getOOExpression(
		"Contract.Invariant(LinkedList.contains(this))")));

	auto containsMethod = new Method("contains");
	aNode->methods()->append(containsMethod);

	FormalArgument* containsMethodArgument = new FormalArgument();
	containsMethod->arguments()->append(containsMethodArgument);
	containsMethodArgument->setName("aNode");
	ClassTypeExpression* argType = new ClassTypeExpression();
	containsMethodArgument->setTypeExpression(argType);
	argType->typeExpression()->ref()->setName("Node");

	FormalResult* containsResult  = new FormalResult();
	containsResult->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN));
	containsMethod->results()->append(containsResult);

	containsMethod->items()->append(new ExpressionStatement(OOExpressionBuilder::getOOExpression(
		"Contract.Requires(aNode!=null)")));
	containsMethod->items()->append(new ExpressionStatement(OOExpressionBuilder::getOOExpression(
		"Contract.Ensures(this==aNode||this.next!=null&&this.next.contains(aNode))")));

	return aNode;
}

TEST(AlloyIntegrationPlugin, AlloyTest)
{
	CHECK_INT_EQUAL(1, 1);
	auto aLinkedList = addLinkedList();

	auto aNode = addNode();
	aLinkedList->classes()->append(aNode);

	auto manager = new Model::TreeManager(aLinkedList);

	VisualizationManager::instance().mainScene()->addTopLevelItem(new RootItem(aLinkedList));

	VisualizationManager::instance().mainScene()->listenToTreeManager(manager);
}

}
