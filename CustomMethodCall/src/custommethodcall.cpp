/***********************************************************************************************************************
 * custommethodcall.cpp
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "custommethodcall.h"
#include "SelfTest/headers/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( custommethodcall, CustomMethodCall::CustomMethodCall )

namespace CustomMethodCall {

bool CustomMethodCall::initialize(Envision::EnvisionManager&)
{
	return true;
}

void CustomMethodCall::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<CustomMethodCall>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<CustomMethodCall>::runTest(testid).printResultStatistics();
}

}
