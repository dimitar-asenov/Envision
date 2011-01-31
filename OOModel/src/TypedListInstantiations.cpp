/***********************************************************************************************************************
 * TypedListInstantiations.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "ModelBase/headers/nodes/TypedList.h"
#include "ModelBase/headers/nodes/TypedListDefinition.h"

#include "Project.h"
#include "Module.h"
#include "Class.h"
#include "Method.h"
#include "MethodItem.h"
#include "MethodArgument.h"

#include "statements/Statement.h"

#include "expressions/Expression.h"
#include "expressions/IntegerLiteral.h"
#include "expressions/FloatLiteral.h"
#include "expressions/StringLiteral.h"
#include "expressions/BooleanLiteral.h"
#include "expressions/CharacterLiteral.h"
#include "expressions/NullLiteral.h"
#include "expressions/ThisExpression.h"
#include "expressions/VariableAccess.h"
#include "expressions/NewExpression.h"
#include "expressions/MethodCall.h"
#include "expressions/UnaryOperation.h"
#include "expressions/BinaryOperation.h"

#include "types/Type.h"
#include "types/PrimitiveType.h"
#include "types/NamedType.h"

template class Model::TypedList<OOModel::Class>;
template class Model::TypedList<OOModel::Project>;
template class Model::TypedList<OOModel::Module>;
template class Model::TypedList<OOModel::Method>;
template class Model::TypedList<OOModel::MethodItem>;
template class Model::TypedList<OOModel::MethodArgument>;

template class Model::TypedList<OOModel::Statement>;

template class Model::TypedList<OOModel::Expression>;
template class Model::TypedList<OOModel::IntegerLiteral>;
template class Model::TypedList<OOModel::FloatLiteral>;
template class Model::TypedList<OOModel::StringLiteral>;
template class Model::TypedList<OOModel::BooleanLiteral>;
template class Model::TypedList<OOModel::CharacterLiteral>;
template class Model::TypedList<OOModel::NullLiteral>;
template class Model::TypedList<OOModel::ThisExpression>;
template class Model::TypedList<OOModel::VariableAccess>;
template class Model::TypedList<OOModel::NewExpression>;
template class Model::TypedList<OOModel::MethodCall>;
template class Model::TypedList<OOModel::UnaryOperation>;
template class Model::TypedList<OOModel::BinaryOperation>;

template class Model::TypedList<OOModel::Type>;
template class Model::TypedList<OOModel::PrimitiveType>;
template class Model::TypedList<OOModel::NamedType>;
