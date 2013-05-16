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

#include "oovisualization.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "MethodAddOn.h"
#include "../src/declarations/VMethod.h"

#include "OOModel/src/allOOModelNodes.h"

#include "InteractionBase/src/autocomplete/AutoComplete.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/renderer/ModelRenderer.h"
#include "VisualizationBase/src/items/VExtendable.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/node_extensions/Position.h"
#include "VisualizationBase/src/items/RootItem.h"

#include "ModelBase/src/model/Model.h"

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
	ArrayTypeExpression* mainArgType = new ArrayTypeExpression();
	mainArgs->setTypeExpression(mainArgType);
	ClassTypeExpression* mainArgElementType = new ClassTypeExpression();
	mainArgType->setTypeExpression(mainArgElementType);
	mainArgElementType->typeExpression()->ref()->setName("String");

	ExpressionStatement* callPrintlnSt = new ExpressionStatement();
	MethodCallExpression* callPrintln = new MethodCallExpression("println",
			new ReferenceExpression("out", new ReferenceExpression("System", new ReferenceExpression("Java"))));
	callPrintln->arguments()->append(new StringLiteral("Hello World"));
	callPrintlnSt->setExpression(callPrintln);
	main->items()->append(callPrintlnSt);

	model->endModification();
	return hello;
}

Class* addGeneric(Model::Model* model, Project* parent)
{
	Class* gen = nullptr;

	if (!parent) gen = dynamic_cast<Class*> (model->createRoot("Class"));
	model->beginModification(parent ? static_cast<Model::Node*> (parent) :gen, "Adding a generic class.");
	if (!gen)
	{
		gen = new Class();
		parent->classes()->append(gen);
	}

	gen->setName("Generic");
	gen->setVisibility(Visibility::PUBLIC);
	gen->typeArguments()->append(new FormalTypeArgument("P"));
	gen->typeArguments()->append(new FormalTypeArgument("Q", new ReferenceExpression("P")));
	gen->typeArguments()->append(new FormalTypeArgument("R", nullptr, new ReferenceExpression("P")));

	// Add some fields
	gen->fields()->append(new Field("index", new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT),
			new IntegerLiteral(42)));
	gen->fields()->append(new Field("data", new ClassTypeExpression(new ReferenceExpression("P"))));

	Method* foo = new Method();
	gen->methods()->append(foo);
	foo->setName("foo");
	foo->setVisibility(Visibility::PUBLIC);
	foo->setStorageSpecifier(StorageSpecifier::INSTANCE_VARIABLE);
	foo->typeArguments()->append(new FormalTypeArgument("T"));

	Method* bar = new Method();
	gen->methods()->append(bar);
	bar->setName("bar");
	bar->setVisibility(Visibility::PUBLIC);
	bar->setStorageSpecifier(StorageSpecifier::CLASS_VARIABLE);
	bar->typeArguments()->append(new FormalTypeArgument("S"));
	bar->typeArguments()->append(new FormalTypeArgument("U", new ReferenceExpression("S")));
	bar->typeArguments()->append(new FormalTypeArgument("V", nullptr, new ReferenceExpression("S")));

	Method* foobar = new Method();
	gen->methods()->append(foobar);
	foobar->setName("foobar");
	foobar->setVisibility(Visibility::PUBLIC);
	foobar->setStorageSpecifier(StorageSpecifier::INSTANCE_VARIABLE);

	VariableDeclarationExpression* var = new VariableDeclarationExpression("var");
	foobar->items()->append(new ExpressionStatement(var));
	ClassTypeExpression* varType = new ClassTypeExpression();
	var->decl()->setTypeExpression(varType);
	varType->setTypeExpression(new ReferenceExpression("Generic"));
	varType->typeExpression()->typeArguments()->append(new ReferenceExpression("A"));
	varType->typeExpression()->typeArguments()->append(new ReferenceExpression("B"));
	varType->typeExpression()->typeArguments()->append(new ReferenceExpression("C"));

	ExpressionStatement* callFooSt = new ExpressionStatement();
	MethodCallExpression* callFoo = new MethodCallExpression("foo");
	callFoo->ref()->typeArguments()->append(new ReferenceExpression("A"));
	callFooSt->setExpression(callFoo);
	foobar->items()->append(callFooSt);

	ExpressionStatement* callBarSt = new ExpressionStatement();
	MethodCallExpression* callBar = new MethodCallExpression("bar");
	callBar->ref()->typeArguments()->append(new ReferenceExpression("A"));
	callBar->ref()->typeArguments()->append(new ReferenceExpression("B"));
	callBar->ref()->typeArguments()->append(new ReferenceExpression("C"));
	callBarSt->setExpression(callBar);
	foobar->items()->append(callBarSt);

	// Set positions
	gen->extension<Position>()->setX(460);
	gen->extension<Position>()->setY(300);
	bar->extension<Position>()->setY(80);
	foobar->extension<Position>()->setY(140);

	model->endModification();
	return gen;
}

