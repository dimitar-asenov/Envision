/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "visualizationbase.h"
#include "VisualizationManager.h"
#include "Scene.h"
#include "View.h"
#include "SelfTest/headers/SelfTestSuite.h"
#include "BoxTest.h"

#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtCore/QCoreApplication>

namespace Visualization {

TEST(VisualizationBase, ShowView)
{
	Scene* scene = new Scene();

	BoxTest* b = new BoxTest(NULL, 3);
	scene->addItem(b);
	b->updateSubtreeState();

	View* view = new View(scene);
	view->setRenderHint(QPainter::Antialiasing);
	view->setRenderHint(QPainter::TextAntialiasing);
	view->parentWidget()->resize(600,500);

	QRect descktop( QApplication::desktop()->screenGeometry() );
	int left = descktop.width()/2-view->parentWidget()->width()/2;
	int top = descktop.height()/2-view->parentWidget()->height()/2;
	view->parentWidget()->move(left,top);

	CHECK_INT_EQUAL(1, 1);

	CHECK_CONDITION(view != NULL);
	//delete view;
	//delete scene;
}

}
