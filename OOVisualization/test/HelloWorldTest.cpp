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

#include "../src/OOVisualizationPlugin.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "MethodAddOn.h"
#include "../src/declarations/VMethod.h"

#include "OOModel/src/allOOModelNodes.h"

#include "InteractionBase/src/autocomplete/AutoComplete.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/views/MainView.h"
#include "VisualizationBase/src/renderer/ModelRenderer.h"
#include "VisualizationBase/src/items/VComposite.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/node_extensions/Position.h"
#include "VisualizationBase/src/ViewItemManager.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/UsedLibrary.h"

using namespace OOModel;
using namespace Visualization;
using namespace Comments;

namespace OOVisualization {

// The methods below add a certain construct to an existing Manager or node.

void addConstructorAndDestructor(Class* cl)
{

	auto con = new Method{cl->name(), Modifier::Public, Method::MethodKind::Constructor};
	cl->methods()->append(con);
	std::unique_ptr<Position>(con->extension<Position>())->set(0, 1);
	con->memberInitializers()->append(new MemberInitializer{new ReferenceExpression{"Super"}, {new IntegerLiteral{42}}});
	con->memberInitializers()->append(new MemberInitializer{
					new ReferenceExpression{"name"}, {new StringLiteral{"hi"}, new IntegerLiteral{10}}});

	auto des = new Method{"~" + cl->name(), Modifier::Public, Method::MethodKind::Destructor};
	cl->methods()->append(des);
	std::unique_ptr<Position>(des->extension<Position>())->set(0, 2);
}

Class* addHelloWorld(Project* parent)
{
	auto hello = new Class{"HelloWorld", Modifier::Public};
	std::unique_ptr<Position>(hello->extension<Position>())->set(0, 0);
	if (parent) parent->classes()->append(hello);

	hello->subDeclarations()->append(new NameImport{
		new ReferenceExpression{"out", new ReferenceExpression{"System"}}});
	// TODO: BUG: If the two lines below are uncommented this will trigger an infinite loop.
	// hello->subDeclarations()->append(new NameImport{new ReferenceExpression{"Java"}));
	// hello->subDeclarations()->append(new NameImport{new ReferenceExpression{"Java"}));

	Method* main = new Method{"main", Modifier::Public | Modifier::Static};
	std::unique_ptr<Position>(main->extension<Position>())->set(0, 0);
	hello->methods()->append(main);

	FormalArgument* mainArgs = new FormalArgument{};
	main->arguments()->append(mainArgs);
	mainArgs->setName("args");
	ArrayTypeExpression* mainArgType = new ArrayTypeExpression{};
	mainArgs->setTypeExpression(mainArgType);
	ClassTypeExpression* mainArgElementType = new ClassTypeExpression{};
	mainArgType->setTypeExpression(mainArgElementType);
	mainArgElementType->typeExpression()->ref()->setName("String");

	ExpressionStatement* callPrintlnSt = new ExpressionStatement{};
	MethodCallExpression* callPrintln = new MethodCallExpression{"println",
			new ReferenceExpression{"out", new ReferenceExpression{"System"}}};
	callPrintln->arguments()->append(new StringLiteral{"Hello World"});
	callPrintlnSt->setExpression(callPrintln);
	main->items()->append(callPrintlnSt);

	// Add constructors and destructors
	addConstructorAndDestructor(hello);

	return hello;
}

Class* addGeneric(Project* parent)
{
	auto gen = new Class{"Generic", Modifier::Public};
	if (parent) parent->classes()->append(gen);

	gen->typeArguments()->append(new FormalTypeArgument{"P"});
	gen->typeArguments()->append(new FormalTypeArgument{"Q", new ReferenceExpression{"P"}});
	gen->typeArguments()->append(new FormalTypeArgument{"R", nullptr, new ReferenceExpression{"P"}});
	auto specialized = new FormalTypeArgument{"Specialized"};
	specialized->setSpecializationExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	gen->typeArguments()->append(specialized);

	// Add some fields
	gen->fields()->append(new Field{"index", new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT},
			new IntegerLiteral{42}});
	gen->fields()->append(new Field{"data", new ClassTypeExpression{new ReferenceExpression{"P"}}});

	Method* foo = new Method{"foo", Modifier::Public};
	gen->methods()->append(foo);
	foo->typeArguments()->append(new FormalTypeArgument{"T"});

	Method* bar = new Method{"bar", Modifier::Public | Modifier::Static};
	gen->methods()->append(bar);
	bar->typeArguments()->append(new FormalTypeArgument{"S"});
	bar->typeArguments()->append(new FormalTypeArgument{"U", new ReferenceExpression{"S"}});
	bar->typeArguments()->append(new FormalTypeArgument{"V", nullptr, new ReferenceExpression{"S"}});

	Method* foobar = new Method{"foobar", Modifier::Public};
	gen->methods()->append(foobar);

	VariableDeclarationExpression* var = new VariableDeclarationExpression{"var"};
	foobar->items()->append(new ExpressionStatement{var});
	ClassTypeExpression* varType = new ClassTypeExpression{};
	var->decl()->setTypeExpression(varType);
	varType->setTypeExpression(new ReferenceExpression{"Generic"});
	varType->typeExpression()->typeArguments()->append(new ReferenceExpression{"A"});
	varType->typeExpression()->typeArguments()->append(new ReferenceExpression{"B"});
	varType->typeExpression()->typeArguments()->append(new ReferenceExpression{"C"});

	ExpressionStatement* callFooSt = new ExpressionStatement{};
	MethodCallExpression* callFoo = new MethodCallExpression{"foo"};
	DCast<ReferenceExpression>(callFoo->callee())->typeArguments()->append(new ReferenceExpression{"A"});
	callFooSt->setExpression(callFoo);
	foobar->items()->append(callFooSt);

	ExpressionStatement* callBarSt = new ExpressionStatement{};
	MethodCallExpression* callBar = new MethodCallExpression{"bar"};
	DCast<ReferenceExpression>(callBar->callee())->typeArguments()->append(new ReferenceExpression{"A"});
	DCast<ReferenceExpression>(callBar->callee())->typeArguments()->append(new ReferenceExpression{"B"});
	DCast<ReferenceExpression>(callBar->callee())->typeArguments()->append(new ReferenceExpression{"C"});
	callBarSt->setExpression(callBar);
	foobar->items()->append(callBarSt);