Class* addAnnotatedWithFriends(Model::Model* model, Project* parent)
{
	Class* ann = nullptr;

	if (!parent) ann = dynamic_cast<Class*> (model->createRoot("Class"));
	model->beginModification(parent ? static_cast<Model::Node*> (parent) :ann, "Adding an annotated class.");
	if (!ann)
	{
		ann = new Class();
		parent->classes()->append(ann);
	}

	ann->setName("AnnotatedWithFriends");
	ann->setVisibility(Visibility::PUBLIC);
	ann->annotations()->append( new ExpressionStatement(new ReferenceExpression("SomeAnnotation")));

	// Add some friends classes
	ann->friends()->append(new ReferenceExpression("Generic"));
	ann->friends()->append(new ReferenceExpression("System", new ReferenceExpression("Java")));

	// Add methods
	Method* foo = new Method("foo", Visibility::PUBLIC, StorageSpecifier::INSTANCE_VARIABLE);
	ann->methods()->append(foo);
	foo->annotations()->append(new ExpressionStatement(new ReferenceExpression("SomeAnnotation2")));
	foo->annotations()->append(new ExpressionStatement(new ReferenceExpression("SomeAnnotation3")));

	VariableDeclarationExpression* var = new VariableDeclarationExpression("bodyVar");
	foo->items()->append(new ExpressionStatement(var));
	var->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	var->decl()->setInitialValue(new IntegerLiteral(42));

	// Set positions
	ann->extension<Position>()->setX(460);
	ann->extension<Position>()->setY(620);

	model->endModification();
	return ann;
}

Class* addEnumeration(Model::Model* model, Project* parent)
{
	Class* en = nullptr;

	if (!parent) en = dynamic_cast<Class*> (model->createRoot("Class"));
	model->beginModification(parent ? static_cast<Model::Node*> (parent) :en, "Adding an enumeration class.");
	if (!en)
	{
		en = new Class();
		parent->classes()->append(en);
	}

	en->setName("Colors");
	en->setVisibility(Visibility::PUBLIC);
	en->enumerators()->append( new Enumerator("RED"));
	en->enumerators()->append( new Enumerator("GREEN"));
	en->enumerators()->append( new Enumerator("BLUE", new IntegerLiteral(5)));

	// Set positions
	en->extension<Position>()->setX(460);
	en->extension<Position>()->setY(880);

	model->endModification();
	return en;
}

