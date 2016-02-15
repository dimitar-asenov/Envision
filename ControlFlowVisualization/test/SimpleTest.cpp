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

#include "../src/ControlFlowVisualizationPlugin.h"
#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"

#include "OOModel/src/allOOModelNodes.h"

#include "InteractionBase/src/autocomplete/AutoComplete.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/node_extensions/Position.h"

using namespace OOModel;
using namespace Visualization;

namespace ControlFlowVisualization {

Method* addComplicated(Class* parent)
{
	auto met = new Method{"complicated"};
	if (parent) parent->methods()->append(met);

	VariableDeclarationExpression* a = new VariableDeclarationExpression{"a"};
	met->items()->append(new ExpressionStatement{a});
	a->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});

	VariableDeclarationExpression* b = new VariableDeclarationExpression{"b"};
	met->items()->append(new ExpressionStatement{b});
	b->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT});
	b->decl()->setInitialValue(new IntegerLiteral{1000});

	LoopStatement* loop = new LoopStatement{};
	met->items()->append(loop);
	VariableDeclarationExpression* initStep = new VariableDeclarationExpression{"i"};
	loop->setInitStep(initStep);
	initStep->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	initStep->decl()->setInitialValue(new IntegerLiteral{0});
	BinaryOperation* loopCondition = new BinaryOperation{};
	loop->setCondition(loopCondition);
	loopCondition->setLeft(new ReferenceExpression{"i"});
	loopCondition->setOp(BinaryOperation::LESS);
	loopCondition->setRight(new ReferenceExpression{"a"});
	AssignmentExpression* updateStep = new AssignmentExpression{};
	loop->setUpdateStep(updateStep);
	updateStep->setLeft(new ReferenceExpression{"i"});
	updateStep->setOp(AssignmentExpression::PLUS_ASSIGN);
	updateStep->setRight(new IntegerLiteral{1});

	AssignmentExpression* loopBodyAssignment = new AssignmentExpression{};
	loop->body()->append(new ExpressionStatement{loopBodyAssignment});
	loopBodyAssignment->setLeft(new ReferenceExpression{"b"});
	loopBodyAssignment->setOp(AssignmentExpression::TIMES_ASSIGN);
	loopBodyAssignment->setRight(new IntegerLiteral{2});

	IfStatement* loopIf = new IfStatement{};
	loop->body()->append(loopIf);
	BinaryOperation* ifCond = new BinaryOperation{};
	loopIf->setCondition(ifCond);
	ifCond->setLeft(new ReferenceExpression{"i"});
	ifCond->setOp(BinaryOperation::NOT_EQUALS);
	ifCond->setRight(new IntegerLiteral{10});
	IfStatement* loopIfLeft = new IfStatement{};
	loopIf->thenBranch()->append(loopIfLeft);
	IfStatement* loopIfRight = new IfStatement{};
	loopIf->elseBranch()->append(loopIfRight);

	BinaryOperation* ifLeftCondition = new BinaryOperation{};
	loopIfLeft->setCondition(ifLeftCondition);
	ifLeftCondition->setLeft(new ReferenceExpression{"a"});
	ifLeftCondition->setOp(BinaryOperation::GREATER_EQUALS);
	ifLeftCondition->setRight(new IntegerLiteral{3});
	loopIfLeft->thenBranch()->append(new ContinueStatement{});
	loopIfLeft->thenBranch()->append(new BreakStatement{});
	loopIfLeft->thenBranch()->append(new ContinueStatement{});
	loopIfLeft->thenBranch()->append(new BreakStatement{});

	BinaryOperation* ifRightCondition = new BinaryOperation{};
	loopIfRight->setCondition(ifRightCondition);
	ifRightCondition->setLeft(new ReferenceExpression{"b"});
	ifRightCondition->setOp(BinaryOperation::EQUALS);
	ifRightCondition->setRight(new IntegerLiteral{-20});
	loopIfRight->thenBranch()->append(new ContinueStatement{});
	loopIfRight->thenBranch()->append(new BreakStatement{});
	loopIfRight->thenBranch()->append(new ContinueStatement{});
	loopIfRight->thenBranch()->append(new BreakStatement{});
	loopIfRight->elseBranch()->append(new ContinueStatement{});
	loopIfRight->elseBranch()->append(new BreakStatement{});
	loopIfRight->elseBranch()->append(new ContinueStatement{});
	loopIfRight->elseBranch()->append(new BreakStatement{});


	loop->body()->append(new ContinueStatement{});
	loop->body()->append(new BreakStatement{});

	ForEachStatement* forEach = new ForEachStatement{};
	met->items()->append(forEach);
	forEach->setVarName("elem");
	forEach->setVarType( new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT} );
	forEach->setCollection(new ReferenceExpression{"SomeCollection"});
	AssignmentExpression* assignEach = new AssignmentExpression{};
	forEach->body()->append(new ExpressionStatement{assignEach});
	assignEach->setLeft(new ReferenceExpression{"a"});
	assignEach->setOp(AssignmentExpression::DIVIDE_ASSIGN);
	assignEach->setRight(new ReferenceExpression{"elem"});


	ReturnStatement* metReturn = new ReturnStatement{};
	metReturn->values()->append(new IntegerLiteral{42});
	met->items()->append(metReturn);
	met->extension<Position>()->set(1, 0);

	return met;
}

