/***********************************************************************************************************************
 * oomodel.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oomodel.h"
#include "SelfTest/headers/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( oomodel, OOModel::OOModel )

namespace OOModel {

bool OOModel::initialize(Envision::EnvisionManager&)
{
	return true;
}

void OOModel::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOModel>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOModel>::runTest(testid).printResultStatistics();
}

}
