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
 * HelloWorldTest.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oovisualization.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "OOModel/headers/allOOModelNodes.h"

#include "VisualizationBase/headers/VisualizationManager.h"
#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/views/MainView.h"
#include "VisualizationBase/headers/ModelRenderer.h"
#include "VisualizationBase/headers/items/VExtendable.h"
#include "VisualizationBase/headers/items/VText.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/node_extensions/Position.h"

#include "ModelBase/headers/Model.h"

#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtCore/QCoreApplication>

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

// The methods below add a certain construct to an existing model or model entity.

Class* addHelloWorld(Model::Model* model, Project* parent)
{
	Class* hello = nullptr;

	if (!parent) hello = dynamic_cast<Class*> (model->createRoot("Class"));
	model->beginModification(parent ? static_cast<Model::Node*> (parent) :hello, "Adding a hello world class.");
	if (!hello)
	{
		hello = new Class();
		parent->classes()->append(hello);
	}

	hello->setName("HelloWorld");
	hello->setVisibility(Visibility::PUBLIC);
	Method* main = new Method();
	hello->methods()->append(main);
	main->setName("main");
	main->setVisibility(Visibility::PUBLIC);
	main->setStorageSpecifier(StorageSpecifier::CLASS_VARIABLE);

	FormalArgument* mainArgs = new FormalArgument();
	main->arguments()->append(mainArgs);
	mainArgs->setName("args");
	ArrayType* mainArgType = new ArrayType();
	mainArgs->setType(mainArgType);
	NamedType* mainArgElementType = new NamedType();
	mainArgType->setType(mainArgElementType);
	mainArgElementType->type()->ref()->set("class:String");

	MethodCallStatement* callPrintln = new MethodCallStatement();
	main->items()->append(callPrintln);
	callPrintln->arguments()->append(new StringLiteral("Hello World"));
	callPrintln->ref()->set("met:println");

	VariableAccess* va = new VariableAccess();
	callPrintln->setPrefix(va);
	va->ref()->set("field:out");

	ReferenceExpression* ref = new ReferenceExpression();
	va->setPrefix(ref);
	ref->ref()->set("class:System");
	ReferenceExpression* prefix = new ReferenceExpression();
	ref->setPrefix(prefix);
	prefix->ref()->set("lib:Java");

	model->endModification();
	return hello;
}

Library* addJavaLibrary(Model::Model* model, Project* parent)
{
	Library* java = nullptr;

	if (!parent) java = dynamic_cast<Library*> (model->createRoot("Library"));
	model->beginModification(parent? static_cast<Model::Node*> (parent) :java, "Adding a java library.");
	if (!java)
	{
		java = new Library();
		parent->libraries()->append( java );
	}

	java->setName("Java");

	Class* string = new Class();
	java->classes()->append(string);
	string->setName("String");
	string->setVisibility(Visibility::PUBLIC);
	NamedType* type = new NamedType();
	string->baseClasses()->append(type);
	type->type()->ref()->set("class:Object");

	Module* io = new Module();
	java->modules()->append(io);
	io->setName("io");

	Class* printstream = new Class();
	io->classes()->append(printstream);
	printstream->setName("PrintStream");
	printstream->setVisibility(Visibility::PUBLIC);

	Method* println = new Method();
	printstream->methods()->append(println);
	println->setName("println");
	println->setVisibility(Visibility::PUBLIC);

	FormalArgument* arg = new FormalArgument();
	println->arguments()->append(arg);
	arg->setName("x");
	NamedType* argType = new NamedType();
	arg->setType(argType);
	argType->type()->ref()->set("class:String");

	Class* system = new Class();
	java->classes()->append(system);
	system->setName("System");
	system->setVisibility(Visibility::PUBLIC);
	Field* out = new Field();
	system->fields()->append(out);
	out->setName("out");
	out->setVisibility(Visibility::PUBLIC);
	out->setStorageSpecifier(StorageSpecifier::CLASS_VARIABLE);
	NamedType* outtype = new NamedType();
	out->setType(outtype);
	outtype->type()->ref()->set("class:PrintStream");
	ReferenceExpression* prefix = new ReferenceExpression();
	outtype->type()->setPrefix(prefix);
	prefix->ref()->set("mod:io");

	// Set positions
	java->extension<Position>()->setX(400);
	string->extension<Position>()->setY(100);
	io->extension<Position>()->setX(240);

	model->endModification();
	return java;
}

