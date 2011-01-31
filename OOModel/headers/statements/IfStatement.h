/***********************************************************************************************************************
 * IfStatement.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef IFSTATEMENT_H_
#define IFSTATEMENT_H_

#include "Statement.h"

#include "../expressions/Expression.h"

namespace OOModel {

class OOMODEL_API IfStatement: public Statement
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(IfStatement)

	ATTRIBUTE(Expression, condition);
	ATTRIBUTE(Statement, thenBranch);
	ATTRIBUTE(Statement, elseBranch);
};

}

#endif /* IFSTATEMENT_H_ */
