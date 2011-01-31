/***********************************************************************************************************************
 * VariableDeclaration.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VARIABLEDECLARATION_H_
#define VARIABLEDECLARATION_H_

#include "Statement.h"

#include "../FormalArgument.h"
#include "../expressions/Expression.h"


namespace OOModel {

class OOMODEL_API VariableDeclaration: public Statement
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(VariableDeclaration)

	ATTRIBUTE(FormalArgument, var);
	ATTRIBUTE(Expression, initialValue);
};

}

#endif /* VARIABLEDECLARATION_H_ */