Method* addLongMethod(Model::Model* model, Class* parent)
{
	Method* longMethod = nullptr;

	if (!parent) longMethod = dynamic_cast<Method*> (model->createRoot("Method"));
	model->beginModification(parent? static_cast<Model::Node*> (parent) : longMethod, "Adding a long method.");
	if (!longMethod)
	{
		longMethod = new Method();
		parent->methods()->append(longMethod);
	}

	longMethod->setName("aLongTestMethod");
	FormalResult* result  = new FormalResult();
	result->setType(new PrimitiveType(PrimitiveType::INT));
	longMethod->results()->append(result);
	FormalArgument* arg1 = new FormalArgument();
	arg1->setType(new PrimitiveType(PrimitiveType::INT));
	arg1->setName("x");
	longMethod->arguments()->append(arg1);
	FormalArgument* arg2 = new FormalArgument();
	arg2->setType(new PrimitiveType(PrimitiveType::INT));
	arg2->setName("y");
	longMethod->arguments()->append(arg2);
	FormalArgument* arg3 = new FormalArgument();
	arg3->setType(new PrimitiveType(PrimitiveType::FLOAT));
	arg3->setName("epsilon");
	longMethod->arguments()->append(arg3);

	VariableDeclaration* var1 = new VariableDeclaration();
	longMethod->items()->append(var1);
	var1->setName("var1");
	var1->setType(new PrimitiveType(PrimitiveType::INT));

	VariableDeclaration* var2 = new VariableDeclaration();
	longMethod->items()->append(var2);
	var2->setName("var2");
	var2->setType(new PrimitiveType(PrimitiveType::LONG));
	var2->setInitialValue(new IntegerLiteral(42));

	VariableDeclaration* var3 = new VariableDeclaration();
	longMethod->items()->append(var3);
	var3->setName("var3");
	var3->setType(new PrimitiveType(PrimitiveType::BOOLEAN));
	var3->setInitialValue(new BooleanLiteral(true));

	VariableDeclaration* var4 = new VariableDeclaration();
	longMethod->items()->append(var4);
	var4->setName("var4");
	var4->setType(new PrimitiveType(PrimitiveType::CHAR));
	var4->setInitialValue(new CharacterLiteral('r'));

	VariableDeclaration* var5 = new VariableDeclaration();
	longMethod->items()->append(var5);
	var5->setName("var5");
	var5->setType(new PrimitiveType(PrimitiveType::DOUBLE));
	var5->setInitialValue(new FloatLiteral(112311096123));

	VariableDeclaration* var6 = new VariableDeclaration();
	longMethod->items()->append(var6);
	var6->setName("var6");
	var6->setType(new PrimitiveType(PrimitiveType::UNSIGNED_LONG));
	var6->setInitialValue(new IntegerLiteral(1000));

	VariableDeclaration* var7 = new VariableDeclaration();
	longMethod->items()->append(var7);
	var7->setName("var7");
	var7->setType(new PrimitiveType(PrimitiveType::VOID));
	var7->setInitialValue(new NullLiteral());

	VariableDeclaration* var8 = new VariableDeclaration();
	longMethod->items()->append(var8);
	var8->setName("var8");
	var8->setType(new PrimitiveType(PrimitiveType::INT));
	MethodCallExpression* var8Value = new MethodCallExpression();
	var8Value->ref()->set("met:getId");
	var8->setInitialValue(var8Value);

	VariableDeclaration* var9 = new VariableDeclaration();
	longMethod->items()->append(var9);
	var9->setName("var9");
	var9->setType(new PrimitiveType(PrimitiveType::VOID));
	var9->setInitialValue(new ThisExpression());

	VariableDeclaration* var10 = new VariableDeclaration();
	longMethod->items()->append(var10);
	var10->setName("var10");
	var10->setType(new PrimitiveType(PrimitiveType::INT));
	CastExpression* cast = new CastExpression();
	var10->setInitialValue(cast);
	cast->setType(new PrimitiveType(PrimitiveType::INT));
	cast->setExpr(new VariableAccess("local:epsilon"));

	VariableDeclaration* var11 = new VariableDeclaration();
	longMethod->items()->append(var11);
	var11->setName("var11");
	var11->setType(new PrimitiveType(PrimitiveType::VOID));
	NewExpression* var11Value = new NewExpression();
	var11Value->setType(new PrimitiveType(PrimitiveType::INT));
	var11->setInitialValue(var11Value);

	VariableDeclaration* var12 = new VariableDeclaration();
	longMethod->items()->append(var12);
	var12->setName("var12");
	ArrayType* var12Type = new ArrayType();
	var12Type->setType(new PrimitiveType(PrimitiveType::INT));
	var12->setType(var12Type);
	NewExpression* var12Value = new NewExpression();
	var12Value->setType(new PrimitiveType(PrimitiveType::INT));
	var12Value->setAmount(new IntegerLiteral(5));
	var12->setInitialValue(var12Value);

	VariableDeclaration* var13 = new VariableDeclaration();
	longMethod->items()->append(var13);
	var13->setName("var13");
	var13->setType(new PrimitiveType(PrimitiveType::BOOLEAN));
	UnaryOperation* uOp1 = new UnaryOperation();
	var13->setInitialValue(uOp1);
	uOp1->setOp(UnaryOperation::NOT);
	BooleanLiteral* var13val = new BooleanLiteral();
	var13val->setValue(false);
	uOp1->setOperand(var13val);

	VariableDeclaration* var14 = new VariableDeclaration();
	longMethod->items()->append(var14);
	var14->setName("var14");
	var14->setType(new PrimitiveType(PrimitiveType::INT));
	UnaryOperation* uOp2 = new UnaryOperation();
	var14->setInitialValue(uOp2);
	uOp2->setOp(UnaryOperation::INCREMENT);
	uOp2->setOperand(new IntegerLiteral(10));

	VariableDeclaration* var15 = new VariableDeclaration();
	longMethod->items()->append(var15);
	var15->setName("var15");
	var15->setType(new PrimitiveType(PrimitiveType::INT));
	BinaryOperation* binOp1 = new BinaryOperation();
	var15->setInitialValue(binOp1);
	binOp1->setOp(BinaryOperation::PLUS);
	binOp1->setLeft(new IntegerLiteral(41));
	binOp1->setRight(new IntegerLiteral(1));

	VariableDeclaration* var16 = new VariableDeclaration();
	longMethod->items()->append(var16);
	var16->setName("var16");
	var16->setType(new PrimitiveType(PrimitiveType::BOOLEAN));
	BinaryOperation* binOp2 = new BinaryOperation();
	var16->setInitialValue(binOp2);
	binOp2->setOp(BinaryOperation::LESS_EQUALS);
	binOp2->setLeft(new IntegerLiteral(41));
	binOp2->setRight(new IntegerLiteral(1));

	VariableDeclaration* var17 = new VariableDeclaration();
	longMethod->items()->append(var17);
	var17->setName("var17");
	ArrayType* var17Type = new ArrayType();
	var17->setType(var17Type);
	NamedType* var17ElementType = new NamedType();
	var17Type->setType(var17ElementType);
	var17ElementType->type()->ref()->set("class:String");
	ArrayInitializer* outterArrayInit = new ArrayInitializer();
	var17->setInitialValue(outterArrayInit);
	outterArrayInit->values()->append(new StringLiteral("this"));
	outterArrayInit->values()->append(new StringLiteral("looks"));
	outterArrayInit->values()->append(new StringLiteral("good"));

	VariableDeclaration* var18 = new VariableDeclaration();
	longMethod->items()->append(var18);
	var18->setName("var18");
	ArrayType* var18Type = new ArrayType();
	var18->setType(var18Type);
	ArrayType* var18TypeType = new ArrayType();
	var18Type->setType(var18TypeType);
	var18TypeType->setType(new PrimitiveType(PrimitiveType::INT));
	ArrayInitializer* matrixArrayInit = new ArrayInitializer();
	var18->setInitialValue(matrixArrayInit);
	ArrayInitializer* col1Init = new ArrayInitializer();
	matrixArrayInit->values()->append(col1Init);
	ArrayInitializer* col2Init = new ArrayInitializer();
	matrixArrayInit->values()->append(col2Init);
	ArrayInitializer* col3Init = new ArrayInitializer();
	matrixArrayInit->values()->append(col3Init);
	col1Init->values()->append(new IntegerLiteral(1));
	col1Init->values()->append(new IntegerLiteral(2));
	col1Init->values()->append(new IntegerLiteral(3));
	col2Init->values()->append(new IntegerLiteral(4));
	col2Init->values()->append(new IntegerLiteral(5));
	col2Init->values()->append(new IntegerLiteral(6));
	col3Init->values()->append(new IntegerLiteral(7));
	col3Init->values()->append(new IntegerLiteral(8));
	col3Init->values()->append(new IntegerLiteral(9));

	IfStatement* ifs = new IfStatement();
	longMethod->items()->append(ifs);
	BinaryOperation* ifCond = new BinaryOperation();
	ifs->setCondition(ifCond);
	ifCond->setLeft(new VariableAccess("local:var14"));
	ifCond->setOp(BinaryOperation::NOT_EQUALS);
	ifCond->setRight(new IntegerLiteral(10));
	UnaryOperation* thenBranch = new UnaryOperation();
	ifs->thenBranch()->append(thenBranch);
	thenBranch->setOp(UnaryOperation::INCREMENT);
	thenBranch->setOperand(new VariableAccess("local:var14"));
	UnaryOperation* elseBranch = new UnaryOperation();
	ifs->elseBranch()->append(elseBranch);
	elseBranch->setOp(UnaryOperation::DECREMENT);
	elseBranch->setOperand(new VariableAccess("local:var14"));

	Block* block = new Block();
	longMethod->items()->append(block);
	AssignmentStatement* assign = new AssignmentStatement();
	block->items()->append(assign);
	assign->setLeft(new VariableAccess("local:var1"));
	assign->setOp(AssignmentStatement::ASSIGN);
	BinaryOperation* arrayAccess = new BinaryOperation();
	assign->setRight(arrayAccess);
	arrayAccess->setLeft(new VariableAccess("global:someArray"));
	arrayAccess->setOp(BinaryOperation::ARRAY_INDEX);
	arrayAccess->setRight(new IntegerLiteral(4));

	LoopStatement* loop = new LoopStatement();
	longMethod->items()->append(loop);
	VariableDeclaration* initStep = new VariableDeclaration();
	loop->setInitStep(initStep);
	initStep->setType(new PrimitiveType(PrimitiveType::INT));
	initStep->setName("i");
	initStep->setInitialValue(new IntegerLiteral(0));
	BinaryOperation* loopCondition = new BinaryOperation();
	loop->setCondition(loopCondition);
	loopCondition->setLeft(new VariableAccess("local:i"));
	loopCondition->setOp(BinaryOperation::LESS);
	loopCondition->setRight(new VariableAccess("local:x"));
	AssignmentStatement* updateStep = new AssignmentStatement();
	loop->setUpdateStep(updateStep);
	updateStep->setLeft(new VariableAccess("local:i"));
	updateStep->setOp(AssignmentStatement::PLUS_ASSIGN);
	updateStep->setRight(new IntegerLiteral(1));
	AssignmentStatement* loopBodyAssignment = new AssignmentStatement();
	loop->body()->append(loopBodyAssignment);
	loopBodyAssignment->setLeft(new VariableAccess("local:var14"));
	loopBodyAssignment->setOp(AssignmentStatement::TIMES_ASSIGN);
	loopBodyAssignment->setRight(new IntegerLiteral(2));
	loop->body()->append(new ContinueStatement());
	loop->body()->append(new BreakStatement());


	ForEachStatement* forEach = new ForEachStatement();
	longMethod->items()->append(forEach);
	forEach->setVarName("elem");
	forEach->setVarType(new PrimitiveType(PrimitiveType::UNSIGNED_INT));
	VariableAccess* forEachCollection = new VariableAccess();
	forEachCollection->ref()->set("global:SomeCollection");
	forEach->setCollection(forEachCollection);
	AssignmentStatement* assignEach = new AssignmentStatement();
	forEach->body()->append(assignEach);
	assignEach->setLeft(new VariableAccess("local:var1"));
	assignEach->setOp(AssignmentStatement::DIVIDE_ASSIGN);
	assignEach->setRight(new VariableAccess("loop:elem"));

	ReturnStatement* longMethodReturn = new ReturnStatement();
	longMethod->items()->append(longMethodReturn);
	longMethodReturn->values()->append(new IntegerLiteral(42));

	if (parent && parent->parent())
		longMethod->extension<Position>()->setY(100);
	else
		longMethod->extension<Position>()->setX(400);

	model->endModification();
	return longMethod;
}

