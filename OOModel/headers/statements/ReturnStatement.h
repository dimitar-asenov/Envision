/***********************************************************************************************************************
 * ReturnStatement.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef RETURNSTATEMENT_H_
#define RETURNSTATEMENT_H_

#include "Statement.h"
#include "../expressions/Expression.h"

#include "ModelBase/headers/nodes/TypedList.h"

namespace OOModel {

class OOMODEL_API ReturnStatement: public Statement
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(ReturnStatement)

	ATTRIBUTE(Model::TypedList<Expression>, values, setValues);
};

}

#endif /* RETURNSTATEMENT_H_ */
