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

#include "ModelBase/headers/nodes/TypedList.h"

namespace OOModel {

class OOMODEL_API LoopStatement: public Statement
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(LoopStatement)

	ATTRIBUTE(Expression, condition, setCondition);
	ATTRIBUTE(Statement, initStep, setInitStep);
	ATTRIBUTE(Statement, updateStep, setUpdateStep);
	ATTRIBUTE(Model::TypedList<StatementItem>, body, setBody);
};

}

#endif /* LOOPSTATEMENT_H_ */