	// Set positions
	std::unique_ptr<Position>(gen->extension<Position>())->set(1, 0);
	std::unique_ptr<Position>(foo->extension<Position>())->set(0, 0);
	std::unique_ptr<Position>(bar->extension<Position>())->set(0, 1);
	std::unique_ptr<Position>(foobar->extension<Position>())->set(0, 2);

	return gen;
}

Class* addAnnotatedWithFriends(Project* parent)
{
	auto ann = new Class{"AnnotatedWithFriends", Modifier::Public};
	if (parent) parent->classes()->append(ann);

	ann->annotations()->append( new ExpressionStatement{new ReferenceExpression{"SomeAnnotation"}});

	// Add some friends classes
	ann->friends()->append(new Class{"Generic"});
	ann->friends()->append(new Class{"Boo.Foo"});

	// Add methods
	Method* foo = new Method{"foo", Modifier::Public};
	ann->methods()->append(foo);
	foo->annotations()->append(new ExpressionStatement{new ReferenceExpression{"SomeAnnotation2"}});
	foo->annotations()->append(new ExpressionStatement{new ReferenceExpression{"SomeAnnotation3"}});

	VariableDeclarationExpression* var = new VariableDeclarationExpression{"bodyVar"};
	foo->items()->append(new ExpressionStatement{var});
	var->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	var->decl()->setInitialValue(new IntegerLiteral{42});

	// Set positions
	std::unique_ptr<Position>(ann->extension<Position>())->set(1, 1);

	return ann;
}

Class* addEnumeration(Project* parent)
{
	auto en = new Class{"Colors", Modifier::Public, Class::ConstructKind::Enum};
	if (parent) parent->classes()->append(en);

	en->enumerators()->append( new Enumerator{"RED"});
	en->enumerators()->append( new Enumerator{"GREEN"});
	en->enumerators()->append( new Enumerator{"BLUE", new IntegerLiteral{5}});

	// Set positions
	std::unique_ptr<Position>(en->extension<Position>())->set(1, 2);

	return en;
}

Class* addAnnotation(Project* parent)
{
	auto ann = new Class{"Annotation", Modifier::Public, Class::ConstructKind::Annotation};
	if (parent) parent->classes()->append(ann);

	// Set positions
	std::unique_ptr<Position>(ann->extension<Position>())->set(1, 3);

	return ann;
}

Class* addInner()
{
	Class* outer = new Class{"Outer", Modifier::Public};

	outer->classes()->append( new Class{"InnerStruct", Class::ConstructKind::Struct});
	outer->classes()->append( new Class{"InnerInterface", Class::ConstructKind::Interface});

	// Set positions
	std::unique_ptr<Position>(outer->extension<Position>())->set(1, 4);

	return outer;
}

Module* addLambda()
{
	auto mod = new Module{"Lambda"};
	mod->fields()->append(new Field{"common",
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});

	auto lambdaCommentNode = new CommentNode{"This is the *Lambda*-Module. The following classes are available\n"
														  " * IUnary\n"
														  " * IBinary\n"
														  " * INoReturn\n"
														  " * LambdaTest\n"
														  "Here you find some more information about **Classes**\n"
														  "[table#someTable#2#1]\n"};

	auto aTable = new CommentTable{nullptr, "someTable", 2, 1};
	aTable->setNodeAt(0, 0, new CommentNode{"#english wikipedia"});
	aTable->setNodeAt(1, 0, new CommentNode{"[browser#http://en.wikipedia.org]"});
	lambdaCommentNode->tables()->append(aTable);

	mod->setComment(lambdaCommentNode);

	auto iUnary = new Class{"IUnary", Modifier::Public};
	mod->classes()->append(iUnary);
	auto unMet = new Method{"op"};
	iUnary->methods()->append(unMet);
	unMet->results()->append(new FormalResult{"",
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});
	unMet->arguments()->append(new FormalArgument{"x",
				new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});

	auto iBinary = new Class{"IBinary", Modifier::Public};
	mod->classes()->append(iBinary);
	auto binMet = new Method{"op"};
	iBinary->methods()->append(binMet);
	binMet->results()->append(new FormalResult{"",
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});
	binMet->arguments()->append(new FormalArgument{"x",
				new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});
	binMet->arguments()->append(new FormalArgument{"y",
				new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});

	auto iNoRet = new Class{"INoReturn", Modifier::Public};
	mod->classes()->append(iNoRet);
	auto noRetMet = new Method{"op"};
	iNoRet->methods()->append(noRetMet);
	noRetMet->arguments()->append(new FormalArgument{"x",
				new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});

	auto test = new Class{"LambdaTest", Modifier::Public};
	mod->classes()->append(test);

	auto acceptUnary =  new Method{"unary"};
	test->methods()->append(acceptUnary);
	acceptUnary->arguments()->append(new FormalArgument{"x",
					new ClassTypeExpression{new ReferenceExpression{"IUnary"}}});

	auto acceptBinary =  new Method{"binary"};
	test->methods()->append(acceptBinary);
	acceptBinary->arguments()->append(new FormalArgument{"x",
					new ClassTypeExpression{new ReferenceExpression{"IBinary"}}});

	auto acceptNoReturn =  new Method{"noreturn"};
	test->methods()->append(acceptNoReturn);
	acceptNoReturn->arguments()->append(new FormalArgument{"x",
					new ClassTypeExpression{new ReferenceExpression{"INoReturn"}}});

	auto testMet = new Method{"test"};
	test->methods()->append(testMet);

	auto callUnary = new MethodCallExpression{"unary"};
	testMet->items()->append(new ExpressionStatement{callUnary});
	auto le = new LambdaExpression{};
	callUnary->arguments()->append(le);
	le->arguments()->append(new FormalArgument{"x",
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});
	le->body()->append(new ReturnStatement{new BinaryOperation{BinaryOperation::PLUS,
			new ReferenceExpression{"x"}, new IntegerLiteral{1}}});

	auto callBinary= new MethodCallExpression{"binary"};
	testMet->items()->append(new ExpressionStatement{callBinary});
	le = new LambdaExpression{};
	callBinary->arguments()->append(le);
	le->arguments()->append(new FormalArgument{"x",
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});
	le->arguments()->append(new FormalArgument{"y",
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});
	le->body()->append(new ReturnStatement{new BinaryOperation{BinaryOperation::PLUS,
			new ReferenceExpression{"x"}, new ReferenceExpression{"y"}}});

	auto callNoRet= new MethodCallExpression{"noreturn"};
	testMet->items()->append(new ExpressionStatement{callNoRet});
	le = new LambdaExpression{};
	callNoRet->arguments()->append(le);
	le->arguments()->append(new FormalArgument{"x",
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});
	auto someOpCall = new MethodCallExpression{"someOp"};
	someOpCall->arguments()->append(new ReferenceExpression{"x"});
	le->body()->append(new ExpressionStatement{someOpCall});

	// Positions
	mod->extension<Position>()->set(2, 0);
	iUnary->extension<Position>()->set(0, 0);
	iBinary->extension<Position>()->set(0, 1);
	iNoRet->extension<Position>()->set(0, 2);
	test->extension<Position>()->set(1, 0);
	return mod;
}

