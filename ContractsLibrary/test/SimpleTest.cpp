/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "../src/ContractsLibraryPlugin.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "../src/items/InterfaceContractsVMethodAddOn.h"
#include "../src/items/SignatureContractsVMethodAddOn.h"
#include "../src/items/ContractFilter.h"
#include "../src/monitor/ValueAtReturnVisitor.h"

#include "OOModel/src/allOOModelNodes.h"
#include "ModelBase/src/model/TreeManager.h"

#include "InteractionBase/src/autocomplete/AutoComplete.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/node_extensions/Position.h"

#include "OOVisualization/src/declarations/VMethod.h"
#include "OOVisualization/src/elements/VStatementItemList.h"
#include "OOVisualization/src/alternative/VKeywordMethodCall.h"

#include "OOInteraction/src/expression_editor/OOExpressionBuilder.h"
#include "OOInteraction/src/customization/CustomizationVisitor.h"

using namespace OOModel;
using namespace Visualization;
using namespace OOInteraction;
using namespace OOVisualization;

namespace ContractsLibrary {

static const bool DISABLE_ALL_CUSTOMIZATIONS = false;
static const bool DISABLE_KEYWORD_VISUALIZATION_ANNOTATIONS = false;
static const bool DISABLE_KEYWORD_INPUT_ANNOTATIONS = false;
static const bool DISABLE_METHOD_ADDONS = false;
static const bool DISABLE_VALUE_AT_RETURN_VISITOR = false;
static const bool DISABLE_FILTERS = false;

Module* createContractsLibrary()
{
	Module* module = new Module{"CodeContracts"};
	module->extension<Position>()->set(1, 0);

	Class* contract = new Class{"Contract"};
	module->classes()->append(contract);

	Method* req = new Method{"Requires", Modifier::Public | Modifier::Static};
	contract->methods()->append(req);
	req->arguments()->append( new FormalArgument{"precondition", new PrimitiveTypeExpression{PrimitiveType::BOOLEAN}} );
	req->extension<Position>()->set(0, 0);

	Method* ens = new Method{"Ensures", Modifier::Public | Modifier::Static};
	contract->methods()->append(ens);
	ens->arguments()->append( new FormalArgument{"postcondition", new PrimitiveTypeExpression{PrimitiveType::BOOLEAN}} );
	std::unique_ptr<Position>(ens->extension<Position>())->set(0, 1);

	Method* res = new Method{"Result", Modifier::Public | Modifier::Static};
	contract->methods()->append(res);
	res->typeArguments()->append( new FormalTypeArgument{"T"} );
	std::unique_ptr<Position>(res->extension<Position>())->set(0, 2);

	Method* old = new Method{"OldValue", Modifier::Public | Modifier::Static};
	contract->methods()->append(old);
	old->typeArguments()->append(new FormalTypeArgument{"T"});
	old->arguments()->append( new FormalArgument{"variable", new ReferenceExpression{"T"}} );
	std::unique_ptr<Position>(old->extension<Position>())->set(0, 3);

	Method* out = new Method{"ValueAtReturn", Modifier::Public | Modifier::Static};
	contract->methods()->append(out);
	out->typeArguments()->append( new FormalTypeArgument{"T"} );
	out->arguments()->append( new FormalArgument{"argument", new ReferenceExpression{"T"}} );
	std::unique_ptr<Position>(out->extension<Position>())->set(0, 4);

	Method* contractClass = new Method{"ContractClass", Modifier::Public | Modifier::Static};
	contract->methods()->append(contractClass);
	contractClass->arguments()->append( new FormalArgument{"class", new ReferenceExpression{"Class"}} );
	std::unique_ptr<Position>(contractClass->extension<Position>())->set(0, 5);

	Method* contractClassFor = new Method{"ContractClassFor", Modifier::Public | Modifier::Static};
	contract->methods()->append(contractClassFor);
	contractClassFor->arguments()->append( new FormalArgument{"class", new ReferenceExpression{"Class"}} );
	std::unique_ptr<Position>(contractClassFor->extension<Position>())->set(0, 6);

	Method* forAllCollection = new Method{"ForAll", Modifier::Public | Modifier::Static};
	contract->methods()->append(forAllCollection);
	forAllCollection->typeArguments()->append( new FormalTypeArgument{"T"});
	forAllCollection->arguments()->append( new FormalArgument{"collection",
			OOExpressionBuilder::getOOExpression("IEnumerable<T>")} );
	forAllCollection->arguments()->append( new FormalArgument{"predicate",
			OOExpressionBuilder::getOOExpression("Predicate<T>")} );
	std::unique_ptr<Position>(forAllCollection->extension<Position>())->set(0, 7);

	Method* forAllRange = new Method{"ForAll", Modifier::Public | Modifier::Static};
	contract->methods()->append(forAllRange);
	forAllRange->arguments()->append( new FormalArgument{"begin", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	forAllRange->arguments()->append( new FormalArgument{"end", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	forAllRange->arguments()->append( new FormalArgument{"predicate",
			OOExpressionBuilder::getOOExpression("Predicate<int>")} );
	std::unique_ptr<Position>(forAllRange->extension<Position>())->set(0, 8);

	Method* existsCollection = new Method{"Exists", Modifier::Public | Modifier::Static};
	contract->methods()->append(existsCollection);
	existsCollection->typeArguments()->append( new FormalTypeArgument{"T"});
	existsCollection->arguments()->append( new FormalArgument{"collection",
			OOExpressionBuilder::getOOExpression("IEnumerable<T>")} );
	existsCollection->arguments()->append( new FormalArgument{"predicate",
			OOExpressionBuilder::getOOExpression("Predicate<T>")} );
	std::unique_ptr<Position>(existsCollection->extension<Position>())->set(0, 9);

	Method* existsRange = new Method{"Exists", Modifier::Public | Modifier::Static};
	contract->methods()->append(existsRange);
	existsRange->arguments()->append( new FormalArgument{"begin", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	existsRange->arguments()->append( new FormalArgument{"end", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	existsRange->arguments()->append( new FormalArgument{"predicate",
			OOExpressionBuilder::getOOExpression("Predicate<int>")} );
	std::unique_ptr<Position>(existsRange->extension<Position>())->set(0, 10);

	// DO NOT DELETE THE COMMENTED CODE BELOW
	// It can be used as a good example of how to do visualization groups with a condition.
	// Plus it is a nice example of what code was substituted by just using annotations within the code.


	// Register a group that holds the guard condition: are we visualizing a method belonging to the Contract class?
//	auto g = new VisualizationGroup{};
//	g->setConditionFunction([=](Visualization::Item*, Model::Node* node) -> bool
//	{
//		auto call = static_cast<OOModel::MethodCallExpression*>(node);
//		if (auto def = call->methodDefinition())
//		{
//			auto* p = def->parent();
//			while (p)
//			{
//				if (p == contract) return true;
//				p = p->parent();
//			}
//		}
//
//		return false;
//	});
//
//	// Register Visualizations in the group
//	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
//			{
//				return new VKeywordMethodCall{parent, static_cast<MethodCallExpression*> (node),
//						VKeywordMethodCall::itemStyles().get("requires")};
//			},
//			[=](Visualization::Item*, Model::Node* node) -> bool
//			{
//				auto call = static_cast<OOModel::MethodCallExpression*>(node);
//				return call->methodDefinition() == req;
//			});
//	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
//			{
//				return new VKeywordMethodCall{parent, static_cast<MethodCallExpression*> (node),
//						VKeywordMethodCall::itemStyles().get("ensures")};
//			},
//			[=](Visualization::Item*, Model::Node* node) -> bool
//			{
//				auto call = static_cast<OOModel::MethodCallExpression*>(node);
//				return call->methodDefinition() == ens;
//			});
//	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
//			{
//				return new VKeywordMethodCall{parent, static_cast<MethodCallExpression*> (node),
//						VKeywordMethodCall::itemStyles().get("old")};
//			},
//			[=](Visualization::Item*, Model::Node* node) -> bool
//			{
//				auto call = static_cast<OOModel::MethodCallExpression*>(node);
//				return call->methodDefinition() == old;
//			});
//	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
//			{
//				return new VKeywordMethodCall{parent, static_cast<MethodCallExpression*> (node),
//						VKeywordMethodCall::itemStyles().get("result")};
//			},
//			[=](Visualization::Item*, Model::Node* node) -> bool
//			{
//				auto call = static_cast<OOModel::MethodCallExpression*>(node);
//				return call->methodDefinition() == res;
//			});
//	g->addVisualization([](Visualization::Item* parent, Model::Node* node) -> Item*
//			{
//				return new VKeywordMethodCall{parent, static_cast<MethodCallExpression*> (node),
//						VKeywordMethodCall::itemStyles().get("out")};
//			},
//			[=](Visualization::Item*, Model::Node* node) -> bool
//			{
//				auto call = static_cast<OOModel::MethodCallExpression*>(node);
//				return call->methodDefinition() == out;
//			});
//
//	Scene::defaultRenderer()->registerGroup(MethodCallExpression::typeIdStatic(), g);
//
//	// Register custom input
//	CommandDescriptor::registerCommand(new CreateMethodCall{"requires", "Contract.Requires"});
//	CommandDescriptor::registerCommand(new CreateMethodCall{"ensures", "Contract.Ensures"});
//	CommandDescriptor::registerCommand(new CreateMethodCall{"old", "Contract.OldValue"});
//	CommandDescriptor::registerCommand(new CreateMethodCall{"result", "Contract.Result", 1});

	// Customizations
	if (!DISABLE_ALL_CUSTOMIZATIONS)
	{
		// Let items know what is the contractClass
		ContractFilter::setContractsClass(contract);

		if (!DISABLE_KEYWORD_VISUALIZATION_ANNOTATIONS)
		{
			req->annotations()->append(new ExpressionStatement{
				OOExpressionBuilder::getOOExpression("EnvisionKeywordVisualization(\"contracts/requires\")")});
			ens->annotations()->append(new ExpressionStatement{
				OOExpressionBuilder::getOOExpression("EnvisionKeywordVisualization(\"contracts/ensures\")")});
			res->annotations()->append(new ExpressionStatement{
				OOExpressionBuilder::getOOExpression("EnvisionKeywordVisualization(\"contracts/result\")")});
			old->annotations()->append(new ExpressionStatement{
				OOExpressionBuilder::getOOExpression("EnvisionKeywordVisualization(\"contracts/old\")")});
			out->annotations()->append(new ExpressionStatement{
				OOExpressionBuilder::getOOExpression("EnvisionKeywordVisualization(\"contracts/out\")")});
		}

		if (!DISABLE_KEYWORD_INPUT_ANNOTATIONS)
		{
			req->annotations()->append(new ExpressionStatement{
				OOExpressionBuilder::getOOExpression("EnvisionShortcut(\"requires\")")});
			ens->annotations()->append(new ExpressionStatement{
				OOExpressionBuilder::getOOExpression("EnvisionShortcut(\"ensures\")")});
			res->annotations()->append(new ExpressionStatement{
				OOExpressionBuilder::getOOExpression("EnvisionShortcut(\"result\",1)")});
			old->annotations()->append(new ExpressionStatement{
				OOExpressionBuilder::getOOExpression("EnvisionShortcut(\"old\")")});
		}


		if (!DISABLE_VALUE_AT_RETURN_VISITOR) ValueAtReturnVisitor::setMethods(ens, out);
		if (!DISABLE_METHOD_ADDONS)
		{
			// Register method add-ons
			VMethod::addAddOn( new InterfaceContractsVMethodAddOn{contractClass} );
			VMethod::addAddOn( new SignatureContractsVMethodAddOn{contractClass} );
		}

		if (!DISABLE_FILTERS)
		{
			// Install filters
			OOVisualization::VStatementItemList::addRangeFilter( ContractFilter::showOnlyContractsFilter );
			OOVisualization::VStatementItemList::addRangeFilter( ContractFilter::hideContractsFilter );
		}
	}

	return module;
}

Class* createBaseClass()
{
	Class* car = new Class{"Car", Modifier::Public};
	std::unique_ptr<Position>(car->extension<Position>())->set(0, 0);

	auto *fuel = new Field{ "fuel", new PrimitiveTypeExpression{PrimitiveType::INT}, Modifier::Public};
	car->fields()->append(fuel);

	auto *travel = new Method{"travel", Modifier::Public};
	car->methods()->append(travel);
	travel->arguments()->append( new FormalArgument{"numPassengers", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	travel->results()->append( new FormalResult{QString{}, new PrimitiveTypeExpression{PrimitiveType::INT}} );

	travel->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
			"Contract.Requires(fuel>0)")});

	travel->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
			"Contract.Requires(numPassengers>0)")});

	travel->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
			"Contract.Ensures(fuel<Contract.OldValue(fuel))")});

	travel->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
			"Contract.Ensures(Contract.Result<int>()>0)")});

	return car;
}

