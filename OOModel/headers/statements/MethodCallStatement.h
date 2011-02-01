/***********************************************************************************************************************
 * MethodCallStatement.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef METHODCALLSTATEMENT_H_
#define METHODCALLSTATEMENT_H_

#include "Statement.h"

#include "../OOReference.h"
#include "../expressions/Expression.h"

#include "ModelBase/headers/nodes/TypedList.h"

namespace OOModel {

class OOMODEL_API MethodCallStatement: public Statement
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(MethodCallStatement)

	ATTRIBUTE(Expression, prefix, setPrefix)
	ATTRIBUTE(OOReference, ref, setRef)
	ATTRIBUTE(Model::TypedList<Expression>, arguments, setArguments)

	public:

};

}

#endif /* METHODCALLSTATEMENT_H_ */