Method* addFactorial(Model::Model* model, Class* parent)
{
	Method* factorial = nullptr;

	if (!parent) factorial = dynamic_cast<Method*> (model->createRoot("Method"));
	model->beginModification(parent? static_cast<Model::Node*> (parent) : factorial, "Adding a factorial method.");
	if (!factorial)
	{
		factorial = new Method();
		parent->methods()->append(factorial);
	}

	factorial->setName("factorial");
	FormalResult* factorialResult = new FormalResult();
	factorial->results()->append(factorialResult);
	factorialResult->setType(new PrimitiveType(PrimitiveType::INT));
	FormalArgument* factorialArgument = new FormalArgument();
	factorial->arguments()->append(factorialArgument);
	factorialArgument->setType(new PrimitiveType(PrimitiveType::INT));
	factorialArgument->setName("x");

	VariableDeclaration* res = new VariableDeclaration();
	factorial->items()->append(res);
	res->setName("result");
	res->setType(new PrimitiveType(PrimitiveType::INT));
	res->setInitialValue(new IntegerLiteral(1));

	// Condition
	IfStatement* ifs = new IfStatement();
	factorial->items()->append(ifs);
	BinaryOperation* ifCond = new BinaryOperation();
	ifs->setCondition(ifCond);
	ifCond->setLeft(new VariableAccess("local:x"));
	ifCond->setOp(BinaryOperation::GREATER_EQUALS);
	ifCond->setRight(new IntegerLiteral(0));

	// Then
	LoopStatement* loop = new LoopStatement();
	ifs->thenBranch()->append(loop);
	VariableDeclaration* initStep = new VariableDeclaration();
	loop->setInitStep(initStep);
	initStep->setType(new PrimitiveType(PrimitiveType::INT));
	initStep->setName("i");
	initStep->setInitialValue(new IntegerLiteral(1));
	BinaryOperation* loopCondition = new BinaryOperation();
	loop->setCondition(loopCondition);
	loopCondition->setLeft(new VariableAccess("local:i"));
	loopCondition->setOp(BinaryOperation::LESS_EQUALS);
	loopCondition->setRight(new VariableAccess("local:x"));
	AssignmentStatement* updateStep = new AssignmentStatement();
	loop->setUpdateStep(updateStep);
	updateStep->setLeft(new VariableAccess("local:i"));
	updateStep->setOp(AssignmentStatement::PLUS_ASSIGN);
	updateStep->setRight(new IntegerLiteral(1));
	AssignmentStatement* loopBodyAssignment = new AssignmentStatement();
	loop->body()->append(loopBodyAssignment);
	loopBodyAssignment->setLeft(new VariableAccess("local:result"));
	loopBodyAssignment->setOp(AssignmentStatement::TIMES_ASSIGN);
	loopBodyAssignment->setRight(new VariableAccess("local:i"));

	// Else
	AssignmentStatement* elseBranch = new AssignmentStatement();
	ifs->elseBranch()->append(elseBranch);

	elseBranch->setLeft(new VariableAccess("local:result"));
	elseBranch->setOp(AssignmentStatement::ASSIGN);
	elseBranch->setRight(new IntegerLiteral(-1));

	// Return
	ReturnStatement* factorialReturn = new ReturnStatement();
	factorial->items()->append(factorialReturn);
	factorialReturn->values()->append(new VariableAccess("local:result"));

	factorial->extension<Position>()->setY(860);

	model->endModification();
	return factorial;
}

