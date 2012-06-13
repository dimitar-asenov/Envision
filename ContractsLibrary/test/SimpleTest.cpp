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

#include "items/VContractCall.h"
#include "interaction/CreateContractMethod.h"

#include "OOModel/src/allOOModelNodes.h"
#include "ModelBase/src/Model.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/views/MainView.h"
#include "VisualizationBase/src/renderer/ModelRenderer.h"
#include "VisualizationBase/src/items/VExtendable.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/node_extensions/Position.h"
#include "VisualizationBase/src/items/RootItem.h"

#include "OOInteraction/src/expression_editor/OOExpressionBuilder.h"
#include "OOInteraction/src/expression_editor/operators/CommandDescriptor.h"

using namespace OOModel;
using namespace Visualization;
using namespace OOInteraction;

namespace ContractsLibrary {

Library* createContractsLibrary()
{
	Library* lib = new Library("CodeContracts");
	lib->extension<Position>()->setX(540);

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
				return new VContractCall(parent, static_cast<MethodCallExpression*> (node),
						VContractCall::itemStyles().get("requires"));
			},
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition() == req;
			});
	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
			{
				return new VContractCall(parent, static_cast<MethodCallExpression*> (node),
						VContractCall::itemStyles().get("ensures"));
			},
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition() == ens;
			});
	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
			{
				return new VContractCall(parent, static_cast<MethodCallExpression*> (node),
						VContractCall::itemStyles().get("old"));
			},
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition() == old;
			});
	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
			{
				return new VContractCall(parent, static_cast<MethodCallExpression*> (node),
						VContractCall::itemStyles().get("result"));
			},
			[=](Visualization::Item*, Model::Node* node) -> bool
			{
				auto call = static_cast<OOModel::MethodCallExpression*>(node);
				return call->methodDefinition() == res;
			});

	Scene::defaultRenderer()->registerGroup(MethodCallExpression::typeIdStatic(), g);

	// Register custom input
	CommandDescriptor::registerCommand(new CreateContractMethod("requires", "Requires"));
	CommandDescriptor::registerCommand(new CreateContractMethod("ensures", "Ensures"));
	CommandDescriptor::registerCommand(new CreateContractMethod("old", "OldValue"));
	CommandDescriptor::registerCommand(new CreateContractMethod("result", "Result",1));

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

TEST(ContractsLibrary, ContractsLibraryTest)
{
	CHECK_INT_EQUAL(1,1);

	Scene* scene = new Scene();

	////////////////////////////////////////////////// Create Model
	Model::Model* model = new Model::Model();
	Project* prj = nullptr;

	// Create project
	prj = dynamic_cast<Project*> (model->createRoot("Project"));
	model->beginModification(prj, "build simple java library and a hello world app");
	prj->setName("HelloWorld");
	prj->libraries()->append(createContractsLibrary());
	prj->classes()->append( createBaseClass());
	prj->classes()->append( createDerivedClass() );
	model->endModification();

	scene->addTopLevelItem( new RootItem(prj));
	scene->scheduleUpdate();
	scene->listenToModel(model);

	// Create view
	MainView* view = new MainView(scene);

	CHECK_CONDITION(view != nullptr);
}

}
