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
 * JavaTest.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oomodel.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "allOOModelNodes.h"

#include "ModelBase/src/Model.h"

namespace OOModel {

TEST(OOModel, JavaLibraryAndHelloWorldTest)
{
	Model::Model model;
	Project* prj = dynamic_cast<Project*> (model.createRoot("Project"));

	model.beginModification(prj, "build simple java library and a hello world app");
	prj->setName("HelloWorld");

	// Build a simple Java Library
	Library* java = new Library();
	prj->libraries()->append(java);
	java->setName("Java");

	Class* string = new Class();
	java->classes()->append(string);
	string->setName("String");
	string->setVisibility(Visibility::PUBLIC);

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

	// Build a simple HelloWorld Application
	Class* hello = new Class();
	prj->classes()->append(hello);
	hello->setName("HelloWorld");
	hello->setVisibility(Visibility::PUBLIC);
	Method* main = new Method();
	hello->methods()->append(main);

	main->setName("main");
	main->setVisibility(Visibility::PUBLIC);
	main->setStorageSpecifier(StorageSpecifier::CLASS_VARIABLE);
	//TODO make an array argument

	ExpressionStatement* callPrintlnSt = new ExpressionStatement();
	MethodCallExpression* callPrintln = new MethodCallExpression();
	StringLiteral* helloStr = new StringLiteral();
	callPrintln->arguments()->append(helloStr);
	helloStr->setValue("Hello World");
	callPrintln->ref()->set("met:println");
	callPrintlnSt->setExpression(callPrintln);
	main->items()->append(callPrintlnSt);

	VariableAccess* va = new VariableAccess();
	callPrintln->setPrefix(va);
	va->ref()->set("field:out");

	ReferenceExpression* ref = new ReferenceExpression();
	va->setPrefix(ref);
	ref->ref()->set("lib:Java,class:System");

	model.endModification();

	CHECK_STR_EQUAL("Java", java->name());
	CHECK_CONDITION(callPrintln->methodDefinition() != nullptr);
	CHECK_CONDITION(callPrintln->methodDefinition() == println);
}

}
