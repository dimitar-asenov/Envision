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

	VariableDeclaration* resultVar = test->items()->append<VariableDeclaration>();
	resultVar->setName("result");
	resultVar->setType<PrimitiveType>()->setType(PrimitiveType::INT);
	MethodCallExpression* sumCall = resultVar->setInitialValue<MethodCallExpression>();
	sumCall->ref()->set("method:sum");
	sumCall->arguments()->append<IntegerLiteral>()->setValue(0);
	sumCall->arguments()->append<VariableAccess>()->ref()->set("local:index");
	sumCall->setPrefix<ThisExpression>();

	test->items()->append<ReturnStatement>()->values()->append<VariableAccess>()->ref()->set("local:result");


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
