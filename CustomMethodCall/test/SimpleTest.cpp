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

	Method* insert = col->methods()->append<Method>();
	insert->setName("insert");
	insert->extension<CustomVisualization>()->setVisName("InsertMethodVis");
	insert->extension<Position>()->setY(80);

	Method* empty = col->methods()->append<Method>();
	empty->setName("empty");
	empty->extension<CustomVisualization>()->setVisName("EmptyMethodVis");
	empty->extension<Position>()->setY(160);

	Method* exists = col->methods()->append<Method>();
	exists->setName("exists");
	exists->extension<CustomVisualization>()->setVisName("ExistsMethodVis");
	exists->extension<Position>()->setY(240);

	Method* test = col->methods()->append<Method>();
	test->setName("test");
	test->extension<Position>()->setX(200);

	/*MethodCallStatement* findCall = */test->items()->append<MethodCallStatement>();

	/*MethodCallStatement* insertCall = */test->items()->append<MethodCallStatement>();

	/*MethodCallStatement* emptyCall = */test->items()->append<MethodCallStatement>();

	/*MethodCallStatement* existsCall = */test->items()->append<MethodCallStatement>();


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
