/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "custommethodcall.h"
#include "SelfTest/headers/SelfTestSuite.h"

namespace CustomMethodCall {

TEST(CustomMethodCall, SimpleTest)
{
	CHECK_INT_EQUAL(1,1);
}

}
