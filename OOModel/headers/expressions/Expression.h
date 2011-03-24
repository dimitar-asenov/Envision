/***********************************************************************************************************************
 * Expression.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include "../oomodel_api.h"

#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"
#include "ModelBase/headers/nodes/nodeMacros.h"

namespace OOModel {

class Class;

class OOMODEL_API Expression : public Model::ExtendableNode
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(Expression)

	public:

		/**
		 * Returns the class definition node that represents the type of this expression.
		 *
		 * In case the type of this expression is a user defined class, this method will return the node that defines
		 * the class. Otherwise returns NULL.
		 */
		virtual Class* classDefinition();
};

}

#endif /* EXPRESSION_H_ */
