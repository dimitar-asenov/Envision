/***********************************************************************************************************************
 * HelloWorldTest.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oovisualization.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "vis/VVisibility.h"
#include "vis/VStatic.h"

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

TEST(OOVisualization, JavaLibraryAndHelloWorldTest)
{
	Scene* scene = new Scene();

	scene->defaultRenderer()->registerVisualization(Visibility::typeIdStatic(), createVisualization<VVisibility, Visibility>);
	scene->defaultRenderer()->registerVisualization(Static::typeIdStatic(), createVisualization<VStatic, Static>);

	////////////////////////////////////////////////// Create Model
	Model::Model* model = new Model::Model();
	Project* prj = dynamic_cast<Project*> (model->createRoot("Project"));

	model->beginModification(prj, "build simple java library and a hello world app");
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
	ref->ref()->set("class:System");
	ref->setPrefix<ReferenceExpression>()->ref()->set("lib:Java");

	// Add a second method
	Method* factorial = hello->methods()->append<Method>();
	factorial->setName("factorial");
	factorial->results()->append<FormalResult>()->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	factorial->arguments()->append<FormalArgument>()->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	factorial->arguments()->at(0)->setName("x");

	VariableDeclaration* var1 = factorial->items()->append<VariableDeclaration>();
	var1->setName("var1");
	var1->setType<PrimitiveType>()->setType(PrimitiveType::INT);

	VariableDeclaration* var2 = factorial->items()->append<VariableDeclaration>();
	var2->setName("var2");
	var2->setType<PrimitiveType>()->setType(PrimitiveType::LONG);
	var2->setInitialValue<IntegerLiteral>()->setValue(42);

	VariableDeclaration* var3 = factorial->items()->append<VariableDeclaration>();
	var3->setName("var3");
	var3->setType<PrimitiveType>()->setType(PrimitiveType::BOOLEAN);
	var3->setInitialValue<BooleanLiteral>()->setValue(true);

	VariableDeclaration* var4 = factorial->items()->append<VariableDeclaration>();
	var4->setName("var4");
	var4->setType<PrimitiveType>()->setType(PrimitiveType::CHAR);
	var4->setInitialValue<CharacterLiteral>()->setValue('r');

	VariableDeclaration* var5 = factorial->items()->append<VariableDeclaration>();
	var5->setName("var5");
	var5->setType<PrimitiveType>()->setType(PrimitiveType::DOUBLE);
	var5->setInitialValue<FloatLiteral>()->setValue(123.112311096123);

	VariableDeclaration* var6 = factorial->items()->append<VariableDeclaration>();
	var6->setName("var6");
	var6->setType<PrimitiveType>()->setType(PrimitiveType::UNSIGNED_LONG);
	var6->setInitialValue<IntegerLiteral>()->setValue(1000);

	// set positions
	factorial->extension<Position>()->setY(100);
	java->extension<Position>()->setX(160);
	java->extension<Position>()->setY(100);
	string->extension<Position>()->setY(100);
	io->extension<Position>()->setX(240);

	model->endModification();
	CHECK_INT_EQUAL(160, java->extension<Position>()->x());
	CHECK_STR_EQUAL("Java", java->name());

	////////////////////////////////////////////////// Set Scene

	scene->addTopLevelItem( scene->defaultRenderer()->render(NULL, prj) );
	scene->scheduleUpdate();

	// Create view
	MainView* view = new MainView(scene);

	CHECK_CONDITION(view != NULL);
}

}
