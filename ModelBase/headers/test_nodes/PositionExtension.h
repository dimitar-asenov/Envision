/***********************************************************************************************************************
 * PositionExtension.h
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef POSITIONEXTENSION_H_
#define POSITIONEXTENSION_H_

#include "../modelbase_api.h"
#include "../nodes/Extendable/ExtendableNode.h"
#include "../nodes/Integer.h"
#include "../nodes/nodeMacros.h"

namespace TestNodes {

class MODELBASE_API PositionExtension
{
	DECLARE_EXTENSION(PositionExtension)

	EXTENSION_ATTRIBUTE_VALUE(Model::Integer, x, setX, int)
	EXTENSION_ATTRIBUTE_VALUE(Model::Integer, y, setY, int)
};

}

#endif /* POSITIONEXTENSION_H_ */