Project* addJavaLibrary(Project* parent)
{
	auto java = new Project{"Java"};
	if (parent) parent->projects()->append(java);

	Class* string = new Class{"String", Modifier::Public};
	java->classes()->append(string);
	ClassTypeExpression* type = new ClassTypeExpression{};
	string->baseClasses()->append(type);
	type->typeExpression()->ref()->setName("Object");

	Module* io = new Module{};
	java->modules()->append(io);
	io->setName("io");
	io->libraries()->append(new Model::UsedLibrary{"JavaSubLib1"});
	io->libraries()->append(new Model::UsedLibrary{"JavaSubLib2"});

	Class* printstream = new Class{"PrintStream", Modifier::Public};
	io->classes()->append(printstream);

	Method* println = new Method{"println", Modifier::Public};
	printstream->methods()->append(println);

	FormalArgument* arg = new FormalArgument{};
	println->arguments()->append(arg);
	arg->setName("x");
	ClassTypeExpression* argType = new ClassTypeExpression{};
	arg->setTypeExpression(argType);
	argType->typeExpression()->ref()->setName("String");

	Class* system = new Class{"System", Modifier::Public};
	java->classes()->append(system);
	Field* out = new Field{"out", Modifier::Public | Modifier::Static};
	system->fields()->append(out);
	ClassTypeExpression* outtype = new ClassTypeExpression{};
	out->setTypeExpression(outtype);
	outtype->typeExpression()->ref()->setName("PrintStream");
	ReferenceExpression* prefix = new ReferenceExpression{};
	outtype->typeExpression()->setPrefix(prefix);
	prefix->ref()->setName("io");

	// Set positions
	java->extension<Position>()->set(2, 1);
	string->extension<Position>()->set(0, 0);
	system->extension<Position>()->set(0, 1);
	io->extension<Position>()->set(1, 0);

	return java;
}

