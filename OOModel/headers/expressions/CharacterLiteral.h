/***********************************************************************************************************************
 * CharacterLiteral.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CHARACTERLITERAL_H_
#define CHARACTERLITERAL_H_

#include "Expression.h"

#include "ModelBase/headers/nodes/Character.h"

namespace OOModel {

class OOMODEL_API CharacterLiteral: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(CharacterLiteral)

	ATTRIBUTE_VALUE(::Model::Character, value, setValue, QChar)
};


}

#endif /* CHARACTERLITERAL_H_ */
