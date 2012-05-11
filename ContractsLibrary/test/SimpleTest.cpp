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

using namespace OOModel;
using namespace Visualization;
using namespace OOInteraction;

namespace ContractsLibrary {

Library* createContractsLibrary()
{
	Library* lib = new Library();
	lib->setName("CodeContracts");
	Class* contract = new Class();
	lib->classes()->append(contract);
	contract->setName("Contract");

	Method* req = new Method();
	req->setName("Requires");
	contract->methods()->append(req);
	req->setVisibility(Visibility::PUBLIC);
	req->setStorageSpecifier(StorageSpecifier::CLASS_VARIABLE);
	auto *fa = new FormalArgument();
	fa->setTypeExpression(new PrimitiveTypeExpression(PrimitiveType::BOOLEAN));
	fa->setName("precondition");
	req->arguments()->append(fa);

	Method* ens = new Method();
	ens->setName("Ensures");
	contract->methods()->append(ens);
	ens->setVisibility(Visibility::PUBLIC);
	ens->setStorageSpecifier(StorageSpecifier::CLASS_VARIABLE);
	fa = new FormalArgument();
	fa->setTypeExpression(new PrimitiveTypeExpression(PrimitiveType::BOOLEAN));
	fa->setName("postcondition");
	ens->arguments()->append(fa);

	// Set positions
	lib->extension<Position>()->setX(540);
	contract->extension<Position>()->setY(0);
	req->extension<Position>()->setY(0);
	ens->extension<Position>()->setY(60);

	return lib;
}

Class* createBaseClass()
{
	Class* car = new Class();
	car->setName("Car");
	car->setVisibility(Visibility::PUBLIC);

	auto *fuel = new Field();
	fuel->setName("fuel");
	fuel->setTypeExpression(new PrimitiveTypeExpression(PrimitiveType::INT));
	fuel->setVisibility(Visibility::PUBLIC);
	car->fields()->append(fuel);

	auto *travel = new Method();
	travel->setName("travel");
	travel->setVisibility(Visibility::PUBLIC);
	car->methods()->append(travel);
	auto *fa = new FormalArgument();
	fa->setTypeExpression(new PrimitiveTypeExpression(PrimitiveType::INT));
	fa->setName("numPassengers");
	travel->arguments()->append(fa);

	travel->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
			"CodeContracts.Contract.Requires(fuel>0)")));

	travel->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
			"CodeContracts.Contract.Requires(numPassengers>0)")));

	return car;
}

Class* createDerivedClass()
{
	Class* car = new Class();
	car->setName("SelfDrivingCar");
	car->setVisibility(Visibility::PUBLIC);
	car->baseClasses()->append(new ReferenceExpression("Car"));

	auto *travel = new Method();
	travel->setName("travel");
	travel->setVisibility(Visibility::PUBLIC);
	car->methods()->append(travel);
	auto *fa = new FormalArgument();
	fa->setTypeExpression(new PrimitiveTypeExpression(PrimitiveType::INT));
	fa->setName("numPassengers");
	travel->arguments()->append(fa);

	travel->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression(
			"CodeContracts.Contract.Requires(numPassengers>=0)")));

	car->extension<Position>()->setY(160);

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
