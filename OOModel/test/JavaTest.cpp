/***********************************************************************************************************************
 * JavaTest.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oomodel.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "Project.h"
#include "Library.h"
#include "Class.h"
#include "Module.h"
#include "FormalArgument.h"
#include "Field.h"
#include "types/NamedType.h"
#include "expressions/StringLiteral.h"
#include "expressions/VariableAccess.h"
#include "expressions/ReferenceExpression.h"
#include "statements/MethodCallStatement.h"

#include "ModelBase/headers/Model.h"

namespace OOModel {

TEST(OOModel, JavaLibraryAndHelloWorldTest)
{
	Model::Model model;
	Project* prj = dynamic_cast<Project*> (model.createRoot("Project"));

	model.beginModification(prj, "build simple java library and a hello world app");
	prj->setName("HelloWorld");

	// Build a simple Java Library
	Library* java = prj->libraries()->append<Library>();
	java->setName("Java");

	Class* string = java->classes()->append<Class>();
	string->setName("String");
	string->setVisibility(Visibility::PUBLIC);

	Module* io = java->modules()->append<Module>();
	io->setName("io");

	Class* printstream = io->classes()->append<Class>();
	printstream->setName("PrintStream");
	printstream->setVisibility(Visibility::PUBLIC);

	Method* println = printstream->methods()->append<Method>();
	println->setName("println");
	println->setVisibility(Visibility::PUBLIC);

	FormalArgument* arg = println->arguments()->append<FormalArgument>();
	arg->setName("x");
	NamedType* argType = arg->setType<NamedType>();
	argType->type()->ref()->set("class:String");

	Class* system = java->classes()->append<Class>();
	system->setName("System");
	system->setVisibility(Visibility::PUBLIC);
	Field* out = system->fields()->append<Field>();
	out->setName("out");
	out->setVisibility(Visibility::PUBLIC);
	out->setStat(Static::CLASS_VARIABLE);
	NamedType* outtype = out->setType<NamedType>();
	outtype->type()->ref()->set("class:PrintStream");
	outtype->type()->setPrefix<ReferenceExpression>()->ref()->set("mod:io");

	// Build a simple HelloWorld Application
	Class* hello = prj->classes()->append<Class>();
	hello->setName("HelloWorld");
	hello->setVisibility(Visibility::PUBLIC);
	Method* main = hello->methods()->append<Method>();

	main->setName("main");
	main->setVisibility(Visibility::PUBLIC);
	main->setStat(Static::CLASS_VARIABLE);
	//TODO make an array argument

	MethodCallStatement* callPrintln = main->items()->append<MethodCallStatement>();
	StringLiteral* helloStr = callPrintln->arguments()->append<StringLiteral>();
	helloStr->setValue("Hello World");
	callPrintln->ref()->set("met:println");

	VariableAccess* va = callPrintln->setPrefix<VariableAccess>();
	va->ref()->set("field:out");

	ReferenceExpression* ref = va->setPrefix<ReferenceExpression>();
	ref->ref()->set("lib:Java,class:System");

	model.endModification();
	CHECK_STR_EQUAL("Java", java->name());
}

}
