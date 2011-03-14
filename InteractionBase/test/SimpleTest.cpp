/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "interactionbase.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/views/MainView.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/items/VExtendable.h"

#include "ModelBase/headers/test_nodes/BinaryNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/List.h"
#include "ModelBase/headers/Model.h"

#include <QtGui/QApplication>

namespace Interaction {

using namespace Visualization;

TEST(InteractionBase, TextSelect)
{
	Scene* scene = new Scene();

	Model::Model* model = new Model::Model();
	Model::List* list = static_cast<Model::List*> (model->createRoot("List"));

	model->beginModification(list, "set");
	TestNodes::BinaryNode* first = list->append<TestNodes::BinaryNode>();
	TestNodes::BinaryNode* second = list->append<TestNodes::BinaryNode>();
	Model::Text* third = list->append<Model::Text>();

	first->name()->set("First node");
	TestNodes::BinaryNode* left = first->setLeft<TestNodes::BinaryNode>();
	TestNodes::BinaryNode* right = first->setRight<TestNodes::BinaryNode>();
	left->name()->set("left node");
	right->name()->set("right node");

	second->name()->set("Empty node");

	third->set("Some independent text");
	model->endModification();

	VList* l = dynamic_cast<VList*> (scene->renderer()->render(NULL, list));
	scene->addTopLevelItem(l);
	scene->scheduleUpdate();
	QApplication::processEvents();

	l->at<VExtendable>(0)->setExpanded();
	scene->scheduleUpdate();
	scene->listenToModel(model);

	// Create view
	MainView* view = new MainView(scene);
	CHECK_CONDITION(view != NULL);
}

}
