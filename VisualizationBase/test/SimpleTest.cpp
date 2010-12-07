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

#include <QtCore/QCoreApplication>

namespace Visualization {

TEST(VisualizationBase, ShowView)
{
	Scene* scene = new Scene();
	new View(scene);

	CHECK_INT_EQUAL(1, 1);
}

}
