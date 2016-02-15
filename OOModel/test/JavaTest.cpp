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

#include "../src/OOModelPlugin.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "../src/allOOModelNodes.h"

#include "ModelBase/src/model/TreeManager.h"

namespace OOModel {

class JavaLibraryAndHelloWorldTest : public SelfTest::Test<OOModelPlugin, JavaLibraryAndHelloWorldTest>
{ public: void test()
{
	auto prj = new Project{"HelloWorld"};
	Model::TreeManager manager{prj};

	manager.beginModification(prj, "build simple java library and a hello world app");

	// Build a simple Java Library
	Project* java = new Project{};
	prj->projects()->append(java);
	java->setName("Java");

	Class* string = new Class{"String", Modifier::Public};
	java->classes()->append(string);

	Module* io = new Module{};
	java->modules()->append(io);
	io->setName("io");

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
	Field* out = new Field{"out", nullptr, Modifier::Public | Modifier::Static};
	system->fields()->append(out);
	ClassTypeExpression* outtype = new ClassTypeExpression{};
	out->setTypeExpression(outtype);
	outtype->typeExpression()->ref()->setName("PrintStream");
	ReferenceExpression* prefix = new ReferenceExpression{};
	outtype->typeExpression()->setPrefix(prefix);
	prefix->ref()->setName("io");

	// Build a simple HelloWorld Application
	Class* hello = new Class{"HelloWorld", Modifier::Public};
	prj->classes()->append(hello);
	Method* main = new Method{"main", Modifier::Public | Modifier::Static};
	hello->methods()->append(main);

	//TODO make an array argument

	ExpressionStatement* callPrintlnSt = new ExpressionStatement{};
	MethodCallExpression* callPrintln = new MethodCallExpression{"println",
			new ReferenceExpression{"out", new ReferenceExpression{"System", new ReferenceExpression{"Java"}}}};
	StringLiteral* helloStr = new StringLiteral{};
	callPrintln->arguments()->append(helloStr);
	helloStr->setValue("Hello World");
	callPrintlnSt->setExpression(callPrintln);
	main->items()->append(callPrintlnSt);

	manager.endModification();

	CHECK_STR_EQUAL("Java", java->name());

	auto ref = DCast<ReferenceExpression>(callPrintln->callee());
	CHECK_CONDITION(ref);
	CHECK_CONDITION(ref->ref()->isResolved());
	CHECK_CONDITION(callPrintln->methodDefinition() != nullptr);
	CHECK_CONDITION(callPrintln->methodDefinition() == println);
}};

}
