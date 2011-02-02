/***********************************************************************************************************************
 * Position.h
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef POSITION_H_
#define POSITION_H_

#include "../visualizationbase_api.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/Integer.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace Visualization {

class VISUALIZATIONBASE_API Position
{
	DECLARE_EXTENSION(Position)

	EXTENSION_ATTRIBUTE_VALUE(Model::Integer, x, setX, int)
	EXTENSION_ATTRIBUTE_VALUE(Model::Integer, y, setY, int)
};

}

#endif /* POSITION_H_ */