Method* addLongMethod(Class* parent)
{
	auto longMethod = new Method{"aLongTestMethod"};
	if (parent) parent->methods()->append(longMethod);

	FormalResult* result  = new FormalResult{};
	result->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	longMethod->results()->append(result);

	auto resultComment = new CommentNode{"**Nested tables example**\n"
													 "[table#outerTable#2#2]"};
	result->setComment(resultComment);

	auto InnerTableComment1 = new CommentNode{"[table#innerTable1#2#1]"};
	auto InnerTableComment2 = new CommentNode{"[table#innerTable2#2#1]"};

	auto outerTable = new CommentTable{nullptr, "outerTable", 2, 2};
	outerTable->setNodeAt(0, 0, new CommentNode{"###Inner Table 1"});
	outerTable->setNodeAt(0, 1, new CommentNode{"###Inner Table 2"});
	outerTable->setNodeAt(1, 0, InnerTableComment1);
	outerTable->setNodeAt(1, 1, InnerTableComment2);
	resultComment->tables()->append(outerTable);

	auto innerTable1 = new CommentTable{nullptr, "innerTable1", 2, 1};
	innerTable1->setNodeAt(0, 0, new CommentNode{"Class"});
	innerTable1->setNodeAt(1, 0, new Class{"SampleClass"});
	InnerTableComment1->tables()->append(innerTable1);

	auto innerTable2 = new CommentTable{nullptr, "innerTable2", 2, 1};
	innerTable2->setNodeAt(0, 0, new CommentNode{"Method"});
	innerTable2->setNodeAt(1, 0, new Method{"SampleMethod"});
	InnerTableComment2->tables()->append(innerTable2);

	FormalArgument* arg1 = new FormalArgument{};
	arg1->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	arg1->setName("x");
	longMethod->arguments()->append(arg1);
	FormalArgument* arg2 = new FormalArgument{};
	arg2->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	arg2->setName("y");
	longMethod->arguments()->append(arg2);
	FormalArgument* arg3 = new FormalArgument{};
	arg3->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::FLOAT});
	arg3->setName("epsilon");
	longMethod->arguments()->append(arg3);

	longMethod->throws()->append(new ReferenceExpression{"MyException"});

	auto argumentCommentNode1 = new CommentNode{"[image#styles/icon/globe.svg]"};
	arg1->setComment(argumentCommentNode1);
	auto argumentCommentNode2 = new CommentNode{"[image#styles/icon/pencil.svg]"};
	arg2->setComment(argumentCommentNode2);
	auto argumentCommentNode3 = new CommentNode{"[image#styles/icon/gurica_tree.svg]"};
	arg3->setComment(argumentCommentNode3);

	auto assertSt = new AssertStatement{};
	longMethod->items()->append(assertSt);
	assertSt->setExpression(new ReferenceExpression{"someValue"});

	auto var0 = new VariableDeclarationExpression{"pSystem"};
	var0->decl()->setTypeExpression(new PointerTypeExpression{new ClassTypeExpression{
			new ReferenceExpression{"System"}}});
	longMethod->items()->append(new ExpressionStatement{var0});
	longMethod->items()->append(new ExpressionStatement{
			new ReferenceExpression{"out", new ReferenceExpression{"pSystem"}}});


	VariableDeclarationExpression* var1 = new VariableDeclarationExpression{"var1"};
	longMethod->items()->append(new ExpressionStatement{var1});
	var1->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});

	VariableDeclarationExpression* var2 = new VariableDeclarationExpression{"var2"};
	longMethod->items()->append(new ExpressionStatement{var2});
	var2->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::LONG});
	var2->decl()->setInitialValue(new IntegerLiteral{42});

	VariableDeclarationExpression* var3 = new VariableDeclarationExpression{"var3"};
	longMethod->items()->append(new ExpressionStatement{var3});
	var3->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN});
	var3->decl()->setInitialValue(new BooleanLiteral{true});

	VariableDeclarationExpression* var4 = new VariableDeclarationExpression{"var4"};
	longMethod->items()->append(new ExpressionStatement{var4});
	var4->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::CHAR});
	var4->decl()->setInitialValue(new CharacterLiteral{'r'});

	VariableDeclarationExpression* var5 = new VariableDeclarationExpression{"var5"};
	longMethod->items()->append(new ExpressionStatement{var5});
	var5->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::DOUBLE});
	var5->decl()->setInitialValue(new FloatLiteral{112311096123});

	VariableDeclarationExpression* var6 = new VariableDeclarationExpression{"var6"};
	longMethod->items()->append(new ExpressionStatement{var6});
	var6->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG});
	var6->decl()->setInitialValue(new IntegerLiteral{1000});

	VariableDeclarationExpression* var7 = new VariableDeclarationExpression{"var7"};
	longMethod->items()->append(new ExpressionStatement{var7});
	var7->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	var7->decl()->setInitialValue(new NullLiteral{});

	VariableDeclarationExpression* var8 = new VariableDeclarationExpression{"var8"};
	longMethod->items()->append(new ExpressionStatement{var8});
	var8->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	MethodCallExpression* var8Value = new MethodCallExpression{"getId"};
	var8->decl()->setInitialValue(var8Value);

	VariableDeclarationExpression* var9 = new VariableDeclarationExpression{"var9"};
	longMethod->items()->append(new ExpressionStatement{var9});
	var9->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	var9->decl()->setInitialValue(new ThisExpression{});

	VariableDeclarationExpression* var92 = new VariableDeclarationExpression{"var92"};
	longMethod->items()->append(new ExpressionStatement{var92});
	var92->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	var92->decl()->setInitialValue(new SuperExpression{});

	VariableDeclarationExpression* var10 = new VariableDeclarationExpression{"var10"};
	longMethod->items()->append(new ExpressionStatement{var10});
	var10->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	CastExpression* cast = new CastExpression{};
	var10->decl()->setInitialValue(cast);
	cast->setType(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	cast->setExpr(new ReferenceExpression{"epsilon"});

	VariableDeclarationExpression* var10_1 = new VariableDeclarationExpression{"var10_1"};
	longMethod->items()->append(new ExpressionStatement{var10_1});
	var10_1->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN});
	auto instanceOfOp = new InstanceOfExpression{};
	var10_1->decl()->setInitialValue(instanceOfOp);
	instanceOfOp->setExpr(new ReferenceExpression{"var10"});
	instanceOfOp->setTypeExpression(new ReferenceExpression{"String"});

	VariableDeclarationExpression* var11 = new VariableDeclarationExpression{"var11"};
	longMethod->items()->append(new ExpressionStatement{var11});
	var11->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	NewExpression* var11Value = new NewExpression{};
	var11Value->setNewType(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	var11->decl()->setInitialValue(var11Value);

	VariableDeclarationExpression* var12 = new VariableDeclarationExpression{"var12"};
	longMethod->items()->append(new ExpressionStatement{var12});
	ArrayTypeExpression* var12Type = new ArrayTypeExpression{};
	var12Type->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	var12->decl()->setTypeExpression(var12Type);
	NewExpression* var12Value = new NewExpression{};
	var12Value->setNewType(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	var12Value->dimensions()->append(new IntegerLiteral{5});
	var12->decl()->setInitialValue(var12Value);

	VariableDeclarationExpression* var13 = new VariableDeclarationExpression{"var13"};
	longMethod->items()->append(new ExpressionStatement{var13});
	var13->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN});
	UnaryOperation* uOp1 = new UnaryOperation{};
	var13->decl()->setInitialValue(uOp1);
	uOp1->setOp(UnaryOperation::NOT);
	BooleanLiteral* var13val = new BooleanLiteral{};
	var13val->setValue(false);
	uOp1->setOperand(var13val);

	VariableDeclarationExpression* var14 = new VariableDeclarationExpression{"var14"};
	longMethod->items()->append(new ExpressionStatement{var14});
	var14->decl()->setTypeExpression(new TypeQualifierExpression { Type::CONST,
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});
	UnaryOperation* uOp2 = new UnaryOperation{};
	var14->decl()->setInitialValue(uOp2);
	uOp2->setOp(UnaryOperation::POSTINCREMENT);
	uOp2->setOperand(new IntegerLiteral{10});

	VariableDeclarationExpression* var15 = new VariableDeclarationExpression{"var15"};
	longMethod->items()->append(new ExpressionStatement{var15});
	var15->decl()->setTypeExpression(new TypeQualifierExpression { Type::VOLATILE,
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});
	BinaryOperation* binOp0 = new BinaryOperation{};
	var15->decl()->setInitialValue(binOp0);
	binOp0->setOp(BinaryOperation::DIVIDE);
	UnaryOperation* uOp3 = new UnaryOperation{};
	binOp0->setLeft(uOp3);
	binOp0->setRight(new IntegerLiteral{3});
	uOp3->setOp(UnaryOperation::PARENTHESIS);
	BinaryOperation* binOp1 = new BinaryOperation{};
	uOp3->setOperand(binOp1);
	binOp1->setOp(BinaryOperation::PLUS);
	binOp1->setLeft(new IntegerLiteral{41});
	binOp1->setRight(new IntegerLiteral{1});

	VariableDeclarationExpression* var16 = new VariableDeclarationExpression{"var16"};
	longMethod->items()->append(new ExpressionStatement{var16});
	var16->decl()->setTypeExpression(new TypeQualifierExpression { Type::CONST,
			new TypeQualifierExpression { Type::VOLATILE,
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN}}});
	BinaryOperation* binOp2 = new BinaryOperation{};
	var16->decl()->setInitialValue(binOp2);
	binOp2->setOp(BinaryOperation::LESS_EQUALS);
	binOp2->setLeft(new IntegerLiteral{41});
	binOp2->setRight(new IntegerLiteral{1});

	VariableDeclarationExpression* var17 = new VariableDeclarationExpression{"var17"};
	longMethod->items()->append(new ExpressionStatement{var17});
	ArrayTypeExpression* var17Type = new ArrayTypeExpression{};
	var17->decl()->setTypeExpression(var17Type);
	ClassTypeExpression* var17ElementType = new ClassTypeExpression{};
	var17Type->setTypeExpression(var17ElementType);
	var17ElementType->typeExpression()->ref()->setName("String");
	ArrayInitializer* outterArrayInit = new ArrayInitializer{};
	var17->decl()->setInitialValue(outterArrayInit);
	outterArrayInit->values()->append(new StringLiteral{"this"});
	outterArrayInit->values()->append(new StringLiteral{"looks"});
	outterArrayInit->values()->append(new StringLiteral{"good"});

	VariableDeclarationExpression* var18 = new VariableDeclarationExpression{"var18"};
	longMethod->items()->append(new ExpressionStatement{var18});
	ArrayTypeExpression* var18Type = new ArrayTypeExpression{};
	var18->decl()->setTypeExpression(var18Type);
	ArrayTypeExpression* var18TypeType = new ArrayTypeExpression{};
	var18Type->setTypeExpression(var18TypeType);
	var18TypeType->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	ArrayInitializer* matrixArrayInit = new ArrayInitializer{};
	var18->decl()->setInitialValue(matrixArrayInit);
	ArrayInitializer* col1Init = new ArrayInitializer{};
	matrixArrayInit->values()->append(col1Init);
	ArrayInitializer* col2Init = new ArrayInitializer{};
	matrixArrayInit->values()->append(col2Init);
	ArrayInitializer* col3Init = new ArrayInitializer{};
	matrixArrayInit->values()->append(col3Init);
	col1Init->values()->append(new IntegerLiteral{1});
	col1Init->values()->append(new IntegerLiteral{2});
	col1Init->values()->append(new IntegerLiteral{3});
	col2Init->values()->append(new IntegerLiteral{4});
	col2Init->values()->append(new IntegerLiteral{5});
	col2Init->values()->append(new IntegerLiteral{6});
	col3Init->values()->append(new IntegerLiteral{7});
	col3Init->values()->append(new IntegerLiteral{8});
	col3Init->values()->append(new IntegerLiteral{9});

	auto aCommentNode = new CommentNode{"[table#aTable#3#3]"};
	var18->setComment(aCommentNode);

	auto table = new CommentTable{nullptr, "aTable", 3, 3};
	table->setNodeAt(0, 0, new CommentNode{"#1"});
	table->setNodeAt(0, 1, new CommentNode{"#2"});
	table->setNodeAt(0, 2, new CommentNode{"#3"});
	table->setNodeAt(1, 0, new CommentNode{"#4"});
	table->setNodeAt(1, 1, new CommentNode{"#5"});
	table->setNodeAt(1, 2, new CommentNode{"#6"});
	table->setNodeAt(2, 0, new CommentNode{"#7"});
	table->setNodeAt(2, 1, new CommentNode{"#8"});
	table->setNodeAt(2, 2, new CommentNode{"#9"});
	aCommentNode->tables()->append(table);

	VariableDeclarationExpression* var19 = new VariableDeclarationExpression{"var19"};
	longMethod->items()->append(new ExpressionStatement{var19});
	var19->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	ConditionalExpression* ce = new ConditionalExpression{};
	var19->decl()->setInitialValue(ce);
	BinaryOperation* binOp3 = new BinaryOperation{};
	ce->setCondition(binOp3);
	binOp3->setOp(BinaryOperation::LESS_EQUALS);
	binOp3->setLeft(new IntegerLiteral{6});
	binOp3->setRight(new IntegerLiteral{10});
	ce->setTrueExpression(new IntegerLiteral{42});
	ce->setFalseExpression(new IntegerLiteral{0});

	auto throwExpr = new ThrowExpression{};
	longMethod->items()->append(new ExpressionStatement{throwExpr});
	throwExpr->setExpr(new ReferenceExpression{"AnException"});

	auto deleteExpr = new DeleteExpression{false};
	deleteExpr->setExpr(new ReferenceExpression{"pSystem"});
	longMethod->items()->append(new ExpressionStatement{deleteExpr});

	auto deleteArrayExpr = new DeleteExpression{true};
	deleteArrayExpr->setExpr(new ReferenceExpression{"pSystem"});
	longMethod->items()->append(new ExpressionStatement{deleteArrayExpr});

	auto var20 = new VariableDeclarationExpression{"var20", new AutoTypeExpression{}};
	longMethod->items()->append(new ExpressionStatement{var20});
	var20->decl()->setInitialValue( new NewExpression{ new ClassTypeExpression{
			new ReferenceExpression{"HelloWorld"}}});

	auto var21 = new VariableDeclarationExpression{"var21", new FunctionTypeExpression{}};
	longMethod->items()->append(new ExpressionStatement{var21});

	auto var22 = new VariableDeclarationExpression{"var22", new FunctionTypeExpression{
			{new VariableDeclarationExpression{"val",
					new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}},
			 new VariableDeclarationExpression{"name", new ClassTypeExpression{new ReferenceExpression{"String"}}}}}};
	longMethod->items()->append(new ExpressionStatement{var22});

	auto var23 = new VariableDeclarationExpression{"var23", new FunctionTypeExpression{ {},
				{new VariableDeclarationExpression{"",
						new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}}}}};
		longMethod->items()->append(new ExpressionStatement{var23});

	auto var24 = new VariableDeclarationExpression{"var24", new FunctionTypeExpression{
				{new VariableDeclarationExpression{"val",
						new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}},
				 new VariableDeclarationExpression{"name",
						new ClassTypeExpression{new ReferenceExpression{"String"}}}},
				{new VariableDeclarationExpression{"x",
						new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}},
				 new VariableDeclarationExpression{"y",
						new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}}}}};
		longMethod->items()->append(new ExpressionStatement{var24});

	auto var25 = new VariableDeclarationExpression{"var25", new AutoTypeExpression{}};
	longMethod->items()->append(new ExpressionStatement{var25});
	auto ne = new NewExpression{new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}};
	var25->decl()->setInitialValue( ne );
	ne->dimensions()->append(new IntegerLiteral{42});
	ne->dimensions()->append(new IntegerLiteral{3});
	ne->dimensions()->append(new IntegerLiteral{6});

	auto var26 = new VariableDeclarationExpression{"var26", new AutoTypeExpression{}};
	longMethod->items()->append(new ExpressionStatement{var26});
	ne = new NewExpression{new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}};
	var26->decl()->setInitialValue( ne );
	ne->dimensions()->append(new IntegerLiteral{2});
	ne->dimensions()->append(new IntegerLiteral{2});
	matrixArrayInit = new ArrayInitializer{};
	ne->setInitializer(matrixArrayInit);
	col1Init = new ArrayInitializer{};
	matrixArrayInit->values()->append(col1Init);
	col2Init = new ArrayInitializer{};
	matrixArrayInit->values()->append(col2Init);
	col1Init->values()->append(new IntegerLiteral{1});
	col1Init->values()->append(new IntegerLiteral{2});
	col2Init->values()->append(new IntegerLiteral{3});
	col2Init->values()->append(new IntegerLiteral{4});

	IfStatement* ifs = new IfStatement{};
	longMethod->items()->append(ifs);
	BinaryOperation* ifCond = new BinaryOperation{};
	ifs->setCondition(ifCond);
	ifCond->setLeft(new ReferenceExpression{"var14"});
	ifCond->setOp(BinaryOperation::NOT_EQUALS);
	ifCond->setRight(new IntegerLiteral{10});
	UnaryOperation* thenBranch = new UnaryOperation{};
	ifs->thenBranch()->append(new ExpressionStatement{thenBranch});
	thenBranch->setOp(UnaryOperation::POSTINCREMENT);
	thenBranch->setOperand(new ReferenceExpression{"var14"});
	UnaryOperation* elseBranch = new UnaryOperation{};
	ifs->elseBranch()->append(new ExpressionStatement{elseBranch});
	elseBranch->setOp(UnaryOperation::POSTDECREMENT);
	elseBranch->setOperand(new ReferenceExpression{"var14"});

	ifs = new IfStatement{};
	longMethod->items()->append(ifs);
	ifCond = new BinaryOperation{};
	ifs->setCondition(ifCond);
	ifCond->setLeft(new ReferenceExpression{"var14"});
	ifCond->setOp(BinaryOperation::NOT_EQUALS);
	ifCond->setRight(new IntegerLiteral{10});
	thenBranch = new UnaryOperation{};
	ifs->thenBranch()->append(new ExpressionStatement{thenBranch});
	thenBranch->setOp(UnaryOperation::POSTINCREMENT);
	thenBranch->setOperand(new ReferenceExpression{"var14"});
	IfStatement* ifs2 = new IfStatement{};
	ifs->elseBranch()->append(ifs2);
	ifCond = new BinaryOperation{};
	ifs2->setCondition(ifCond);
	ifCond->setLeft(new ReferenceExpression{"var14"});
	ifCond->setOp(BinaryOperation::NOT_EQUALS);
	ifCond->setRight(new IntegerLiteral{10});
	thenBranch = new UnaryOperation{};
	ifs2->thenBranch()->append(new ExpressionStatement{thenBranch});
	thenBranch->setOp(UnaryOperation::POSTINCREMENT);
	thenBranch->setOperand(new ReferenceExpression{"var14"});

	auto swi = new SwitchStatement{};
	swi->setSwitchExpression(new ReferenceExpression{"someVar"});
	longMethod->items()->append(swi);
	//Case 0
	auto swi0 = new CaseStatement{};
	swi->body()->append(swi0);
	swi0->setCaseExpression(new IntegerLiteral{0});
	//Case 1
	auto swi1 = new CaseStatement{};
	swi->body()->append(swi1);
	swi1->setCaseExpression(new IntegerLiteral{1});
	auto swi1Op = new UnaryOperation{};
	swi1->body()->append(new ExpressionStatement{swi1Op});
	swi1Op->setOp(UnaryOperation::PREINCREMENT);
	swi1Op->setOperand(new ReferenceExpression{"result"});
	swi1->body()->append(new BreakStatement{});
	// Default
	auto swiDefault = new CaseStatement{};
	swi->body()->append(swiDefault);
	auto swiDefaultOp = new UnaryOperation{};
	swiDefault->body()->append(new ExpressionStatement{swiDefaultOp});
	swiDefaultOp->setOp(UnaryOperation::PREDECREMENT);
	swiDefaultOp->setOperand(new ReferenceExpression{"result"});

	Block* block = new Block{};
	longMethod->items()->append(block);
	AssignmentExpression* assign = new AssignmentExpression{};
	block->items()->append(new ExpressionStatement{assign});
	assign->setLeft(new ReferenceExpression{"var1"});
	assign->setOp(AssignmentExpression::ASSIGN);
	BinaryOperation* arrayAccess = new BinaryOperation{};
	assign->setRight(arrayAccess);
	arrayAccess->setLeft(new ReferenceExpression{"someArray"});
	arrayAccess->setOp(BinaryOperation::ARRAY_INDEX);
	arrayAccess->setRight(new IntegerLiteral{4});

	LoopStatement* loop = new LoopStatement{};
	longMethod->items()->append(loop);
	VariableDeclarationExpression* initStep = new VariableDeclarationExpression{"i"};
	loop->setInitStep(initStep);
	initStep->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	initStep->decl()->setInitialValue(new IntegerLiteral{0});
	BinaryOperation* loopCondition = new BinaryOperation{};
	loop->setCondition(loopCondition);
	loopCondition->setLeft(new ReferenceExpression{"i"});
	loopCondition->setOp(BinaryOperation::LESS);
	loopCondition->setRight(new ReferenceExpression{"x"});
	AssignmentExpression* updateStep = new AssignmentExpression{};
	loop->setUpdateStep(updateStep);
	updateStep->setLeft(new ReferenceExpression{"i"});
	updateStep->setOp(AssignmentExpression::PLUS_ASSIGN);
	updateStep->setRight(new IntegerLiteral{1});
	AssignmentExpression* loopBodyAssignment = new AssignmentExpression{};
	loop->body()->append(new ExpressionStatement{loopBodyAssignment});
	loopBodyAssignment->setLeft(new ReferenceExpression{"var14"});
	loopBodyAssignment->setOp(AssignmentExpression::TIMES_ASSIGN);
	loopBodyAssignment->setRight(new IntegerLiteral{2});
	loop->body()->append(new ContinueStatement{});
	loop->body()->append(new BreakStatement{});


	ForEachStatement* forEach = new ForEachStatement{};
	longMethod->items()->append(forEach);
	forEach->setVarName("elem");
	forEach->setVarType(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT});
	forEach->setCollection(new ReferenceExpression{"SomeCollection"});
	AssignmentExpression* assignEach = new AssignmentExpression{};
	forEach->body()->append(new ExpressionStatement{assignEach});
	assignEach->setLeft(new ReferenceExpression{"var1"});
	assignEach->setOp(AssignmentExpression::DIVIDE_ASSIGN);
	assignEach->setRight(new ReferenceExpression{"elem"});

	auto trycatch = new TryCatchFinallyStatement{};
	trycatch->tryBody()->append(new ExpressionStatement{new ReferenceExpression{"var1"}});
	auto catch1 = new CatchClause{};
	trycatch->catchClauses()->append(catch1);
	catch1->setExceptionToCatch(new ReferenceExpression{"someExpDecl"});
	catch1->body()->append(new ExpressionStatement{new ReferenceExpression{"var2"}});
	auto catch2 = new CatchClause{};
	trycatch->catchClauses()->append(catch2);
	catch2->setExceptionToCatch(new ReferenceExpression{"someOtherExpDecl"});
	catch2->body()->append(new ExpressionStatement{new ReferenceExpression{"var3"}});
	trycatch->finallyBody()->append(new ExpressionStatement{new ReferenceExpression{"var4"}});
	longMethod->items()->append(trycatch);

	auto sync = new SynchronizedStatement{};
	sync->setExpression(new ReferenceExpression{"someMonitor"});
	sync->body()->append(new ExpressionStatement{new ReferenceExpression{"var22"}});
	longMethod->items()->append(sync);

	ReturnStatement* longMethodReturn = new ReturnStatement{};
	longMethod->items()->append(longMethodReturn);
	longMethodReturn->values()->append(new IntegerLiteral{42});

	std::unique_ptr<Position>(longMethod->extension<Position>())->set(1, 0);

	return longMethod;
}

