/***********************************************************************************************************************
 * SwitchStatement.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SWITCHSTATEMENT_H_
#define SWITCHSTATEMENT_H_

#include "Statement.h"

#include "../FormalArgument.h"
#include "../expressions/Expression.h"
#include "SwitchCase.h"

#include "ModelBase/headers/nodes/TypedList.h"


namespace OOModel {

class OOMODEL_API SwitchStatement: public Statement
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(SwitchStatement)

	ATTRIBUTE(Expression, switchVar, setSwitchVar);
	ATTRIBUTE(Model::TypedList<SwitchCase>, cases, setCases)
};

}

#endif /* SWITCHSTATEMENT_H_ */
