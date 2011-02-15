/***********************************************************************************************************************
 * TypedListInstantiations.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "ModelBase/headers/nodes/TypedListDefinition.h"

#include "allOOModelNodes.h"

template class Model::TypedList<OOModel::Class>;
template class Model::TypedList<OOModel::Project>;
template class Model::TypedList<OOModel::Library>;
template class Model::TypedList<OOModel::Module>;
template class Model::TypedList<OOModel::Method>;
template class Model::TypedList<OOModel::StatementItem>;
template class Model::TypedList<OOModel::FormalArgument>;
template class Model::TypedList<OOModel::FormalResult>;
template class Model::TypedList<OOModel::Field>;

template class Model::TypedList<OOModel::Statement>;
template class Model::TypedList<OOModel::Block>;
template class Model::TypedList<OOModel::BreakStatement>;
template class Model::TypedList<OOModel::ContinueStatement>;
template class Model::TypedList<OOModel::ForEachStatement>;
template class Model::TypedList<OOModel::IfStatement>;
template class Model::TypedList<OOModel::LoopStatement>;
template class Model::TypedList<OOModel::ReturnStatement>;
template class Model::TypedList<OOModel::SwitchCase>;
template class Model::TypedList<OOModel::SwitchStatement>;
template class Model::TypedList<OOModel::VariableDeclaration>;
template class Model::TypedList<OOModel::MethodCallStatement>;
template class Model::TypedList<OOModel::AssignmentStatement>;

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
template class Model::TypedList<OOModel::MethodCallExpression>;
template class Model::TypedList<OOModel::UnaryOperation>;
template class Model::TypedList<OOModel::BinaryOperation>;
template class Model::TypedList<OOModel::CastExpression>;
template class Model::TypedList<OOModel::ReferenceExpression>;

template class Model::TypedList<OOModel::Type>;
template class Model::TypedList<OOModel::PrimitiveType>;
template class Model::TypedList<OOModel::NamedType>;
