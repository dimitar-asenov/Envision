/***********************************************************************************************************************
 * PositionExtension.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "test_nodes/PositionExtension.h"

namespace TestNodes {

DEFINE_EXTENSION(PositionExtension)
REGISTER_EXTENSION_ATTRIBUTE(PositionExtension, x, Integer, false, false, true)
REGISTER_EXTENSION_ATTRIBUTE(PositionExtension, y, Integer, false, false, true)

}
