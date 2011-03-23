/***********************************************************************************************************************
 * VariableDeclaration.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VARIABLEDECLARATION_H_
#define VARIABLEDECLARATION_H_

#include "Statement.h"

#include "../attributeMacros.h"
#include "../types/Type.h"
#include "../expressions/Expression.h"

#include "ModelBase/headers/nodes/Text.h"

namespace OOModel {

class OOMODEL_API VariableDeclaration: public Statement
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(VariableDeclaration)

	ATTRIBUTE_OOP_NAME
	ATTRIBUTE(Type, type, setType)
	ATTRIBUTE(Expression, initialValue, setInitialValue);

	public:
		virtual bool definesSymbol() const;
		virtual const QString& symbolName() const;
};

}

#endif /* VARIABLEDECLARATION_H_ */
