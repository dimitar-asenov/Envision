/***********************************************************************************************************************
 * ArrayType.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ARRAYTYPE_H_
#define ARRAYTYPE_H_

#include "Type.h"

namespace OOModel {

class OOMODEL_API ArrayType : public Type
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(ArrayType)

	ATTRIBUTE(Type, type, setType);
};

}

#endif /* ARRAYTYPE_H_ */
