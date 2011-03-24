/***********************************************************************************************************************
 * ReferenceExpression.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef REFERENCEEXPRESSION_H_
#define REFERENCEEXPRESSION_H_

#include "Expression.h"

#include "ModelBase/headers/nodes/Reference.h"

namespace OOModel {

class Class;

class OOMODEL_API ReferenceExpression: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(ReferenceExpression)

	ATTRIBUTE(Expression, prefix, setPrefix)
	ATTRIBUTE(Model::Reference, ref, setRef)

	public:
		virtual Class* classDefinition();
};

}

#endif /* REFERENCEEXPRESSION_H_ */
