/***********************************************************************************************************************
 * NamedType.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef NAMEDTYPE_H_
#define NAMEDTYPE_H_

#include "Type.h"

#include "../expressions/ReferenceExpression.h"

namespace OOModel {

class OOMODEL_API NamedType : public Type
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(NamedType)

	ATTRIBUTE(ReferenceExpression, type, setType);
};

}

#endif /* NAMEDTYPE_H_ */