Module* addLambda()
{
	auto mod = new Module("Lambda");
	mod->fields()->append(new Field("common",
			new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT)));

	auto iUnary = new Class("IUnary", Visibility::PUBLIC);
	mod->classes()->append(iUnary);
	auto unMet = new Method("op");
	iUnary->methods()->append(unMet);
	unMet->results()->append(new FormalResult("",
			new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT)));
	unMet->arguments()->append(new FormalArgument("x",
				new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT)));

	auto iBinary = new Class("IBinary", Visibility::PUBLIC);
	mod->classes()->append(iBinary);
	auto binMet = new Method("op");
	iBinary->methods()->append(binMet);
	binMet->results()->append(new FormalResult("",
			new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT)));
	binMet->arguments()->append(new FormalArgument("x",
				new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT)));
	binMet->arguments()->append(new FormalArgument("y",
				new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT)));

	auto iNoRet = new Class("INoReturn", Visibility::PUBLIC);
	mod->classes()->append(iNoRet);
	auto noRetMet = new Method("op");
	iNoRet->methods()->append(noRetMet);
	noRetMet->arguments()->append(new FormalArgument("x",
				new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT)));

	auto test = new Class("LambdaTest", Visibility::PUBLIC);
	mod->classes()->append(test);

	auto acceptUnary =  new Method("unary");
	test->methods()->append(acceptUnary);
	acceptUnary->arguments()->append(new FormalArgument("x",
					new ClassTypeExpression(new ReferenceExpression("IUnary"))));

	auto acceptBinary =  new Method("binary");
	test->methods()->append(acceptBinary);
	acceptBinary->arguments()->append(new FormalArgument("x",
					new ClassTypeExpression(new ReferenceExpression("IBinary"))));

	auto acceptNoReturn =  new Method("noreturn");
	test->methods()->append(acceptNoReturn);
	acceptNoReturn->arguments()->append(new FormalArgument("x",
					new ClassTypeExpression(new ReferenceExpression("INoReturn"))));

	auto testMet = new Method("test");
	test->methods()->append(testMet);

	auto callUnary = new MethodCallExpression("unary");
	testMet->items()->append(new ExpressionStatement(callUnary));
	auto le = new LambdaExpression();
	callUnary->arguments()->append(le);
	le->arguments()->append(new FormalArgument("x",
			new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT)));
	le->body()->append(new ReturnStatement(new BinaryOperation(BinaryOperation::PLUS,
			new ReferenceExpression("x"), new IntegerLiteral(1))));

	auto callBinary= new MethodCallExpression("binary");
	testMet->items()->append(new ExpressionStatement(callBinary));
	le = new LambdaExpression();
	callBinary->arguments()->append(le);
	le->arguments()->append(new FormalArgument("x",
			new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT)));
	le->arguments()->append(new FormalArgument("y",
			new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT)));
	le->body()->append(new ReturnStatement(new BinaryOperation(BinaryOperation::PLUS,
			new ReferenceExpression("x"), new ReferenceExpression("y"))));

	auto callNoRet= new MethodCallExpression("noreturn");
	testMet->items()->append(new ExpressionStatement(callNoRet));
	le = new LambdaExpression();
	callNoRet->arguments()->append(le);
	le->arguments()->append(new FormalArgument("x",
			new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT)));
	auto someOpCall = new MethodCallExpression("someOp");
	someOpCall->arguments()->append(new ReferenceExpression("x"));
	le->body()->append(new ExpressionStatement(someOpCall));

	// Positions
	mod->extension<Position>()->set(900,300);
	return mod;
}

Class* addInner()
{
	Class* outer = new Class("Outer", Visibility::PUBLIC);

	outer->classes()->append( new Class("Inner1"));
	outer->classes()->append( new Class("Inner2"));

	// Set positions
	outer->extension<Position>()->set(900,760);

	return outer;
}

