/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "visualizationbase.h"
#include "VisualizationManager.h"
#include "Scene.h"
#include "views/MainView.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "BoxTest.h"
#include "items/VText.h"
#include "items/VExtendable.h"
#include "items/VList.h"
#include "ModelRenderer.h"

#include "ModelBase/headers/test_nodes/BinaryNode.h"

#include "ModelBase/headers/nodes/Text.h"
#include "ModelBase/headers/nodes/List.h"
#include "ModelBase/headers/Model.h"

#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtCore/QCoreApplication>

namespace Visualization {

//TEST(VisualizationBase, DoStuff)
//{
//	Scene* scene = new Scene();
//
//	BoxTest* b = new BoxTest(NULL, 3);
//	scene->addItem(b);
//	b->updateSubtreeState();
//
//	View* view = new View(scene);
//	view->setRenderHint(QPainter::Antialiasing);
//	view->setRenderHint(QPainter::TextAntialiasing);
//	view->parentWidget()->resize(600,500);
//
//	QRect descktop( QApplication::desktop()->screenGeometry() );
//	int left = descktop.width()/2-view->parentWidget()->width()/2;
//	int top = descktop.height()/2-view->parentWidget()->height()/2;
//	view->parentWidget()->move(left,top);
//
//	CHECK_INT_EQUAL(1, 1);
//
//	CHECK_CONDITION(view != NULL);
//}

//TEST(VisualizationBase, ModelTextTest)
//{
//	Scene* scene = new Scene();
//
//	Model::Model model;
//	Model::Text* text = static_cast<Model::Text*> (model.createRoot("Text"));
//
//	model.beginModification(text, "set");
//	text->set("This is a test");
//	model.endModification();
//
//	VText* t = new VText(NULL, text);
//	scene->addItem(t);
//	t->updateSubtreeState();
//
//	View* view = new View(scene);
//	view->setRenderHint(QPainter::Antialiasing);
//	view->setRenderHint(QPainter::TextAntialiasing);
//	view->parentWidget()->resize(600,500);
//
//	QRect descktop( QApplication::desktop()->screenGeometry() );
//	int left = descktop.width()/2-view->parentWidget()->width()/2;
//	int top = descktop.height()/2-view->parentWidget()->height()/2;
//	view->parentWidget()->move(left,top);
//
//	CHECK_INT_EQUAL(1, 1);
//
//	CHECK_CONDITION(view != NULL);
//}

TEST(VisualizationBase, ExtendableTest)
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
	scene->updateTopLevelItems();
	QApplication::processEvents();

	l->at<VExtendable>(0)->setExpanded();
	scene->updateTopLevelItems();

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
