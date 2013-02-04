/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: May 11, 2012
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "contractslibrary.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "items/ContractsVMethodAddOn.h"
#include "interaction/CreateContractMethod.h"
#include "monitor/ValueAtReturnVisitor.h"

#include "OOModel/src/allOOModelNodes.h"
#include "ModelBase/src/model/Model.h"

#include "InteractionBase/src/autocomplete/AutoComplete.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/node_extensions/Position.h"
#include "VisualizationBase/src/items/RootItem.h"

#include "OOVisualization/src/top_level/VMethod.h"
#include "OOVisualization/src/alternative/VKeywordMethodCall.h"

#include "OOInteraction/src/expression_editor/OOExpressionBuilder.h"
#include "OOInteraction/src/expression_editor/operators/CommandDescriptor.h"

using namespace OOModel;
using namespace Visualization;
using namespace OOInteraction;

namespace ContractsLibrary {

Library* createContractsLibrary()
{
	Library* lib = new Library("CodeContracts");
	lib->extension<Position>()->setX(1200);

	Class* contract = new Class("Contract");
	lib->classes()->append(contract);
	contract->extension<Position>()->setY(0);

	Method* req = new Method("Requires", Visibility::PUBLIC, StorageSpecifier::CLASS_VARIABLE);
	contract->methods()->append(req);
	req->arguments()->append( new FormalArgument("precondition", new PrimitiveTypeExpression(PrimitiveType::BOOLEAN)) );
	req->extension<Position>()->setY(0);

	Method* ens = new Method("Ensures", Visibility::PUBLIC, StorageSpecifier::CLASS_VARIABLE);
	contract->methods()->append(ens);
	ens->arguments()->append( new FormalArgument("postcondition", new PrimitiveTypeExpression(PrimitiveType::BOOLEAN)) );
	ens->extension<Position>()->setY(60);

	Method* res = new Method("Result", Visibility::PUBLIC, StorageSpecifier::CLASS_VARIABLE);
	contract->methods()->append(res);
	res->typeArguments()->append( new FormalTypeArgument("T") );
	res->extension<Position>()->setY(120);

	Method* old = new Method("OldValue", Visibility::PUBLIC, StorageSpecifier::CLASS_VARIABLE);
	contract->methods()->append(old);
	old->typeArguments()->append(new FormalTypeArgument("T"));
	old->arguments()->append( new FormalArgument("variable", new ReferenceExpression("T")) );
	old->extension<Position>()->setY(180);

	Method* contractClass = new Method("ContractClass", Visibility::PUBLIC, StorageSpecifier::CLASS_VARIABLE);
	contract->methods()->append(contractClass);
	contractClass->arguments()->append( new FormalArgument("class", new ReferenceExpression("Class")) );
	contractClass->extension<Position>()->setY(240);

	Method* contractClassFor = new Method("ContractClassFor", Visibility::PUBLIC, StorageSpecifier::CLASS_VARIABLE);
	contract->methods()->append(contractClassFor);
	contractClassFor->arguments()->append( new FormalArgument("class", new ReferenceExpression("Class")) );
	contractClassFor->extension<Position>()->setY(300);

	Method* out = new Method("ValueAtReturn", Visibility::PUBLIC, StorageSpecifier::CLASS_VARIABLE);
	contract->methods()->append(out);
	out->typeArguments()->append( new FormalTypeArgument("T") );
	out->arguments()->append( new FormalArgument("argument", new ReferenceExpression("T")) );
	out->extension<Position>()->setY(360);

	ValueAtReturnVisitor::setMethods(ens, out);

	// Register a group that holds the guard condition: are we visualizing a method belonging to the Contract class?
	auto g = new VisualizationGroup();
	g->setConditionFunction([=](Visualization::Item*, Model::Node* node) -> bool
	{
		auto call = static_cast<OOModel::MethodCallExpression*>(node);
		if (auto def = call->methodDefinition())
		{
			auto* p = def->parent();
			while (p)
			{
				if (p == contract) return true;
				p = p->parent();
			}
		}

		return false;
	});

	// Register Visualizations in the group
	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
			{
				return new OOVisualization::VKeywordMethodCall(parent, static_cast<MethodCallExpression*> (node),
						OOVisualization::VKeywordMethodCall::itemStyles().get("requires"));
			},
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition() == req;
			});
	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
			{
				return new OOVisualization::VKeywordMethodCall(parent, static_cast<MethodCallExpression*> (node),
						OOVisualization::VKeywordMethodCall::itemStyles().get("ensures"));
			},
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition() == ens;
			});
	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
			{
				return new OOVisualization::VKeywordMethodCall(parent, static_cast<MethodCallExpression*> (node),
						OOVisualization::VKeywordMethodCall::itemStyles().get("old"));
			},
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition() == old;
			});
	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
			{
				return new OOVisualization::VKeywordMethodCall(parent, static_cast<MethodCallExpression*> (node),
						OOVisualization::VKeywordMethodCall::itemStyles().get("result"));
			},
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition() == res;
			});
	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
			{
				return new OOVisualization::VKeywordMethodCall(parent, static_cast<MethodCallExpression*> (node),
						OOVisualization::VKeywordMethodCall::itemStyles().get("out"));
			},
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition() == out;
			});

	Scene::defaultRenderer()->registerGroup(MethodCallExpression::typeIdStatic(), g);

	// Register custom input
	CommandDescriptor::registerCommand(new CreateContractMethod("requires", "Requires"));
	CommandDescriptor::registerCommand(new CreateContractMethod("ensures", "Ensures"));
	CommandDescriptor::registerCommand(new CreateContractMethod("old", "OldValue"));
	CommandDescriptor::registerCommand(new CreateContractMethod("result", "Result",1));

	// Register method add on
	OOVisualization::VMethod::addAddOn( new ContractsVMethodAddOn(contractClass) );

	return lib;
}

