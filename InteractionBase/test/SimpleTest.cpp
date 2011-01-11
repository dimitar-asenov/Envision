/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "interactionbase.h"
#include "SelfTest/headers/SelfTestSuite.h"

namespace Interaction {

TEST(InteractionBase, SimpleTest)
{
	CHECK_INT_EQUAL(1,1);
}

}