TEST(OOVisualization, JavaLibraryAndHelloWorldTest)
{
	Scene* scene = new Scene();

	////////////////////////////////////////////////// Create Model
	Model::Model* model = new Model::Model();
	Project* prj = nullptr;

	// Create project
	prj = dynamic_cast<Project*> (model->createRoot("Project"));
	model->beginModification(prj, "build simple java library and a hello world app");
	prj->setName("HelloWorld");
	model->endModification();

	Library* java = nullptr;
	java = addJavaLibrary(model, prj);

	// Build a simple HelloWorld Application
	Class* hello = nullptr;
	hello = addHelloWorld(model, prj);

//	// Add a second method
	Method* longMethod = nullptr;
	longMethod = addLongMethod(model, hello);

//	// Add a third method
	Method* factorial = nullptr;
	factorial = addFactorial(model, hello);

	////////////////////////////////////////////////// Set Scene
	Model::Node* top_level = nullptr;
	if (prj) top_level = prj;
	else if(hello) top_level = hello;
	else if(java) top_level = java;
	else if (longMethod) top_level = longMethod;
	else top_level = factorial;

	scene->addTopLevelItem( scene->defaultRenderer()->render(nullptr, top_level) );
	scene->scheduleUpdate();
	scene->listenToModel(model);

	// Create view
	MainView* view = new MainView(scene);

	CHECK_CONDITION(view != nullptr);
}

}