Class* createDerivedClass()
{
	Class* car = new Class{"SelfDrivingCar", Modifier::Public};
	std::unique_ptr<Position>(car->extension<Position>())->set(0, 1);
	car->baseClasses()->append(new ReferenceExpression{"Car"});

	auto *travel = new Method{"travel", Modifier::Public};
	car->methods()->append(travel);
	travel->arguments()->append( new FormalArgument{"numPassengers", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	travel->results()->append( new FormalResult{QString{}, new PrimitiveTypeExpression{PrimitiveType::INT}} );

	travel->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
			"Contract.Requires(numPassengers>=0)")});

	return car;
}

Class* createDerivedDerivedClass()
{
	Class* car = new Class{"SelfDrivingBus", Modifier::Public};
	std::unique_ptr<Position>(car->extension<Position>())->set(0, 2);
	car->baseClasses()->append(new ReferenceExpression{"SelfDrivingCar"});

	auto *travel = new Method{"travel", Modifier::Public};
	car->methods()->append(travel);
	travel->arguments()->append( new FormalArgument{"numPassengers", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	travel->results()->append( new FormalResult{QString{}, new PrimitiveTypeExpression{PrimitiveType::INT}} );

	return car;
}

Class* createInterface()
{
	Class* interface = new Class{"ICalc", Modifier::Public};
	std::unique_ptr<Position>(interface->extension<Position>())->set(1, 0);
	interface->annotations()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
			"Contract.ContractClass(ICalcContracts)")});

	auto *op = new Method{"op", Modifier::Public};
	interface->methods()->append(op);
	op->arguments()->append( new FormalArgument{"x", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	op->arguments()->append( new FormalArgument{"y", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	op->results()->append( new FormalResult{QString{}, new PrimitiveTypeExpression{PrimitiveType::INT}} );

	return interface;
}

Class* createInterfaceContracts()
{
	Class* calcContracts = new Class{"ICalcContracts", Modifier::Public};
	std::unique_ptr<Position>(calcContracts->extension<Position>())->set(1, 1);
	calcContracts->annotations()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
				"Contract.ContractClassFor(ICalc)")});
	calcContracts->baseClasses()->append(OOExpressionBuilder::getOOExpression("ICalc"));

	auto *op = new Method{"op", Modifier::Public};
	calcContracts->methods()->append(op);
	op->arguments()->append( new FormalArgument{"x", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	op->arguments()->append( new FormalArgument{"y", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	op->results()->append( new FormalResult{QString{}, new PrimitiveTypeExpression{PrimitiveType::INT}} );
	op->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
				"Contract.Requires(x!=y)")});
	op->items()->append(new ReturnStatement{ OOExpressionBuilder::getOOExpression("0")});

	return calcContracts;
}

Method* createMinMax()
{
	auto *minMax = new Method{"min_max", Modifier::Public};
	minMax->arguments()->append( new FormalArgument{"a", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	minMax->arguments()->append( new FormalArgument{"b", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	minMax->arguments()->append(
			new FormalArgument{"min", new PrimitiveTypeExpression{PrimitiveType::INT}, FormalArgument::OUT} );
	minMax->arguments()->append(
			new FormalArgument{"max", new PrimitiveTypeExpression{PrimitiveType::INT}, FormalArgument::OUT} );

	minMax->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
			"Contract.Ensures(Contract.ValueAtReturn(min)<="
			"Contract.ValueAtReturn(max))")});

	auto i = new IfStatement{};
	minMax->items()->append(i);
	i->setCondition( OOExpressionBuilder::getOOExpression("a>b"));
	i->thenBranch()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("max=a")});
	i->thenBranch()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("min=b")});
	i->elseBranch()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("max=b")});
	i->elseBranch()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("min=a")});

	return minMax;
}