Project* addJavaLibrary(Model::Model* model, Project* parent)
{
	Project* java = nullptr;

	if (!parent) java = dynamic_cast<Project*> (model->createRoot("Project"));
	model->beginModification(parent? static_cast<Model::Node*> (parent) :java, "Adding a java library.");
	if (!java)
	{
		java = new Project();
		parent->projects()->append( java );
	}

	java->setName("Java");

	Class* string = new Class();
	java->classes()->append(string);
	string->setName("String");
	string->setVisibility(Visibility::PUBLIC);
	ClassTypeExpression* type = new ClassTypeExpression();
	string->baseClasses()->append(type);
	type->typeExpression()->ref()->setName("Object");

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
	ClassTypeExpression* argType = new ClassTypeExpression();
	arg->setTypeExpression(argType);
	argType->typeExpression()->ref()->setName("String");

	Class* system = new Class();
	java->classes()->append(system);
	system->setName("System");
	system->setVisibility(Visibility::PUBLIC);
	Field* out = new Field();
	system->fields()->append(out);
	out->setName("out");
	out->setVisibility(Visibility::PUBLIC);
	out->setStorageSpecifier(StorageSpecifier::CLASS_VARIABLE);
	ClassTypeExpression* outtype = new ClassTypeExpression();
	out->setTypeExpression(outtype);
	outtype->typeExpression()->ref()->setName("PrintStream");
	ReferenceExpression* prefix = new ReferenceExpression();
	outtype->typeExpression()->setPrefix(prefix);
	prefix->ref()->setName("io");

	// Set positions
	java->extension<Position>()->setX(460);
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
	result->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	longMethod->results()->append(result);
	FormalArgument* arg1 = new FormalArgument();
	arg1->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	arg1->setName("x");
	longMethod->arguments()->append(arg1);
	FormalArgument* arg2 = new FormalArgument();
	arg2->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	arg2->setName("y");
	longMethod->arguments()->append(arg2);
	FormalArgument* arg3 = new FormalArgument();
	arg3->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::FLOAT));
	arg3->setName("epsilon");
	longMethod->arguments()->append(arg3);

	auto var0 = new VariableDeclarationExpression("pSystem");
	var0->decl()->setTypeExpression(new PointerTypeExpression(new ClassTypeExpression(
			new ReferenceExpression("System", new ReferenceExpression("Java")))));
	longMethod->items()->append(var0);
	longMethod->items()->append(new ExpressionStatement(
			new ReferenceExpression("out", new ReferenceExpression("pSystem"))));


	VariableDeclarationExpression* var1 = new VariableDeclarationExpression("var1");
	longMethod->items()->append(new ExpressionStatement(var1));
	var1->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));

	VariableDeclarationExpression* var2 = new VariableDeclarationExpression("var2");
	longMethod->items()->append(new ExpressionStatement(var2));
	var2->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::LONG));
	var2->decl()->setInitialValue(new IntegerLiteral(42));

	VariableDeclarationExpression* var3 = new VariableDeclarationExpression("var3");
	longMethod->items()->append(new ExpressionStatement(var3));
	var3->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN));
	var3->decl()->setInitialValue(new BooleanLiteral(true));

	VariableDeclarationExpression* var4 = new VariableDeclarationExpression("var4");
	longMethod->items()->append(new ExpressionStatement(var4));
	var4->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::CHAR));
	var4->decl()->setInitialValue(new CharacterLiteral('r'));

	VariableDeclarationExpression* var5 = new VariableDeclarationExpression("var5");
	longMethod->items()->append(new ExpressionStatement(var5));
	var5->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::DOUBLE));
	var5->decl()->setInitialValue(new FloatLiteral(112311096123));

	VariableDeclarationExpression* var6 = new VariableDeclarationExpression("var6");
	longMethod->items()->append(new ExpressionStatement(var6));
	var6->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG));
	var6->decl()->setInitialValue(new IntegerLiteral(1000));

	VariableDeclarationExpression* var7 = new VariableDeclarationExpression("var7");
	longMethod->items()->append(new ExpressionStatement(var7));
	var7->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	var7->decl()->setInitialValue(new NullLiteral());

	VariableDeclarationExpression* var8 = new VariableDeclarationExpression("var8");
	longMethod->items()->append(new ExpressionStatement(var8));
	var8->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	MethodCallExpression* var8Value = new MethodCallExpression();
	var8Value->ref()->setName("getId");
	var8->decl()->setInitialValue(var8Value);

	VariableDeclarationExpression* var9 = new VariableDeclarationExpression("var9");
	longMethod->items()->append(new ExpressionStatement(var9));
	var9->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	var9->decl()->setInitialValue(new ThisExpression());

	VariableDeclarationExpression* var10 = new VariableDeclarationExpression("var10");
	longMethod->items()->append(new ExpressionStatement(var10));
	var10->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	CastExpression* cast = new CastExpression();
	var10->decl()->setInitialValue(cast);
	cast->setType(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	cast->setExpr(new ReferenceExpression("epsilon"));

	VariableDeclarationExpression* var11 = new VariableDeclarationExpression("var11");
	longMethod->items()->append(new ExpressionStatement(var11));
	var11->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	NewExpression* var11Value = new NewExpression();
	var11Value->setNewType(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	var11->decl()->setInitialValue(var11Value);

	VariableDeclarationExpression* var12 = new VariableDeclarationExpression("var12");
	longMethod->items()->append(new ExpressionStatement(var12));
	ArrayTypeExpression* var12Type = new ArrayTypeExpression();
	var12Type->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	var12->decl()->setTypeExpression(var12Type);
	NewExpression* var12Value = new NewExpression();
	var12Value->setNewType(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	var12Value->setAmount(new IntegerLiteral(5));
	var12->decl()->setInitialValue(var12Value);

	VariableDeclarationExpression* var13 = new VariableDeclarationExpression("var13");
	longMethod->items()->append(new ExpressionStatement(var13));
	var13->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN));
	UnaryOperation* uOp1 = new UnaryOperation();
	var13->decl()->setInitialValue(uOp1);
	uOp1->setOp(UnaryOperation::NOT);
	BooleanLiteral* var13val = new BooleanLiteral();
	var13val->setValue(false);
	uOp1->setOperand(var13val);

	VariableDeclarationExpression* var14 = new VariableDeclarationExpression("var14");
	longMethod->items()->append(new ExpressionStatement(var14));
	var14->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	UnaryOperation* uOp2 = new UnaryOperation();
	var14->decl()->setInitialValue(uOp2);
	uOp2->setOp(UnaryOperation::POSTINCREMENT);
	uOp2->setOperand(new IntegerLiteral(10));

	VariableDeclarationExpression* var15 = new VariableDeclarationExpression("var15");
	longMethod->items()->append(new ExpressionStatement(var15));
	var15->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	BinaryOperation* binOp0 = new BinaryOperation();
	var15->decl()->setInitialValue(binOp0);
	binOp0->setOp(BinaryOperation::DIVIDE);
	UnaryOperation* uOp3 = new UnaryOperation();
	binOp0->setLeft(uOp3);
	binOp0->setRight(new IntegerLiteral(3));
	uOp3->setOp(UnaryOperation::PARENTHESIS);
	BinaryOperation* binOp1 = new BinaryOperation();
	uOp3->setOperand(binOp1);
	binOp1->setOp(BinaryOperation::PLUS);
	binOp1->setLeft(new IntegerLiteral(41));
	binOp1->setRight(new IntegerLiteral(1));

	VariableDeclarationExpression* var16 = new VariableDeclarationExpression("var16");
	longMethod->items()->append(new ExpressionStatement(var16));
	var16->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN));
	BinaryOperation* binOp2 = new BinaryOperation();
	var16->decl()->setInitialValue(binOp2);
	binOp2->setOp(BinaryOperation::LESS_EQUALS);
	binOp2->setLeft(new IntegerLiteral(41));
	binOp2->setRight(new IntegerLiteral(1));

	VariableDeclarationExpression* var17 = new VariableDeclarationExpression("var17");
	longMethod->items()->append(new ExpressionStatement(var17));
	ArrayTypeExpression* var17Type = new ArrayTypeExpression();
	var17->decl()->setTypeExpression(var17Type);
	ClassTypeExpression* var17ElementType = new ClassTypeExpression();
	var17Type->setTypeExpression(var17ElementType);
	var17ElementType->typeExpression()->ref()->setName("String");
	ArrayInitializer* outterArrayInit = new ArrayInitializer();
	var17->decl()->setInitialValue(outterArrayInit);
	outterArrayInit->values()->append(new StringLiteral("this"));
	outterArrayInit->values()->append(new StringLiteral("looks"));
	outterArrayInit->values()->append(new StringLiteral("good"));

	VariableDeclarationExpression* var18 = new VariableDeclarationExpression("var18");
	longMethod->items()->append(new ExpressionStatement(var18));
	ArrayTypeExpression* var18Type = new ArrayTypeExpression();
	var18->decl()->setTypeExpression(var18Type);
	ArrayTypeExpression* var18TypeType = new ArrayTypeExpression();
	var18Type->setTypeExpression(var18TypeType);
	var18TypeType->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	ArrayInitializer* matrixArrayInit = new ArrayInitializer();
	var18->decl()->setInitialValue(matrixArrayInit);
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

	VariableDeclarationExpression* var19 = new VariableDeclarationExpression("var19");
	longMethod->items()->append(new ExpressionStatement(var19));
	var19->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	ConditionalExpression* ce = new ConditionalExpression();
	var19->decl()->setInitialValue(ce);
	BinaryOperation* binOp3 = new BinaryOperation();
	ce->setCondition(binOp3);
	binOp3->setOp(BinaryOperation::LESS_EQUALS);
	binOp3->setLeft(new IntegerLiteral(6));
	binOp3->setRight(new IntegerLiteral(10));
	ce->setTrueExpression(new IntegerLiteral(42));
	ce->setFalseExpression(new IntegerLiteral(0));

	auto throwExpr = new ThrowExpression();
	longMethod->items()->append(new ExpressionStatement(throwExpr));
	throwExpr->setExpr(new ReferenceExpression("AnException"));

	auto deleteExpr = new DeleteExpression(false);
	deleteExpr->setExpr(new ReferenceExpression("pSystem"));
	longMethod->items()->append(new ExpressionStatement(deleteExpr));

	auto deleteArrayExpr = new DeleteExpression(true);
	deleteArrayExpr->setExpr(new ReferenceExpression("pSystem"));
	longMethod->items()->append(new ExpressionStatement(deleteArrayExpr));

	IfStatement* ifs = new IfStatement();
	longMethod->items()->append(ifs);
	BinaryOperation* ifCond = new BinaryOperation();
	ifs->setCondition(ifCond);
	ifCond->setLeft(new ReferenceExpression("var14"));
	ifCond->setOp(BinaryOperation::NOT_EQUALS);
	ifCond->setRight(new IntegerLiteral(10));
	UnaryOperation* thenBranch = new UnaryOperation();
	ifs->thenBranch()->append(thenBranch);
	thenBranch->setOp(UnaryOperation::POSTINCREMENT);
	thenBranch->setOperand(new ReferenceExpression("var14"));
	UnaryOperation* elseBranch = new UnaryOperation();
	ifs->elseBranch()->append(elseBranch);
	elseBranch->setOp(UnaryOperation::POSTDECREMENT);
	elseBranch->setOperand(new ReferenceExpression("var14"));

	Block* block = new Block();
	longMethod->items()->append(block);
	AssignmentExpression* assign = new AssignmentExpression();
	block->items()->append(new ExpressionStatement(assign));
	assign->setLeft(new ReferenceExpression("var1"));
	assign->setOp(AssignmentExpression::ASSIGN);
	BinaryOperation* arrayAccess = new BinaryOperation();
	assign->setRight(arrayAccess);
	arrayAccess->setLeft(new ReferenceExpression("someArray"));
	arrayAccess->setOp(BinaryOperation::ARRAY_INDEX);
	arrayAccess->setRight(new IntegerLiteral(4));

	LoopStatement* loop = new LoopStatement();
	longMethod->items()->append(loop);
	VariableDeclarationExpression* initStep = new VariableDeclarationExpression("i");
	loop->setInitStep(initStep);
	initStep->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	initStep->decl()->setInitialValue(new IntegerLiteral(0));
	BinaryOperation* loopCondition = new BinaryOperation();
	loop->setCondition(loopCondition);
	loopCondition->setLeft(new ReferenceExpression("i"));
	loopCondition->setOp(BinaryOperation::LESS);
	loopCondition->setRight(new ReferenceExpression("x"));
	AssignmentExpression* updateStep = new AssignmentExpression();
	loop->setUpdateStep(updateStep);
	updateStep->setLeft(new ReferenceExpression("i"));
	updateStep->setOp(AssignmentExpression::PLUS_ASSIGN);
	updateStep->setRight(new IntegerLiteral(1));
	AssignmentExpression* loopBodyAssignment = new AssignmentExpression();
	loop->body()->append(new ExpressionStatement(loopBodyAssignment));
	loopBodyAssignment->setLeft(new ReferenceExpression("var14"));
	loopBodyAssignment->setOp(AssignmentExpression::TIMES_ASSIGN);
	loopBodyAssignment->setRight(new IntegerLiteral(2));
	loop->body()->append(new ContinueStatement());
	loop->body()->append(new BreakStatement());


	ForEachStatement* forEach = new ForEachStatement();
	longMethod->items()->append(forEach);
	forEach->setVarName("elem");
	forEach->setVarType(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT));
	forEach->setCollection(new ReferenceExpression("SomeCollection"));
	AssignmentExpression* assignEach = new AssignmentExpression();
	forEach->body()->append(new ExpressionStatement(assignEach));
	assignEach->setLeft(new ReferenceExpression("var1"));
	assignEach->setOp(AssignmentExpression::DIVIDE_ASSIGN);
	assignEach->setRight(new ReferenceExpression("elem"));

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
	factorialResult->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	FormalArgument* factorialArgument = new FormalArgument();
	factorial->arguments()->append(factorialArgument);
	factorialArgument->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	factorialArgument->setName("x");

	VariableDeclarationExpression* res = new VariableDeclarationExpression("result");
	factorial->items()->append(new ExpressionStatement(res));
	res->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	res->decl()->setInitialValue(new IntegerLiteral(1));

	// Condition
	IfStatement* ifs = new IfStatement();
	factorial->items()->append(ifs);
	BinaryOperation* ifCond = new BinaryOperation();
	ifs->setCondition(ifCond);
	ifCond->setLeft(new ReferenceExpression("x"));
	ifCond->setOp(BinaryOperation::GREATER_EQUALS);
	ifCond->setRight(new IntegerLiteral(0));

	// Then
	LoopStatement* loop = new LoopStatement();
	ifs->thenBranch()->append(loop);
	VariableDeclarationExpression* initStep = new VariableDeclarationExpression("i");
	loop->setInitStep(initStep);
	initStep->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	initStep->decl()->setInitialValue(new IntegerLiteral(1));
	BinaryOperation* loopCondition = new BinaryOperation();
	loop->setCondition(loopCondition);
	loopCondition->setLeft(new ReferenceExpression("i"));
	loopCondition->setOp(BinaryOperation::LESS_EQUALS);
	loopCondition->setRight(new ReferenceExpression("x"));
	AssignmentExpression* updateStep = new AssignmentExpression();
	loop->setUpdateStep(updateStep);
	updateStep->setLeft(new ReferenceExpression("i"));
	updateStep->setOp(AssignmentExpression::PLUS_ASSIGN);
	updateStep->setRight(new IntegerLiteral(1));
	AssignmentExpression* loopBodyAssignment = new AssignmentExpression();
	loop->body()->append(new ExpressionStatement(loopBodyAssignment));
	loopBodyAssignment->setLeft(new ReferenceExpression("result"));
	loopBodyAssignment->setOp(AssignmentExpression::TIMES_ASSIGN);
	loopBodyAssignment->setRight(new ReferenceExpression("i"));

	// Else
	AssignmentExpression* elseBranch = new AssignmentExpression();
	ifs->elseBranch()->append(new ExpressionStatement(elseBranch));

	elseBranch->setLeft(new ReferenceExpression("result"));
	elseBranch->setOp(AssignmentExpression::ASSIGN);
	elseBranch->setRight(new IntegerLiteral(-1));

	// Return
	ReturnStatement* factorialReturn = new ReturnStatement();
	factorial->items()->append(factorialReturn);
	factorialReturn->values()->append(new ReferenceExpression("result"));

	factorial->extension<Position>()->setY(960);

	model->endModification();
	return factorial;
}

