/***********************************************************************************************************************
 * LoopStatement.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LOOPSTATEMENT_H_
#define LOOPSTATEMENT_H_

#include "Statement.h"

#include "../expressions/Expression.h"

namespace OOModel {

class OOMODEL_API LoopStatement: public Statement
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(LoopStatement)

	ATTRIBUTE(Expression, condition);
	ATTRIBUTE(Statement, initStep);
	ATTRIBUTE(Statement, updateStep);
	ATTRIBUTE(Statement, body);
};

}

#endif /* LOOPSTATEMENT_H_ */