Class* createBaseClass()
{
	Class* car = new Class("Car", Visibility::PUBLIC);

	auto *fuel = new Field( "fuel", new PrimitiveTypeExpression(PrimitiveType::INT), Visibility::PUBLIC);
	car->fields()->append(fuel);

	auto *travel = new Method("travel", Visibility::PUBLIC);
	car->methods()->append(travel);
	travel->arguments()->append( new FormalArgument("numPassengers", new PrimitiveTypeExpression(PrimitiveType::INT)) );
	travel->results()->append( new FormalResult(QString(), new PrimitiveTypeExpression(PrimitiveType::INT)) );

	travel->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
			"CodeContracts.Contract.Requires(fuel>0)")));

	travel->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
			"CodeContracts.Contract.Requires(numPassengers>0)")));

	travel->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
			"CodeContracts.Contract.Ensures(fuel<CodeContracts.Contract.OldValue(fuel))")));

	travel->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
			"CodeContracts.Contract.Ensures(CodeContracts.Contract.Result<int>()>0)")));

	return car;
}

Class* createDerivedClass()
{
	Class* car = new Class("SelfDrivingCar", Visibility::PUBLIC);
	car->baseClasses()->append(new ReferenceExpression("Car"));

	auto *travel = new Method("travel", Visibility::PUBLIC);
	car->methods()->append(travel);
	travel->arguments()->append( new FormalArgument("numPassengers", new PrimitiveTypeExpression(PrimitiveType::INT)) );
	travel->results()->append( new FormalResult(QString(), new PrimitiveTypeExpression(PrimitiveType::INT)) );

	travel->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
			"CodeContracts.Contract.Requires(numPassengers>=0)")));

	car->extension<Position>()->setY(180);

	return car;
}

Class* createMinMax()
{
	Class* minMaxClass = new Class("MinMax", Visibility::PUBLIC);

	auto *minMax = new Method("minMax", Visibility::PUBLIC);
	minMaxClass->methods()->append(minMax);
	minMax->arguments()->append( new FormalArgument("x", new PrimitiveTypeExpression(PrimitiveType::INT)) );
	minMax->arguments()->append( new FormalArgument("y", new PrimitiveTypeExpression(PrimitiveType::INT)) );
	minMax->arguments()->append(
			new FormalArgument("min", new PrimitiveTypeExpression(PrimitiveType::INT), FormalArgument::OUT) );
	minMax->arguments()->append(
			new FormalArgument("max", new PrimitiveTypeExpression(PrimitiveType::INT), FormalArgument::OUT) );

	minMax->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
			"CodeContracts.Contract.Ensures(CodeContracts.Contract.ValueAtReturn(min)<="
			"CodeContracts.Contract.ValueAtReturn(max))")));

	auto i = new IfStatement();
	minMax->items()->append(i);
	i->setCondition( OOExpressionBuilder::getOOExpression("a>b"));
	i->thenBranch()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("max=a")));
	i->thenBranch()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("min=b")));
	i->elseBranch()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("max=b")));
	i->elseBranch()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("min=a")));

	minMaxClass->extension<Position>()->setX(820);

	return minMaxClass;
}

