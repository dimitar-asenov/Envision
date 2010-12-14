/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "visualizationbase.h"
#include "VisualizationManager.h"
#include "ModelScene.h"
#include "View.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "BoxTest.h"
#include "items/VText.h"
#include "items/VExtendable.h"
#include "ModelRenderer.h"

#include "BinaryNode.h"

#include "ModelBase/headers/nodes/Text.h"
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
//	//delete view;
//	//delete scene;
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
//	//delete view;
//	//delete scene;
//}

TEST(VisualizationBase, ExtendableTest)
{
	ModelScene* scene = new ModelScene();
	ModelRenderer* renderer = new ModelRenderer();

	renderer->registerVisualization(BinaryNode::getTypeIdStatic(), createVisualization<VExtendable, BinaryNode>);
	renderer->registerVisualization(Model::Text::getTypeIdStatic(), createVisualization<VText, Model::Text>);

	scene->setRenderer(renderer);

	Model::Model model;
	BinaryNode* root = static_cast<BinaryNode*> (model.createRoot("BinaryNode"));

	model.beginModification(root, "set");
	root->name()->set("Root node");
	BinaryNode* left = root->makeLeftNode("BinaryNode");
	BinaryNode* right = root->makeRightNode("BinaryNode");
	left->name()->set("left node");
	right->name()->set("right node");
	model.endModification();

	ModelItem* t = renderer->render(NULL, root);
	scene->addItem(t);
	t->updateSubtreeState();

	View* view = new View(scene);
	view->setRenderHint(QPainter::Antialiasing);
	view->setRenderHint(QPainter::TextAntialiasing);
	view->parentWidget()->resize(600,500);

	QRect descktop( QApplication::desktop()->screenGeometry() );
	int leftPos = descktop.width()/2-view->parentWidget()->width()/2;
	int topPos = descktop.height()/2-view->parentWidget()->height()/2;
	view->parentWidget()->move(leftPos,topPos);

	CHECK_INT_EQUAL(1, 1);

	CHECK_CONDITION(view != NULL);
	//delete view;
	//delete scene;
}

}
