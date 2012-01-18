/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "controlflowvisualization.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "OOModel/headers/allOOModelNodes.h"

#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/views/MainView.h"
#include "VisualizationBase/headers/node_extensions/Position.h"

using namespace OOModel;
using namespace Visualization;

namespace ControlFlowVisualization {

Class* addClass(Model::Model* model, Project* parent)
{
	Class* cl = nullptr;

	if (!parent) cl = dynamic_cast<Class*> (model->createRoot("Class"));
	model->beginModification(parent ? static_cast<Model::Node*> (parent) :cl, "Adding a hello world class.");
	if (!cl)
	{
		cl = new Class();
		parent->classes()->append(cl);
	}

	cl->setName("SomeClass");

	model->endModification();
	return cl;
}

Method* addComplicated(Model::Model* model, Class* parent)
{
	Method* met = nullptr;

	if (!parent) met = dynamic_cast<Method*> (model->createRoot("Method"));
	model->beginModification(parent? static_cast<Model::Node*> (parent) : met, "Adding a Complicated method.");
	if (!met)
	{
		met = new Method();
		parent->methods()->append(met);
	}

	met->setName("complicated");

	VariableDeclaration* a = new VariableDeclaration();
	met->items()->append(a);
	a->setName("a");
	a->setType(new PrimitiveType(PrimitiveType::INT));

	VariableDeclaration* b = new VariableDeclaration();
	met->items()->append(b);
	b->setName("b");
	b->setType(new PrimitiveType(PrimitiveType::UNSIGNED_INT));
	b->setInitialValue(new IntegerLiteral(1000));

	LoopStatement* loop = new LoopStatement();
	met->items()->append(loop);
	VariableDeclaration* initStep = new VariableDeclaration();
	loop->setInitStep(initStep);
	initStep->setType(new PrimitiveType(PrimitiveType::INT));
	initStep->setName("i");
	initStep->setInitialValue(new IntegerLiteral(0));
	BinaryOperation* loopCondition = new BinaryOperation();
	loop->setCondition(loopCondition);
	loopCondition->setLeft(new VariableAccess("local:i"));
	loopCondition->setOp(BinaryOperation::LESS);
	loopCondition->setRight(new VariableAccess("local:a"));
	AssignmentStatement* updateStep = new AssignmentStatement();
	loop->setUpdateStep(updateStep);
	updateStep->setLeft(new VariableAccess("local:i"));
	updateStep->setOp(AssignmentStatement::PLUS_ASSIGN);
	updateStep->setRight(new IntegerLiteral(1));

	AssignmentStatement* loopBodyAssignment = new AssignmentStatement();
	loop->body()->append(loopBodyAssignment);
	loopBodyAssignment->setLeft(new VariableAccess("local:b"));
	loopBodyAssignment->setOp(AssignmentStatement::TIMES_ASSIGN);
	loopBodyAssignment->setRight(new IntegerLiteral(2));

	IfStatement* loopIf = new IfStatement();
	loop->body()->append(loopIf);
	BinaryOperation* ifCond = new BinaryOperation();
	loopIf->setCondition(ifCond);
	ifCond->setLeft(new VariableAccess("local:i"));
	ifCond->setOp(BinaryOperation::NOT_EQUALS);
	ifCond->setRight(new IntegerLiteral(10));
	IfStatement* loopIfLeft = new IfStatement();
	loopIf->thenBranch()->append(loopIfLeft);
	IfStatement* loopIfRight = new IfStatement();
	loopIf->elseBranch()->append(loopIfRight);

	BinaryOperation* ifLeftCondition = new BinaryOperation();
	loopIfLeft->setCondition(ifLeftCondition);
	ifLeftCondition->setLeft(new VariableAccess("local:a"));
	ifLeftCondition->setOp(BinaryOperation::GREATER_EQUALS);
	ifLeftCondition->setRight(new IntegerLiteral(3));
	loopIfLeft->thenBranch()->append(new ContinueStatement());
	loopIfLeft->thenBranch()->append(new BreakStatement());
	loopIfLeft->thenBranch()->append(new ContinueStatement());
	loopIfLeft->thenBranch()->append(new BreakStatement());

	BinaryOperation* ifRightCondition = new BinaryOperation();
	loopIfRight->setCondition(ifRightCondition);
	ifRightCondition->setLeft(new VariableAccess("local:b"));
	ifRightCondition->setOp(BinaryOperation::EQUALS);
	ifRightCondition->setRight(new IntegerLiteral(-20));
	loopIfRight->thenBranch()->append(new ContinueStatement());
	loopIfRight->thenBranch()->append(new BreakStatement());
	loopIfRight->thenBranch()->append(new ContinueStatement());
	loopIfRight->thenBranch()->append(new BreakStatement());
	loopIfRight->elseBranch()->append(new ContinueStatement());
	loopIfRight->elseBranch()->append(new BreakStatement());
	loopIfRight->elseBranch()->append(new ContinueStatement());
	loopIfRight->elseBranch()->append(new BreakStatement());


	loop->body()->append(new ContinueStatement());
	loop->body()->append(new BreakStatement());

	ForEachStatement* forEach = new ForEachStatement();
	met->items()->append(forEach);
	forEach->setVarName("elem");
	forEach->setVarType( new PrimitiveType(PrimitiveType::UNSIGNED_INT) );
	forEach->setCollection(new VariableAccess("global:SomeCollection"));
	AssignmentStatement* assignEach = new AssignmentStatement();
	forEach->body()->append(assignEach);
	assignEach->setLeft(new VariableAccess("local:a"));
	assignEach->setOp(AssignmentStatement::DIVIDE_ASSIGN);
	assignEach->setRight(new VariableAccess("local:elem"));


	ReturnStatement* metReturn = new ReturnStatement();
	metReturn->values()->append(new IntegerLiteral(42));
	met->items()->append(metReturn);
	met->extension<Position>()->setX(400);

	model->endModification();
	return met;
}

Method* addDivBySix(Model::Model* model, Class* parent)
{
	Method* divbysix = nullptr;

	if (!parent) divbysix = dynamic_cast<Method*> (model->createRoot("Method"));
	model->beginModification(parent? static_cast<Model::Node*> (parent) : divbysix, "Adding a divBySix method.");
	if (!divbysix)
	{
		divbysix = new Method();
		parent->methods()->append(divbysix);
	}

	divbysix->setName("findDivBySix");
	FormalResult* divbysixResult = new FormalResult();
	divbysixResult->setType(new PrimitiveType(PrimitiveType::INT));
	divbysix->results()->append(divbysixResult);
	FormalArgument* arg = new FormalArgument();
	divbysix->arguments()->append(arg);
	arg->setName("numbers");
	ArrayType* argType = new ArrayType();
	argType->setType(new PrimitiveType(PrimitiveType::INT));
	arg->setType(argType);

	VariableDeclaration* result = new VariableDeclaration();
	divbysix->items()->append(result);
	result->setName("result");
	result->setType(new PrimitiveType(PrimitiveType::INT));
	result->setInitialValue(new IntegerLiteral(-1));

	LoopStatement* sixloop = new LoopStatement();
	divbysix->items()->append(sixloop);
	VariableDeclaration* sixLoopInit = new VariableDeclaration();
	sixloop->setInitStep(sixLoopInit);
	sixLoopInit->setName("i");
	sixLoopInit->setType(new PrimitiveType(PrimitiveType::INT));
	sixLoopInit->setInitialValue(new IntegerLiteral(0));
	BinaryOperation* sixLoopCond = new BinaryOperation();
	sixloop->setCondition(sixLoopCond);
	sixLoopCond->setLeft(new VariableAccess("local:i"));
	sixLoopCond->setOp(BinaryOperation::LESS);
	MethodCallExpression* sizeCall = new MethodCallExpression();
	sixLoopCond->setRight(sizeCall);
	sizeCall->ref()->set("size");
	sizeCall->setPrefix(new VariableAccess("local:numbers"));

	//TODO test the visualization without the remaining parts of this method
	AssignmentStatement* sixLoopUpdate = new AssignmentStatement();
	sixloop->setUpdateStep(sixLoopUpdate);
	sixLoopUpdate->setLeft(new VariableAccess("local:i"));
	sixLoopUpdate->setOp(AssignmentStatement::PLUS_ASSIGN);
	sixLoopUpdate->setRight(new IntegerLiteral(1));

	VariableDeclaration* n = new VariableDeclaration();
	sixloop->body()->append(n);
	n->setName("n");
	n->setType(new PrimitiveType(PrimitiveType::INT));
	BinaryOperation* item = new BinaryOperation();
	n->setInitialValue(item);
	item->setLeft(new VariableAccess("local:numbers"));
	item->setOp(BinaryOperation::ARRAY_INDEX);
	item->setRight(new VariableAccess("local:i"));

	IfStatement* ifdiv2 = new IfStatement();
	sixloop->body()->append(ifdiv2);
	BinaryOperation* eq0 = new BinaryOperation();
	ifdiv2->setCondition(eq0);
	eq0->setOp(BinaryOperation::EQUALS);
	eq0->setRight(new IntegerLiteral(0));
	BinaryOperation* div2 = new BinaryOperation();
	eq0->setLeft(div2);
	div2->setLeft(new VariableAccess("local:n"));
	div2->setOp(BinaryOperation::REMAINDER);
	div2->setRight(new IntegerLiteral(2));
	ifdiv2->elseBranch()->append(new  ContinueStatement());

	IfStatement* ifdiv3 = new IfStatement();
	ifdiv2->thenBranch()->append(ifdiv3);
	eq0 = new BinaryOperation();
	ifdiv3->setCondition(eq0);
	eq0->setOp(BinaryOperation::EQUALS);
	eq0->setRight(new IntegerLiteral(0));
	BinaryOperation* div3 = new BinaryOperation();
	eq0->setLeft(div3);
	div3->setLeft(new VariableAccess("local:n"));
	div3->setOp(BinaryOperation::REMAINDER);
	div3->setRight(new IntegerLiteral(3));

	AssignmentStatement* resultFound = new AssignmentStatement();
	ifdiv3->thenBranch()->append(resultFound);
	resultFound->setLeft(new VariableAccess("local:result"));
	resultFound->setOp(AssignmentStatement::ASSIGN);
	resultFound->setRight(new VariableAccess("local:i"));
	ifdiv3->thenBranch()->append( new BreakStatement());

	ReturnStatement* divbysixFinalReturn = new ReturnStatement();
	divbysixFinalReturn->values()->append(new VariableAccess("local:result"));
	divbysix->items()->append(divbysixFinalReturn);

	model->endModification();
	return divbysix;
}

TEST(ControlFlowVisualization, SimpleTest)
{
	Model::Model* model = new Model::Model();

	Class* cl = nullptr;
	cl = addClass(model, nullptr);

	Method* complicated = nullptr;
	complicated = addComplicated(model, cl);

	Method* divbysix = nullptr;
	divbysix = addDivBySix(model, cl);

	Model::Node* top_level = nullptr;
	if (cl) top_level = cl;
	else if(complicated) top_level = complicated;
	else top_level = divbysix;

	Scene* scene = new Scene();
	scene->addTopLevelItem( scene->defaultRenderer()->render(nullptr, top_level) );
	scene->scheduleUpdate();
	scene->listenToModel(model);

	// Create view
	MainView* view = new MainView(scene);

	CHECK_CONDITION(view != nullptr);
}

}
