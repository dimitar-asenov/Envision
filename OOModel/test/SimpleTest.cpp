/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oomodel.h"
#include "SelfTest/headers/SelfTestSuite.h"

namespace OOModel {

TEST(OOModel, SimpleTest)
{
	CHECK_INT_EQUAL(1,1);
}

}
