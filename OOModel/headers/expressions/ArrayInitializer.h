/***********************************************************************************************************************
 * ArrayInitializer.h
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ARRAYINITIALIZER_H_
#define ARRAYINITIALIZER_H_

#include "Expression.h"

#include "ModelBase/headers/nodes/TypedList.h"

namespace OOModel {

class OOMODEL_API ArrayInitializer: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(ArrayInitializer)

	ATTRIBUTE(Model::TypedList<Expression>, values, setValues);
};

}

#endif /* ARRAYINITIALIZER_H_ */