Method* addFactorial(Class* parent)
{
	auto factorial = new Method{"factorial"};
	if (parent) parent->methods()->append(factorial);

	auto factorialCommentNode = new CommentNode{"This is an iterative implementation of a factorial function\n"};

	factorial->setComment(factorialCommentNode);

	FormalResult* factorialResult = new FormalResult{};
	factorial->results()->append(factorialResult);
	factorialResult->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});

	factorialResult->setComment(new CommentNode{"This is the result of the factorial function"});

	FormalArgument* factorialArgument = new FormalArgument{};
	factorial->arguments()->append(factorialArgument);
	factorialArgument->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	factorialArgument->setName("x");

	auto factorialArgumentCommentNode = new CommentNode{"Inline HTML to Browser\n"
													"<html>\n"
													"	<script type=\"text/javascript\">\n"
													"		function hi() { alert(\"Hello World!\"); } \n"
													"	</script>\n"
													"	<button onclick=\"hi()\">Try it</button>\n"
													"</html>"};

	factorialArgument->setComment(factorialArgumentCommentNode);

	VariableDeclarationExpression* res = new VariableDeclarationExpression{"result"};
	factorial->items()->append(new ExpressionStatement{res});
	res->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	res->decl()->setInitialValue(new IntegerLiteral{1});

	// Condition
	IfStatement* ifs = new IfStatement{};
	factorial->items()->append(ifs);
	BinaryOperation* ifCond = new BinaryOperation{};
	ifs->setCondition(ifCond);
	ifCond->setLeft(new ReferenceExpression{"x"});
	ifCond->setOp(BinaryOperation::GREATER_EQUALS);
	ifCond->setRight(new IntegerLiteral{0});

	// Then
	LoopStatement* loop = new LoopStatement{};
	ifs->thenBranch()->append(loop);
	VariableDeclarationExpression* initStep = new VariableDeclarationExpression{"i"};
	loop->setInitStep(initStep);
	initStep->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	initStep->decl()->setInitialValue(new IntegerLiteral{1});
	BinaryOperation* loopCondition = new BinaryOperation{};
	loop->setCondition(loopCondition);
	loopCondition->setLeft(new ReferenceExpression{"i"});
	loopCondition->setOp(BinaryOperation::LESS_EQUALS);
	loopCondition->setRight(new ReferenceExpression{"x"});
	AssignmentExpression* updateStep = new AssignmentExpression{};
	loop->setUpdateStep(updateStep);
	updateStep->setLeft(new ReferenceExpression{"i"});
	updateStep->setOp(AssignmentExpression::PLUS_ASSIGN);
	updateStep->setRight(new IntegerLiteral{1});
	AssignmentExpression* loopBodyAssignment = new AssignmentExpression{};
	loop->body()->append(new ExpressionStatement{loopBodyAssignment});
	loopBodyAssignment->setLeft(new ReferenceExpression{"result"});
	loopBodyAssignment->setOp(AssignmentExpression::TIMES_ASSIGN);
	loopBodyAssignment->setRight(new ReferenceExpression{"i"});

	// Else
	AssignmentExpression* elseBranch = new AssignmentExpression{};
	ifs->elseBranch()->append(new ExpressionStatement{elseBranch});

	elseBranch->setLeft(new ReferenceExpression{"result"});
	elseBranch->setOp(AssignmentExpression::ASSIGN);
	elseBranch->setRight(new IntegerLiteral{-1});

	// Return
	ReturnStatement* factorialReturn = new ReturnStatement{};
	factorial->items()->append(factorialReturn);
	factorialReturn->values()->append(new ReferenceExpression{"result"});

	std::unique_ptr<Position>(factorial->extension<Position>())->set(0, 3);

	return factorial;
}