TEST(OOVisualization, JavaLibraryAndHelloWorldTest)
{
	////////////////////////////////////////////////// Create Model
	Model::Model* model = new Model::Model();
	Project* prj = nullptr;

	// Create project
	prj = dynamic_cast<Project*> (model->createRoot("Project"));
	model->beginModification(prj, "build simple java library and a hello world app");
	prj->setName("HelloWorld");
	prj->fields()->append(new Field("global",
			new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT)));
	prj->fields()->append(new Field("global2",
			new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG)));
	model->endModification();

	Project* java = nullptr;
	java = addJavaLibrary(model, prj);

	// Build a simple HelloWorld Application
	Class* hello = nullptr;
	hello = addHelloWorld(model, prj);

	// Build a simple Generic Class
	Class* gen = nullptr;
	gen = addGeneric(model, prj);

	// Build a simple Class that has annotations and friends
	Class* ann = nullptr;
	ann = addAnnotatedWithFriends(model, prj);

	// Build a simple enumeration
	Class* en = nullptr;
	en = addEnumeration(model, prj);

//	// Add a second method
	Method* longMethod = nullptr;
	longMethod = addLongMethod(model, hello);

//	// Add a third method
	Method* factorial = nullptr;
	factorial = addFactorial(model, hello);

	prj->beginModification("add lambda module");
	prj->modules()->append(addLambda());
	prj->classes()->append(addInner());
	prj->endModification();

// Add a method Add-on
	VMethod::addAddOn(new MethodAddOn("foo"));

	////////////////////////////////////////////////// Set Scene
	Model::Node* top_level = nullptr;
	if (prj) top_level = prj;
	else if(hello) top_level = hello;
	else if(gen) top_level = gen;
	else if(ann) top_level = ann;
	else if(en) top_level = en;
	else if(java) top_level = java;
	else if (longMethod) top_level = longMethod;
	else top_level = factorial;

	VisualizationManager::instance().mainScene()->addTopLevelItem( new RootItem(top_level));
	VisualizationManager::instance().mainScene()->listenToModel(model);

	CHECK_CONDITION(top_level != nullptr);
}

}
