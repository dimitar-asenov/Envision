/***********************************************************************************************************************
 * ThisExpression.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/ThisExpression.h"

#include "top_level/Class.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(ThisExpression, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(ThisExpression, Expression)

Class* ThisExpression::classDefinition()
{
	//TODO: do this more efficiently, by eliminating dynamic casts. Have a method which returns your parent
	//class or something

	Model::Node* p = parent();
	while (p)
	{
		if (dynamic_cast<Class*> (p))
			return static_cast<Class*> (p);
		p = p->parent();
	}

	return NULL;
}

}