Method* addExtraMethod(Class* parent)
{
	auto extra = new Method{"additional"};
	if (parent) parent->methods()->append(extra);

	extra->items()->append(new DeclarationStatement{new TypeAlias{"newTypeName",
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}}});

	auto templateRefExpr = new ReferenceExpression{"Template"};
	templateRefExpr->typeArguments()->append(new ReferenceExpression{"S"});
	auto templateAlias = new TypeAlias{"templateAlias", templateRefExpr};
	templateAlias->typeArguments()->append(new FormalTypeArgument{"S"});
	extra->items()->append(new DeclarationStatement{templateAlias});

	auto explicitRef = new ReferenceExpression{"templateAlias"};
	explicitRef->typeArguments()->append(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::LONG});
	extra->items()->append(new DeclarationStatement{new ExplicitTemplateInstantiation{explicitRef}});

	extra->items()->append(new ExpressionStatement{
					new ReferenceExpression{"someGlobalVariable", new GlobalScopeExpression{}}});
	extra->items()->append(new ExpressionStatement{ new TypeTraitExpression{TypeTraitExpression::TypeTraitKind::SizeOf,
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}}});
	extra->items()->append(new ExpressionStatement{ new TypeTraitExpression{TypeTraitExpression::TypeTraitKind::AlignOf,
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::FLOAT}}});
	extra->items()->append(new ExpressionStatement{ new TypeTraitExpression{TypeTraitExpression::TypeTraitKind::TypeId,
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::DOUBLE}}});

	auto loop = new LoopStatement{LoopStatement::LoopKind::PostCheck};
	extra->items()->append(loop);
	BinaryOperation* loopCondition = new BinaryOperation{};
	loop->setCondition(loopCondition);
	loopCondition->setLeft(new ReferenceExpression{"i"});
	loopCondition->setOp(BinaryOperation::LESS);
	loopCondition->setRight(new ReferenceExpression{"x"});
	AssignmentExpression* loopBodyAssignment = new AssignmentExpression{};
	loop->body()->append(new ExpressionStatement{loopBodyAssignment});
	loopBodyAssignment->setLeft(new ReferenceExpression{"var14"});
	loopBodyAssignment->setOp(AssignmentExpression::TIMES_ASSIGN);
	loopBodyAssignment->setRight(new IntegerLiteral{2});
	loop->body()->append(new ContinueStatement{});
	loop->body()->append(new BreakStatement{});


	std::unique_ptr<Position>(extra->extension<Position>())->set(0, 4);

	return extra;
}

