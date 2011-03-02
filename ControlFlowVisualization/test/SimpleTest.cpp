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

using namespace OOModel;
using namespace Visualization;

namespace ControlFlowVisualization {

TEST(ControlFlowVisualization, SimpleTest)
{
	Scene* scene = new Scene();

	////////////////////////////////////////////////// Create Model
	Model::Model* model = new Model::Model();
	Method* met = dynamic_cast<Method*> (model->createRoot("Method"));

	model->beginModification(met, "make a flow control test method");
	met->setName("complicated");

	VariableDeclaration* a = met->items()->append<VariableDeclaration>();
	a->setName("a");
	a->setType<PrimitiveType>()->setType(PrimitiveType::INT);

	VariableDeclaration* b = met->items()->append<VariableDeclaration>();
	b->setName("b");
	b->setType<PrimitiveType>()->setType(PrimitiveType::UNSIGNED_INT);
	b->setInitialValue<IntegerLiteral>()->setValue(1000);

	LoopStatement* loop = met->items()->append<LoopStatement>();
	VariableDeclaration* initStep = loop->setInitStep<VariableDeclaration>();
	initStep->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	initStep->setName("i");
	initStep->setInitialValue<IntegerLiteral>()->setValue(0);
	BinaryOperation* loopCondition = loop->setCondition<BinaryOperation>();
	loopCondition->setLeft<VariableAccess>()->ref()->set("local:i");
	loopCondition->setOp(BinaryOperation::LESS);
	loopCondition->setRight<VariableAccess>()->ref()->set("local:a");
	AssignmentStatement* updateStep = loop->setUpdateStep<AssignmentStatement>();
	updateStep->setLeft<VariableAccess>()->ref()->set("local:i");
	updateStep->setOp(AssignmentStatement::PLUS_ASSIGN);
	updateStep->setRight<IntegerLiteral>()->setValue(1);

	AssignmentStatement* loopBodyAssignment = loop->body()->append<AssignmentStatement>();
	loopBodyAssignment->setLeft<VariableAccess>()->ref()->set("local:b");
	loopBodyAssignment->setOp(AssignmentStatement::TIMES_ASSIGN);
	loopBodyAssignment->setRight<IntegerLiteral>()->setValue(2);

	IfStatement* loopIf = loop->body()->append<IfStatement>();
	BinaryOperation* ifCond = loopIf->setCondition<BinaryOperation>();
	ifCond->setLeft<VariableAccess>()->ref()->set("local:i");
	ifCond->setOp(BinaryOperation::NOT_EQUALS);
	ifCond->setRight<IntegerLiteral>()->setValue(10);
	IfStatement* loopIfLeft = loopIf->thenBranch()->append<IfStatement>();
	IfStatement* loopIfRight = loopIf->elseBranch()->append<IfStatement>();

	BinaryOperation* ifLeftCondition = loopIfLeft->setCondition<BinaryOperation>();
	ifLeftCondition->setLeft<VariableAccess>()->ref()->set("local:a");
	ifLeftCondition->setOp(BinaryOperation::GREATER_EQUALS);
	ifLeftCondition->setRight<IntegerLiteral>()->setValue(3);
	loopIfLeft->thenBranch()->append<ContinueStatement>();
	loopIfLeft->thenBranch()->append<BreakStatement>();
	loopIfLeft->thenBranch()->append<ContinueStatement>();
	loopIfLeft->thenBranch()->append<BreakStatement>();


	BinaryOperation* ifRightCondition = loopIfRight->setCondition<BinaryOperation>();
	ifRightCondition->setLeft<VariableAccess>()->ref()->set("local:b");
	ifRightCondition->setOp(BinaryOperation::EQUALS);
	ifRightCondition->setRight<IntegerLiteral>()->setValue(-20);
	loopIfRight->thenBranch()->append<ContinueStatement>();
	loopIfRight->thenBranch()->append<BreakStatement>();
	loopIfRight->thenBranch()->append<ContinueStatement>();
	loopIfRight->thenBranch()->append<BreakStatement>();
	loopIfRight->elseBranch()->append<ContinueStatement>();
	loopIfRight->elseBranch()->append<BreakStatement>();
	loopIfRight->elseBranch()->append<ContinueStatement>();
	loopIfRight->elseBranch()->append<BreakStatement>();


	loop->body()->append<ContinueStatement>();
	loop->body()->append<BreakStatement>();

	ForEachStatement* forEach = met->items()->append<ForEachStatement>();
	forEach->setVarName("elem");
	forEach->setVarType<PrimitiveType>()->setType(PrimitiveType::UNSIGNED_INT);
	forEach->setCollection<VariableAccess>()->ref()->set("global:SomeCollection");
	AssignmentStatement* assignEach = forEach->body()->append<AssignmentStatement>();
	assignEach->setLeft<VariableAccess>()->ref()->set("local:a");
	assignEach->setOp(AssignmentStatement::DIVIDE_ASSIGN);
	assignEach->setRight<VariableAccess>()->ref()->set("loop:elem");


	met->items()->append<ReturnStatement>()->values()->append<IntegerLiteral>()->setValue(24);
	model->endModification();

	////////////////////////////////////////////////// Set Scene

	scene->addTopLevelItem( scene->defaultRenderer()->render(NULL, met) );
	scene->scheduleUpdate();

	// Create view
	MainView* view = new MainView(scene);

	CHECK_CONDITION(view != NULL);
}

}
