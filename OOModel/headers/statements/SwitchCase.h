/***********************************************************************************************************************
 * SwitchCase.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SWITCHCASE_H_
#define SWITCHCASE_H_

#include "../oomodel_api.h"
#include "Statement.h"
#include "../elements/StatementItemList.h"
#include "../expressions/Expression.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class OOMODEL_API SwitchCase : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(SwitchCase)

	ATTRIBUTE(Expression, expr, setExpr)
	ATTRIBUTE(StatementItemList, statement, setStatement)
};

}

#endif /* SWITCHCASE_H_ */
