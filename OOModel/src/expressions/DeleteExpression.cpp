#include "DeleteExpression.h"


#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(OOModel::DeleteExpression)

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(DeleteExpression, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(DeleteExpression, Expression)

REGISTER_ATTRIBUTE(DeleteExpression, deleteType, Expression, false, false, true)

Type* DeleteExpression::type()
{
	// TODO
	return deleteType()->type();
}

}
