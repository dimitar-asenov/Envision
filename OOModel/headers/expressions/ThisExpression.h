/***********************************************************************************************************************
 * ThisExpression.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef THISEXPRESSION_H_
#define THISEXPRESSION_H_

#include "Expression.h"

namespace OOModel {

class OOMODEL_API ThisExpression: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(ThisExpression)

	public:
		virtual Class* classDefinition();
};

}

#endif /* THISEXPRESSION_H_ */
