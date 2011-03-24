/***********************************************************************************************************************
 * ReferenceExpression.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/ReferenceExpression.h"
#include "top_level/Class.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(ReferenceExpression, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(ReferenceExpression, Expression)

REGISTER_ATTRIBUTE(ReferenceExpression, prefix, Expression, false, true, true)
REGISTER_ATTRIBUTE(ReferenceExpression, ref, Reference, false, false, true)

Class* ReferenceExpression::classDefinition()
{
	ReferenceExpression* exp = this;
	QString path;
	while (exp)
	{
		if (!path.isEmpty()) path.prepend(',');
		path.prepend(exp->ref()->path());

		exp = dynamic_cast<ReferenceExpression*> (exp->prefix());
	}

	return dynamic_cast<Class*> (parent()->navigateTo(this, path));
}

}
