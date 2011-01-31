/***********************************************************************************************************************
 * StringLiteral.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef STRINGLITERAL_H_
#define STRINGLITERAL_H_

#include "Expression.h"

#include "ModelBase/headers/nodes/Text.h"

namespace OOModel {

class OOMODEL_API StringLiteral: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(StringLiteral)

	ATTRIBUTE_VALUE_CUSTOM_RETURN(::Model::Text, value, setValue, QString, const QString&)
};

}

#endif /* STRINGLITERAL_H_ */
