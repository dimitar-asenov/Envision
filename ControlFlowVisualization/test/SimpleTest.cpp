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
	Class* cl = NULL;

	if (!parent) cl = dynamic_cast<Class*> (model->createRoot("Class"));
	model->beginModification(parent ? static_cast<Model::Node*> (parent) :cl, "Adding a hello world class.");
	if (!cl) cl = parent->classes()->append<Class>();

	cl->setName("SomeClass");

	model->endModification();
	return cl;
}

Method* addComplicated(Model::Model* model, Class* parent)
{
	Method* met = NULL;

	if (!parent) met = dynamic_cast<Method*> (model->createRoot("Method"));
	model->beginModification(parent? static_cast<Model::Node*> (parent) : met, "Adding a Complicated method.");
	if (!met) met = parent->methods()->append<Method>();

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
	met->extension<Position>()->setX(400);

	model->endModification();
	return met;
}

Method* addDivBySix(Model::Model* model, Class* parent)
{
	Method* divbysix = NULL;

	if (!parent) divbysix = dynamic_cast<Method*> (model->createRoot("Method"));
	model->beginModification(parent? static_cast<Model::Node*> (parent) : divbysix, "Adding a divBySix method.");
	if (!divbysix) divbysix = parent->methods()->append<Method>();

	divbysix->setName("findDivBySix");
	divbysix->results()->append<FormalResult>()->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	FormalArgument* arg = divbysix->arguments()->append<FormalArgument>();
	arg->setName("numbers");
	arg->setType<ArrayType>()->setType<PrimitiveType>()->setType(PrimitiveType::INT);

	VariableDeclaration* result = divbysix->items()->append<VariableDeclaration>();
	result->setName("result");
	result->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	result->setInitialValue<IntegerLiteral>()->setValue(-1);

	LoopStatement* sixloop = divbysix->items()->append<LoopStatement>();
	VariableDeclaration* sixLoopInit = sixloop->setInitStep<VariableDeclaration>();
	sixLoopInit->setName("i");
	sixLoopInit->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	sixLoopInit->setInitialValue<IntegerLiteral>()->setValue(0);
	BinaryOperation* sixLoopCond = sixloop->setCondition<BinaryOperation>();
	sixLoopCond->setLeft<VariableAccess>()->ref()->set("local:i");
	sixLoopCond->setOp(BinaryOperation::LESS);
	MethodCallExpression* sizeCall = sixLoopCond->setRight<MethodCallExpression>();
	sizeCall->ref()->set("size");
	sizeCall->setPrefix<VariableAccess>()->ref()->set("local:numbers");

	//TODO test the visualization without the remaining parts of this method
	AssignmentStatement* sixLoopUpdate = sixloop->setUpdateStep<AssignmentStatement>();
	sixLoopUpdate->setLeft<VariableAccess>()->ref()->set("local:i");
	sixLoopUpdate->setOp(AssignmentStatement::PLUS_ASSIGN);
	sixLoopUpdate->setRight<IntegerLiteral>()->setValue(1);

	VariableDeclaration* n = sixloop->body()->append<VariableDeclaration>();
	n->setName("n");
	n->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	BinaryOperation* item = n->setInitialValue<BinaryOperation>();
	item->setLeft<VariableAccess>()->ref()->set("local:numbers");
	item->setOp(BinaryOperation::ARRAY_INDEX);
	item->setRight<VariableAccess>()->ref()->set("local:i");

	IfStatement* ifdiv2 = sixloop->body()->append<IfStatement>();
	BinaryOperation* eq0 = ifdiv2->setCondition<BinaryOperation>();
	eq0->setOp(BinaryOperation::EQUALS);
	eq0->setRight<IntegerLiteral>()->setValue(0);
	BinaryOperation* div2 = eq0->setLeft<BinaryOperation>();
	div2->setLeft<VariableAccess>()->ref()->set("local:n");
	div2->setOp(BinaryOperation::REMAINDER);
	div2->setRight<IntegerLiteral>()->setValue(2);
	ifdiv2->elseBranch()->append<ContinueStatement>();

	IfStatement* ifdiv3 = ifdiv2->thenBranch()->append<IfStatement>();
	eq0 = ifdiv3->setCondition<BinaryOperation>();
	eq0->setOp(BinaryOperation::EQUALS);
	eq0->setRight<IntegerLiteral>()->setValue(0);
	BinaryOperation* div3 = eq0->setLeft<BinaryOperation>();
	div3->setLeft<VariableAccess>()->ref()->set("local:n");
	div3->setOp(BinaryOperation::REMAINDER);
	div3->setRight<IntegerLiteral>()->setValue(3);

	AssignmentStatement* resultFound = ifdiv3->thenBranch()->append<AssignmentStatement>();
	resultFound->setLeft<VariableAccess>()->ref()->set("local:result");
	resultFound->setOp(AssignmentStatement::ASSIGN);
	resultFound->setRight<VariableAccess>()->ref()->set("local:i");
	ifdiv3->thenBranch()->append<BreakStatement>();

	divbysix->items()->append<ReturnStatement>()->values()->append<VariableAccess>()->ref()->set("local:result");

	model->endModification();
	return divbysix;
}

TEST(ControlFlowVisualization, SimpleTest)
{
	Model::Model* model = new Model::Model();

	Class* cl = NULL;
	cl = addClass(model, NULL);

	Method* complicated = NULL;
	complicated = addComplicated(model, cl);

	Method* divbysix = NULL;
	divbysix = addDivBySix(model, cl);

	Model::Node* top_level = NULL;
	if (cl) top_level = cl;
	else if(complicated) top_level = complicated;
	else top_level = divbysix;

	Scene* scene = new Scene();
	scene->addTopLevelItem( scene->defaultRenderer()->render(NULL, top_level) );
	scene->scheduleUpdate();
	scene->listenToModel(model);

	// Create view
	MainView* view = new MainView(scene);

	CHECK_CONDITION(view != NULL);
}

}