Method* addDivBySix(Class* parent)
{
	auto divbysix = new Method{"findDivBySix"};
	if (parent) parent->methods()->append(divbysix);

	FormalResult* divbysixResult = new FormalResult{};
	divbysixResult->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	divbysix->results()->append(divbysixResult);
	FormalArgument* arg = new FormalArgument{};
	divbysix->arguments()->append(arg);
	arg->setName("numbers");
	ArrayTypeExpression* argType = new ArrayTypeExpression{};
	argType->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	arg->setTypeExpression(argType);

	VariableDeclarationExpression* result = new VariableDeclarationExpression{"result"};
	divbysix->items()->append(new ExpressionStatement{result});
	result->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	result->decl()->setInitialValue(new IntegerLiteral{-1});

	LoopStatement* sixloop = new LoopStatement{};
	divbysix->items()->append(sixloop);
	VariableDeclarationExpression* sixLoopInit = new VariableDeclarationExpression{"i"};
	sixloop->setInitStep(sixLoopInit);
	sixLoopInit->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	sixLoopInit->decl()->setInitialValue(new IntegerLiteral{0});
	BinaryOperation* sixLoopCond = new BinaryOperation{};
	sixloop->setCondition(sixLoopCond);
	sixLoopCond->setLeft(new ReferenceExpression{"i"});
	sixLoopCond->setOp(BinaryOperation::LESS);
	MethodCallExpression* sizeCall = new MethodCallExpression{"size", new ReferenceExpression{"numbers"}};
	sixLoopCond->setRight(sizeCall);

	//TODO test the visualization without the remaining parts of this method
	AssignmentExpression* sixLoopUpdate = new AssignmentExpression{};
	sixloop->setUpdateStep(sixLoopUpdate);
	sixLoopUpdate->setLeft(new ReferenceExpression{"i"});
	sixLoopUpdate->setOp(AssignmentExpression::PLUS_ASSIGN);
	sixLoopUpdate->setRight(new IntegerLiteral{1});

	VariableDeclarationExpression* n = new VariableDeclarationExpression{"n"};
	sixloop->body()->append(new ExpressionStatement{n});
	n->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	BinaryOperation* item = new BinaryOperation{};
	n->decl()->setInitialValue(item);
	item->setLeft(new ReferenceExpression{"numbers"});
	item->setOp(BinaryOperation::ARRAY_INDEX);
	item->setRight(new ReferenceExpression{"i"});

	IfStatement* ifdiv2 = new IfStatement{};
	sixloop->body()->append(ifdiv2);
	BinaryOperation* eq0 = new BinaryOperation{};
	ifdiv2->setCondition(eq0);
	eq0->setOp(BinaryOperation::EQUALS);
	eq0->setRight(new IntegerLiteral{0});
	BinaryOperation* div2 = new BinaryOperation{};
	eq0->setLeft(div2);
	div2->setLeft(new ReferenceExpression{"n"});
	div2->setOp(BinaryOperation::REMAINDER);
	div2->setRight(new IntegerLiteral{2});
	ifdiv2->elseBranch()->append(new ContinueStatement{});

	IfStatement* ifdiv3 = new IfStatement{};
	ifdiv2->thenBranch()->append(ifdiv3);
	eq0 = new BinaryOperation{};
	ifdiv3->setCondition(eq0);
	eq0->setOp(BinaryOperation::EQUALS);
	eq0->setRight(new IntegerLiteral{0});
	BinaryOperation* div3 = new BinaryOperation{};
	eq0->setLeft(div3);
	div3->setLeft(new ReferenceExpression{"n"});
	div3->setOp(BinaryOperation::REMAINDER);
	div3->setRight(new IntegerLiteral{3});

	AssignmentExpression* resultFound = new AssignmentExpression{};
	ifdiv3->thenBranch()->append(new ExpressionStatement{resultFound});
	resultFound->setLeft(new ReferenceExpression{"result"});
	resultFound->setOp(AssignmentExpression::ASSIGN);
	resultFound->setRight(new ReferenceExpression{"i"});
	ifdiv3->thenBranch()->append( new BreakStatement{});

	ReturnStatement* divbysixFinalReturn = new ReturnStatement{};
	divbysixFinalReturn->values()->append(new ReferenceExpression{"result"});
	divbysix->items()->append(divbysixFinalReturn);

	divbysix->extension<Position>()->set(0, 0);

	return divbysix;
}

class SimpleTest : public SelfTest::Test<ControlFlowVisualizationPlugin, SimpleTest> { public: void test()
{
	auto cl = new Class{"SomeClass"};

	addComplicated(cl);
	addDivBySix(cl);

	auto manager = new Model::TreeManager{cl};

	VisualizationManager::instance().mainScene()->addTopLevelNode(cl);
	VisualizationManager::instance().mainScene()->listenToTreeManager(manager);

	CHECK_CONDITION(cl != nullptr);
}};

}
