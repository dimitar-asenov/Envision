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

namespace OOModel {

class OOMODEL_API ReturnStatement: public Statement
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(ReturnStatement)

	ATTRIBUTE(Expression, value);
};

}

#endif /* RETURNSTATEMENT_H_ */
