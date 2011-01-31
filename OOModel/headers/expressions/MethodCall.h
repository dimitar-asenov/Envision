/***********************************************************************************************************************
 * MethodCall.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef METHODCALL_H_
#define METHODCALL_H_

#include "Expression.h"
#include "../OOReference.h"
#include "../types/Type.h"
#include "ModelBase/headers/nodes/TypedList.h"

namespace OOModel {

class OOMODEL_API MethodCall: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(MethodCall)

	ATTRIBUTE(Expression, prefix)
	ATTRIBUTE(OOReference, ref)
	ATTRIBUTE(Model::TypedList<Expression>, arguments)
};

}

#endif /* METHODCALL_H_ */
