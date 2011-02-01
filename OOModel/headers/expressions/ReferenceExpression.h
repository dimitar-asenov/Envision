/***********************************************************************************************************************
 * ReferenceExpression.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef REFERENCEEXPRESSION_H_
#define REFERENCEEXPRESSION_H_

#include "Expression.h"
#include "../OOReference.h"

namespace OOModel {

class OOMODEL_API ReferenceExpression: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(ReferenceExpression)

	ATTRIBUTE(OOReference, ref, setRef)
};

}

#endif /* REFERENCEEXPRESSION_H_ */
