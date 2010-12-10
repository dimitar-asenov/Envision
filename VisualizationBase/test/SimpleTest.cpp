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
	scene->addItem(new Box(NULL, 10));
	new View(scene);

	CHECK_INT_EQUAL(1, 1);
}

}
