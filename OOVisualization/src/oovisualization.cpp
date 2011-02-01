/***********************************************************************************************************************
 * oovisualization.cpp
 *
 *  Created on: Feb 01, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oovisualization.h"
#include "SelfTest/headers/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( oovisualization, OOVisualization::OOVisualization )

namespace OOVisualization {

bool OOVisualization::initialize(Envision::EnvisionManager&)
{
	return true;
}

void OOVisualization::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOVisualization>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOVisualization>::runTest(testid).printResultStatistics();
}

}
