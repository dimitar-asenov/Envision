/***********************************************************************************************************************
 * Position.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "node_extensions/Position.h"

namespace Visualization {

DEFINE_EXTENSION(Position)
REGISTER_EXTENSION_ATTRIBUTE(Position, x, Integer, false, false, true)
REGISTER_EXTENSION_ATTRIBUTE(Position, y, Integer, false, false, true)

}
