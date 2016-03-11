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

#include "../src/CustomMethodCallPlugin.h"
#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"

#include "../src/items/FindMethodVis.h"
#include "../src/items/EmptyMethodVis.h"
#include "../src/items/InsertMethodVis.h"
#include "../src/items/SumMethodVis.h"

#include "OOModel/src/allOOModelNodes.h"
#include "OOModel/src/typesystem/TypeArgumentBindings.h"

#include "VisualizationBase/src/node_extensions/Position.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/VisualizationManager.h"

using namespace OOModel;
using namespace Visualization;

namespace CustomMethodCall {

Class* addCollection(Project* parent)
{
	auto col = new Class{"Collection", Modifier::Public};
	if (parent) parent->classes()->append(col);


	Method* find = new Method{};
	col->methods()->append(find);
	find->setName("find");
	find->extension<Position>()->set(0, 0);
	FormalArgument* findArg = new FormalArgument{};
	find->arguments()->append(findArg);
	findArg->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	findArg->setName("x");
	FormalResult* findResult = new FormalResult{};
	findResult->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	find->results()->append(findResult);

	Method* insert = new Method{};
	col->methods()->append(insert);
	insert->setName("insert");
	insert->extension<Position>()->set(0, 1);
	FormalArgument* insertArg = new FormalArgument{};
	insert->arguments()->append(insertArg);
	insertArg->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	insertArg->setName("x");

	Method* empty = new Method{};
	col->methods()->append(empty);
	empty->setName("empty");
	empty->extension<Position>()->set(0, 2);
	FormalResult* emptyResult = new FormalResult{};
	emptyResult->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN});
	empty->results()->append(emptyResult);

	Method* exists = new Method{};
	col->methods()->append(exists);
	exists->setName(QChar{0x2203});
	exists->extension<Position>()->set(0, 3);
	FormalArgument* existsArg = new FormalArgument{};
	exists->arguments()->append(existsArg);
	existsArg->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	existsArg->setName("x");
	FormalResult* existsResult = new FormalResult{};
	existsResult->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN});
	exists->results()->append(existsResult);

	Method* sum = new Method{};
	col->methods()->append(sum);
	sum->setName("sum");
	sum->extension<Position>()->set(0, 4);
	FormalArgument* sumArgFrom = new FormalArgument{};
	sum->arguments()->append(sumArgFrom);
	sumArgFrom->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	sumArgFrom->setName("from");
	FormalArgument* sumArgTo = new FormalArgument{};
	sum->arguments()->append(sumArgTo);
	sumArgTo->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	sumArgTo->setName("to");
	FormalResult* sumResult = new FormalResult{};
	sumResult->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	sum->results()->append(sumResult);

	Method* test = new Method{};
	col->methods()->append(test);
	test->setName("test");
	test->extension<Position>()->set(1, 0);

	IfStatement* ifs = new IfStatement{};
	test->items()->append(ifs);
	BinaryOperation* orIf = new BinaryOperation{};
	ifs->setCondition(orIf);
	orIf->setOp(BinaryOperation::CONDITIONAL_OR);
	MethodCallExpression* emptyCall = new MethodCallExpression{"empty"};
	orIf->setLeft(emptyCall);

	UnaryOperation* negation = new UnaryOperation{};
	orIf->setRight(negation);
	negation->setOp(UnaryOperation::NOT);
	MethodCallExpression* existsCall = new MethodCallExpression{QChar{0x2203}};
	negation->setOperand(existsCall);
	existsCall->arguments()->append( new IntegerLiteral{42});

	ExpressionStatement* insertCallSt = new ExpressionStatement{};
	MethodCallExpression* insertCall = new MethodCallExpression{"insert"};
	insertCall->arguments()->append( new IntegerLiteral{42});
	insertCallSt->setExpression(insertCall);
	ifs->thenBranch()->append(insertCallSt);

	VariableDeclarationExpression* indexVar = new VariableDeclarationExpression{"index"};
	test->items()->append(new ExpressionStatement{indexVar});
	indexVar->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	MethodCallExpression* findCall = new MethodCallExpression{"find"};
	indexVar->decl()->setInitialValue(findCall);
	findCall->arguments()->append( new IntegerLiteral{42});

	VariableDeclarationExpression* resultVar = new VariableDeclarationExpression{"result"};
	test->items()->append(new ExpressionStatement{resultVar});
	resultVar->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	MethodCallExpression* sumCall = new MethodCallExpression{"sum", new ThisExpression{}};
	resultVar->decl()->setInitialValue(sumCall);
	sumCall->arguments()->append( new IntegerLiteral{0});
	sumCall->arguments()->append( new ReferenceExpression{"index"});

	ReturnStatement* testReturn = new ReturnStatement{};
	testReturn->values()->append(new ReferenceExpression{"result"});
	test->items()->append(testReturn);

	FormalResult* testFormalResult = new FormalResult{};
	testFormalResult->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	test->results()->append(testFormalResult);

	// Register a group that holds the guard condition: are we visualizing a method belonging to the Collection class?
	auto g = new VisualizationGroup{};
	g->setConditionFunction([=](Visualization::Item*, Model::Node* node) -> bool
	{
		auto call = static_cast<OOModel::MethodCallExpression*>(node);
		if (auto def = call->methodDefinition({}))
		{
			auto* p = def->parent();
			while (p)
			{
				if (p == col) return true;
				p = p->parent();
			}
		}

		return false;
	});

	// Register Visualizations in the group
	g->addVisualization(createVisualization<FindMethodVis, MethodCallExpression>, FindMethodVis::typeIdStatic(),
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition({}) == find;
			});
	g->addVisualization(createVisualization<EmptyMethodVis, MethodCallExpression>, EmptyMethodVis::typeIdStatic(),
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition({}) == empty;
			});
	g->addVisualization(createVisualization<InsertMethodVis, MethodCallExpression>, InsertMethodVis::typeIdStatic(),
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition({}) == insert;
			});
	g->addVisualization(createVisualization<SumMethodVis, MethodCallExpression>, SumMethodVis::typeIdStatic(),
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition({}) == sum;
			});

	Scene::defaultRenderer()->registerGroup(MethodCallExpression::typeIdStatic(), g);

	return col;
}

class CustomVisTest : public SelfTest::Test<CustomMethodCallPlugin, CustomVisTest> { public: void test()
{
	auto collection = addCollection(nullptr);
	auto manager = new Model::TreeManager{collection};

	VisualizationManager::instance().mainScene()->addTopLevelNode(collection);
	VisualizationManager::instance().mainScene()->listenToTreeManager(manager);

	CHECK_CONDITION(collection != nullptr);
}};

}
