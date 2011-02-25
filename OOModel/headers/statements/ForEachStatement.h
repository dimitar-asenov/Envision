/***********************************************************************************************************************
 * ForEachStatement.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef FOREACHSTATEMENT_H_
#define FOREACHSTATEMENT_H_

#include "Statement.h"

#include "../elements/FormalArgument.h"
#include "../expressions/Expression.h"

#include "ModelBase/headers/nodes/TypedList.h"

namespace OOModel {

class OOMODEL_API ForEachStatement: public Statement
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(ForEachStatement)

	ATTRIBUTE_VALUE_CUSTOM_RETURN(::Model::Text, varName, setVarName, QString, const QString&)
	ATTRIBUTE(Type, varType, setVarType)
	ATTRIBUTE(Expression, collection, setCollection);
	ATTRIBUTE(Model::TypedList<StatementItem>, body, setBody);
};

}

#endif /* FOREACHSTATEMENT_H_ */
