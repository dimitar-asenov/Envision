/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "interactionbase.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "VisualizationBase/headers/VisualizationManager.h"
#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/views/MainView.h"
#include "VisualizationBase/headers/items/VText.h"
#include "VisualizationBase/headers/items/VExtendable.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/ModelRenderer.h"

#include "ModelBase/headers/test_nodes/BinaryNode.h"
#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/List.h"
#include "ModelBase/headers/Model.h"

#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtCore/QCoreApplication>

namespace Interaction {

using namespace Visualization;

TEST(InteractionBase, TextSelect)
{
	Scene* scene = new Scene();
	ModelRenderer* renderer = new ModelRenderer();

	renderer->registerVisualization(TestNodes::BinaryNode::getTypeIdStatic(), createVisualization<VExtendable, TestNodes::BinaryNode>);
	renderer->registerVisualization(Model::Text::getTypeIdStatic(), createVisualization<VText, Model::Text>);
	renderer->registerVisualization(Model::List::getTypeIdStatic(), createVisualization<VList, Model::List>);

	scene->setRenderer(renderer);

	Model::Model* model = new Model::Model();
	Model::List* list = static_cast<Model::List*> (model->createRoot("List"));

	model->beginModification(list, "set");
	TestNodes::BinaryNode* first = list->append<TestNodes::BinaryNode>();
	TestNodes::BinaryNode* second = list->append<TestNodes::BinaryNode>();
	Model::Text* third = list->append<Model::Text>();

	first->name()->set("First node");
	TestNodes::BinaryNode* left = first->makeLeftNode("BinaryNode");
	TestNodes::BinaryNode* right = first->makeRightNode("BinaryNode");
	left->name()->set("left node");
	right->name()->set("right node");

	second->name()->set("Empty node");

	third->set("Some independent text");
	model->endModification();

	VList* l = dynamic_cast<VList*> (renderer->render(NULL, list));
	scene->addTopLevelItem(l);
	scene->scheduleUpdate();
	QApplication::processEvents();

	l->at<VExtendable>(0)->setExpanded();
	scene->scheduleUpdate();

	// Create view
	MainView* view = new MainView(scene);
	VisualizationManager::instance().getMainWindow()->resize(1200,700);

	// Center Window
	QRect descktop( QApplication::desktop()->screenGeometry() );
	int leftPos = descktop.width()/2-VisualizationManager::instance().getMainWindow()->width()/2;
	int topPos = descktop.height()/2-VisualizationManager::instance().getMainWindow()->height()/2;
	VisualizationManager::instance().getMainWindow()->move(leftPos,topPos);

	CHECK_CONDITION(view != NULL);
}

}