class JavaLibraryAndHelloWorldTest : public Test<OOVisualizationPlugin, JavaLibraryAndHelloWorldTest> {
public: void test()
{
	// Create project
	auto prj = new Project{"HelloWorld"};
	prj->libraries()->append(new Model::UsedLibrary{"java"});

	prj->subDeclarations()->append(new NameImport{
			new ReferenceExpression{"Code", new ReferenceExpression{"SomeLibrary"}}});
	prj->subDeclarations()->append(new NameImport{
			new ReferenceExpression{"AnotherLibrary"}});
	prj->subDeclarations()->append(new NameImport{
				new ReferenceExpression{"All", new ReferenceExpression{"Import"}}, true});
	prj->fields()->append(new Field{"global",
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}});
	prj->fields()->append(new Field{"global2",
			new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG}});

	Project* java = nullptr;
	java = addJavaLibrary(prj);

	// Build a simple HelloWorld Application
	Class* hello = nullptr;
	hello = addHelloWorld(prj);

	// Build a simple Generic Class
	Class* gen = nullptr;
	gen = addGeneric(prj);

	// Build a simple Class that has annotations and friends
	Class* ann = nullptr;
	ann = addAnnotatedWithFriends(prj);

	// Build a simple enumeration
	Class* en = nullptr;
	en = addEnumeration(prj);

	// Add an annotation class
	addAnnotation(prj);