Method* createFactorial()
{
	auto *fact = new Method{"factorial", Modifier::Public};
	fact->arguments()->append( new FormalArgument{"x", new PrimitiveTypeExpression{PrimitiveType::INT}} );
	fact->results()->append( new FormalResult{QString{}, new PrimitiveTypeExpression{PrimitiveType::INT}} );
	fact->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
				"Contract.Requires(x>=0)")});
	fact->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
			"Contract.Ensures(Contract.Result<int>()>0)")});
	fact->items()->append(new ReturnStatement{ OOExpressionBuilder::getOOExpression(
				"x<=1?1:x*factorial(x-1)")});

	return fact;
}

Method* createAppend()
{
	auto *app = new Method{"append", Modifier::Public};
		app->arguments()->append( new FormalArgument{"x", new PrimitiveTypeExpression{PrimitiveType::INT}} );
		app->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
					"Contract.Ensures(elements[size-1]==x)")});
		app->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
				"Contract.Ensures(size==Contract.OldValue(size)+1)")});
		app->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression(
					"elements[size++]=x")});

	return app;
}


Class* createPaperClass()
{
	Class* paperClass = new Class{"Paper", Modifier::Public};
	std::unique_ptr<Position>(paperClass->extension<Position>())->set(2, 0);

	auto *minMax = createMinMax();
	paperClass->methods()->append(minMax);
	std::unique_ptr<Position>(minMax->extension<Position>())->set(0, 0);

	auto *fact = createFactorial();
	paperClass->methods()->append(fact);
	std::unique_ptr<Position>(fact->extension<Position>())->set(0, 1);

	auto *app = createAppend();
	paperClass->methods()->append(app);
	std::unique_ptr<Position>(app->extension<Position>())->set(0, 2);

	return paperClass;
}

