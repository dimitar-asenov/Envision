/***********************************************************************************************************************
 * controlflowvisualization.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "controlflowvisualization.h"
#include "SelfTest/headers/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( controlflowvisualization, ControlFlowVisualization::ControlFlowVisualization )

namespace ControlFlowVisualization {

bool ControlFlowVisualization::initialize(Envision::EnvisionManager&)
{
	return true;
}

void ControlFlowVisualization::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<ControlFlowVisualization>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<ControlFlowVisualization>::runTest(testid).printResultStatistics();
}

}