//	// Add a second method
	Method* longMethod = nullptr;
	longMethod = addLongMethod(hello);

//	// Add a third method
	Method* factorial = nullptr;
	factorial = addFactorial(hello);

	// Add an extra method
	addExtraMethod(hello);

	prj->modules()->append(addLambda());
	prj->classes()->append(addInner());

// Add a method Add-on
	VMethod::addAddOn(new MethodAddOn{"foo"});

	////////////////////////////////////////////////// Set Scene
	Model::Node* top_level = nullptr;
	if (prj) top_level = prj;
	else if (hello) top_level = hello;
	else if (gen) top_level = gen;
	else if (ann) top_level = ann;
	else if (en) top_level = en;
	else if (java) top_level = java;
	else if (longMethod) top_level = longMethod;
	else top_level = factorial;

	auto manager = new Model::TreeManager{top_level};
	manager->setName("HelloWorld");

	auto mainScene = VisualizationManager::instance().mainScene();
	mainScene->addTopLevelNode(top_level);
	mainScene->listenToTreeManager(manager);

	// Center view
	mainScene->updateNow();
	for (auto v : mainScene->views())
		if (auto mainView = dynamic_cast<Visualization::MainView*>(v))
		{
			mainView->centerOn(mainScene->sceneRect().center());
			break;
		}

	// Watch files
	VisualizationManager::instance().mainScene()->addRefreshActionFunction(
		[top_level](Scene* scene){
			scene->viewItems()->removeAllViewItems();
			scene->setMainCursor(nullptr);
			clearAllStyleSets();
			scene->addTopLevelNode(top_level);
	});

	QStringList filesToWatch;
	std::function<void (QString)> allFiles = [&filesToWatch, &allFiles](QString dirPath)
	{
		auto dir = QDir{dirPath + '/'};
		auto entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDot | QDir::NoDotDot);
		for (auto entry : entries)
		{
			if (entry.isFile()) filesToWatch.append(entry.absoluteFilePath());
			else if (entry.isDir()) allFiles(entry.absoluteFilePath());
		}
	};

	allFiles("styles");
	auto watcher = new QFileSystemWatcher{filesToWatch};

	auto onFileChange = [watcher](const QString& fileName)
	{
		watcher->addPath(fileName);

		QKeyEvent *eventPress = new QKeyEvent { QEvent::KeyPress, Qt::Key_F5, Qt::NoModifier};
		QKeyEvent *eventRelease = new QKeyEvent { QEvent::KeyRelease, Qt::Key_F5, Qt::NoModifier};
		QCoreApplication::postEvent (VisualizationManager::instance().mainScene(), eventPress);
		QCoreApplication::postEvent (VisualizationManager::instance().mainScene(), eventRelease);
	};


	QObject::connect(watcher, &QFileSystemWatcher::fileChanged, onFileChange);

	CHECK_CONDITION(top_level != nullptr);
}};

}
