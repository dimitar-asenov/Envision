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
#include "Box.h"

#include <QtCore/QCoreApplication>

namespace Visualization {

TEST(VisualizationBase, ShowView)
{
	Scene* scene = new Scene();

	Box* b = new Box(NULL, 3);
	scene->addItem(b);
	b->updateSubtreeState();

	View* view = new View(scene);

	CHECK_INT_EQUAL(1, 1);

	CHECK_CONDITION(view != NULL);
	//delete view;
	//delete scene;
}

}