Module* createClientModule()
{
	Module* module = new Module{"Client"};
	std::unique_ptr<Position>(module->extension<Position>())->set(0, 0);

	module->classes()->append( createBaseClass());
	module->classes()->append( createDerivedClass() );
	module->classes()->append( createDerivedDerivedClass() );
	module->classes()->append( createInterface() );
	module->classes()->append( createInterfaceContracts() );

	// Choose either the class or a method
	module->classes()->append( createPaperClass() );
//	module->methods()->append( createMinMax() );
//	module->methods()->append( createFactorial() );
//	module->methods()->append( createAppend() );

	auto ref = DCast<ReferenceExpression>(OOExpressionBuilder::getOOExpression("CodeContracts.Contract"));
	Q_ASSERT(ref);
	module->subDeclarations()->append(new NameImport{ref});

	return module;
}

class ContractsLibraryTest : public Test<ContractsLibraryPlugin, ContractsLibraryTest> { public: void test()
{
	////////////////////////////////////////////////// Create Manager

	// Create project
	auto prj = new Project{"CustomizationDemo"};
	auto client = createClientModule();
	prj->modules()->append( client );
	auto contracts = createContractsLibrary();
	prj->modules()->append( contracts );

	auto manager = new Model::TreeManager{prj};

	VisualizationManager::instance().mainScene()->addTopLevelNode(prj);
//	VisualizationManager::instance().mainScene()->addTopLevelNode(client);
//	VisualizationManager::instance().mainScene()->addTopLevelNode(client->classes()->at(0));
//	VisualizationManager::instance().mainScene()->addTopLevelNode(client->methods()->at(0));
//	VisualizationManager::instance().mainScene()->addTopLevelNode(
//			contracts->classes()->at(0)->methods()->at(0));

	VisualizationManager::instance().mainScene()->listenToTreeManager(manager);

	CustomizationVisitor customizations;
	customizations.visit(prj);

	CHECK_CONDITION(prj != nullptr);
}};

}
