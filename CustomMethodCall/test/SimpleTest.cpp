/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "custommethodcall.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "CustomVisualization.h"

#include "OOModel/headers/allOOModelNodes.h"

#include "VisualizationBase/headers/node_extensions/Position.h"
#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/views/MainView.h"

using namespace OOModel;
using namespace Visualization;

namespace CustomMethodCall {

Class* addCollection(Model::Model* model, Project* parent)
{
	Class* col = NULL;

	if (!parent) col = dynamic_cast<Class*> (model->createRoot("Class"));
	model->beginModification(parent ? static_cast<Model::Node*> (parent) :col, "Adding a collection class.");
	if (!col) col = parent->classes()->append<Class>();

	col->setName("Collection");
	col->setVisibility(Visibility::PUBLIC);

	Method* find = col->methods()->append<Method>();
	find->setName("find");
	find->extension<CustomVisualization>()->setVisName("FindMethodVis");
	FormalArgument* findArg = find->arguments()->append<FormalArgument>();
	findArg->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	findArg->setName("x");
	find->results()->append<FormalResult>()->setType<PrimitiveType>()->setType(PrimitiveType::INT);

	Method* insert = col->methods()->append<Method>();
	insert->setName("insert");
	insert->extension<CustomVisualization>()->setVisName("InsertMethodVis");
	insert->extension<Position>()->setY(100);
	FormalArgument* insertArg = insert->arguments()->append<FormalArgument>();
	insertArg->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	insertArg->setName("x");

	Method* empty = col->methods()->append<Method>();
	empty->setName("empty");
	empty->extension<CustomVisualization>()->setVisName("EmptyMethodVis");
	empty->extension<Position>()->setY(200);
	empty->results()->append<FormalResult>()->setType<PrimitiveType>()->setType(PrimitiveType::BOOLEAN);

	Method* exists = col->methods()->append<Method>();
	exists->setName(QChar(0x2203));
	exists->extension<CustomVisualization>()->setVisName("ExistsMethodVis");
	exists->extension<Position>()->setY(300);
	FormalArgument* existsArg = exists->arguments()->append<FormalArgument>();
	existsArg->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	existsArg->setName("x");
	exists->results()->append<FormalResult>()->setType<PrimitiveType>()->setType(PrimitiveType::BOOLEAN);

	Method* sum = col->methods()->append<Method>();
	sum->setName("sum");
	sum->extension<CustomVisualization>()->setVisName("SumMethodVis");
	sum->extension<Position>()->setY(400);
	FormalArgument* sumArgFrom = sum->arguments()->append<FormalArgument>();
	sumArgFrom->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	sumArgFrom->setName("from");
	FormalArgument* sumArgTo = sum->arguments()->append<FormalArgument>();
	sumArgTo->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	sumArgTo->setName("to");
	sum->results()->append<FormalResult>()->setType<PrimitiveType>()->setType(PrimitiveType::INT);

	Method* test = col->methods()->append<Method>();
	test->setName("test");
	test->extension<Position>()->setX(300);

	IfStatement* ifs = test->items()->append<IfStatement>();
	BinaryOperation* orIf = ifs->setCondition<BinaryOperation>();
	orIf->setOp(BinaryOperation::CONDITIONAL_OR);
	MethodCallExpression* emptyCall = orIf->setLeft<MethodCallExpression>();
	emptyCall->ref()->set("method:empty");

	UnaryOperation* negation = orIf->setRight<UnaryOperation>();
	negation->setOp(UnaryOperation::NOT);
	MethodCallExpression* existsCall = negation->setOperand<MethodCallExpression>();
	existsCall->ref()->set(QString("method:%1").arg(QChar(0x2203)));
	existsCall->arguments()->append<IntegerLiteral>()->setValue(42);

	MethodCallStatement* insertCall = ifs->thenBranch()->append<MethodCallStatement>();
	insertCall->ref()->set("method:insert");
	insertCall->arguments()->append<IntegerLiteral>()->setValue(42);

	VariableDeclaration* indexVar = test->items()->append<VariableDeclaration>();
	indexVar->setName("index");
	indexVar->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	MethodCallExpression* findCall = indexVar->setInitialValue<MethodCallExpression>();
	findCall->ref()->set("method:find");
	findCall->arguments()->append<IntegerLiteral>()->setValue(42);

	MethodCallExpression* sumCall = test->items()->append<ReturnStatement>()->values()->append<MethodCallExpression>();
	sumCall->ref()->set("method:sum");
	sumCall->arguments()->append<IntegerLiteral>()->setValue(0);
	sumCall->arguments()->append<VariableAccess>()->ref()->set("local:index");

	test->results()->append<FormalResult>()->setType<PrimitiveType>()->setType(PrimitiveType::INT);

	model->endModification();
	return col;
}

TEST(CustomMethodCall, CustomVisTest)
{
	Scene* scene = new Scene();

	////////////////////////////////////////////////// Create Model
	Model::Model* model = new Model::Model();

	Class* collection = NULL;
	collection = addCollection(model, NULL);

	////////////////////////////////////////////////// Set Scene
	Model::Node* top_level = NULL;
	if(collection) top_level = collection;

	scene->addTopLevelItem( scene->defaultRenderer()->render(NULL, top_level) );
	scene->scheduleUpdate();
	scene->listenToModel(model);

	// Create view
	MainView* view = new MainView(scene);

	CHECK_CONDITION(view != NULL);
}

}
