/***********************************************************************************************************************
 * VariableAccess.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VARIABLEACCESS_H_
#define VARIABLEACCESS_H_

#include "Expression.h"
#include "ModelBase/headers/nodes/Reference.h"

namespace OOModel {

class Class;

class OOMODEL_API VariableAccess: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(VariableAccess)

	ATTRIBUTE(Expression, prefix, setPrefix)
	ATTRIBUTE(Model::Reference, ref, setRef)

	public:
		virtual Class* classDefinition();
};

}

#endif /* VARIABLEACCESS_H_ */
