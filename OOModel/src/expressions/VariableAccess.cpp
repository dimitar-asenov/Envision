/***********************************************************************************************************************
 * VariableAccess.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VariableAccess.h"
#include "expressions/ReferenceExpression.h"
#include "top_level/Class.h"
#include "top_level/Field.h"

#include "statements/VariableDeclaration.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(VariableAccess, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(VariableAccess, Expression)

REGISTER_ATTRIBUTE(VariableAccess, prefix, Expression, false, true, true)
REGISTER_ATTRIBUTE(VariableAccess, ref, Reference, false, false, true)

Class* VariableAccess::classDefinition()
{
	QString path = ref()->path();

	ReferenceExpression* exp = dynamic_cast<ReferenceExpression*> (prefix());
	while (exp)
	{
		if (!path.isEmpty()) path.prepend(',');
		path.prepend(exp->ref()->path());

		exp = dynamic_cast<ReferenceExpression*> (exp->prefix());
	}

	Model::Node* var = navigateTo(this, path);

	Field* f = dynamic_cast<Field*> (var);
	if (f) return f->type()->classDefinition();

	VariableDeclaration* vd = dynamic_cast<VariableDeclaration*> (var);
	if (vd) return vd->type()->classDefinition();

	return NULL;
}

}
