/***********************************************************************************************************************
 * selftest.cpp
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "selftest.h"

Q_EXPORT_PLUGIN2( selftest, SelfTest::SelfTest )

namespace SelfTest {

bool SelfTest::initialize(Envision::EnvisionManager&)
{
	return true;
}

void SelfTest::selfTest(QString)
{
}

}