Class* createInterface()
{
	Class* interface = new Class("ICalc", Visibility::PUBLIC);
	interface->annotations()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
			"CodeContracts.Contract.ContractClass(ICalcContracts)")));

	auto *fact = new Method("factorial", Visibility::PUBLIC);
	interface->methods()->append(fact);
	fact->arguments()->append( new FormalArgument("a", new PrimitiveTypeExpression(PrimitiveType::INT)) );
	fact->results()->append( new FormalResult(QString(), new PrimitiveTypeExpression(PrimitiveType::INT)) );

	auto *div = new Method("divide", Visibility::PUBLIC);
	interface->methods()->append(div);
	div->arguments()->append( new FormalArgument("x", new PrimitiveTypeExpression(PrimitiveType::DOUBLE)) );
	div->arguments()->append( new FormalArgument("divisor", new PrimitiveTypeExpression(PrimitiveType::DOUBLE)) );
	div->results()->append( new FormalResult(QString(), new PrimitiveTypeExpression(PrimitiveType::DOUBLE)) );
	div->extension<Position>()->setY(120);

	interface->extension<Position>()->setX(380);

	return interface;
}

Class* createInterfaceContracts()
{
	Class* calcContracts = new Class("ICalcContracts", Visibility::PUBLIC);
	calcContracts->annotations()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
				"CodeContracts.Contract.ContractClassFor(ICalc)")));
	calcContracts->baseClasses()->append(OOExpressionBuilder::getOOExpression("ICalc"));

	auto *fact = new Method("factorial", Visibility::PUBLIC);
	calcContracts->methods()->append(fact);
	fact->arguments()->append( new FormalArgument("a", new PrimitiveTypeExpression(PrimitiveType::INT)) );
	fact->results()->append( new FormalResult(QString(), new PrimitiveTypeExpression(PrimitiveType::INT)) );
	fact->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
				"CodeContracts.Contract.Requires(a>=0)")));
	fact->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
			"CodeContracts.Contract.Ensures(CodeContracts.Contract.Result<int>()>0)")));

	auto *div = new Method("divide", Visibility::PUBLIC);
	calcContracts->methods()->append(div);
	div->arguments()->append( new FormalArgument("x", new PrimitiveTypeExpression(PrimitiveType::DOUBLE)) );
	div->arguments()->append( new FormalArgument("divisor", new PrimitiveTypeExpression(PrimitiveType::DOUBLE)) );
	div->results()->append( new FormalResult(QString(), new PrimitiveTypeExpression(PrimitiveType::DOUBLE)) );
	div->extension<Position>()->setY(100);
	div->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
				"CodeContracts.Contract.Requires(divisor!=0)")));

	calcContracts->extension<Position>()->setX(380);
	calcContracts->extension<Position>()->setY(340);

	return calcContracts;
}
TEST(ContractsLibrary, ContractsLibraryTest)
{
	////////////////////////////////////////////////// Create Model
	Model::Model* model = new Model::Model();
	Project* prj = nullptr;

	// Create project
	prj = dynamic_cast<Project*> (model->createRoot("Project"));
	model->beginModification(prj, "create a few classes that use contracts");
	prj->setName("HelloWorld");
	prj->libraries()->append(createContractsLibrary());
	prj->classes()->append( createBaseClass());
	prj->classes()->append( createDerivedClass() );
	prj->classes()->append( createMinMax() );
	prj->classes()->append( createInterface() );
	prj->classes()->append( createInterfaceContracts() );
	model->endModification();

	VisualizationManager::instance().mainScene()->addTopLevelItem( new RootItem(prj));
	VisualizationManager::instance().mainScene()->listenToModel(model);

	CHECK_CONDITION(prj != nullptr);
}

}
