/***********************************************************************************************************************
 * TypedListInstantiations.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TYPEDLISTINSTANTIATIONS_H_
#define TYPEDLISTINSTANTIATIONS_H_

#include "ModelBase/headers/nodes/TypedList.h"

namespace OOModel {

class Class;
class Project;
class Library;
class Module;
class Method;
class StatementItem;
class FormalArgument;
class FormalResult;
class Field;

class Statement;
class Block;
class BreakStatement;
class ContinueStatement;
class ForEachStatement;
class IfStatement;
class LoopStatement;
class ReturnStatement;
class SwitchCase;
class SwitchStatement;
class VariableDeclaration;
class MethodCallStatement;
class AssignmentStatement;

class Expression;
class IntegerLiteral;
class FloatLiteral;
class StringLiteral;
class BooleanLiteral;
class CharacterLiteral;
class NullLiteral;
class ThisExpression;
class VariableAccess;
class NewExpression;
class MethodCallExpression;
class UnaryOperation;
class BinaryOperation;
class CastExpression;
class ReferenceExpression;

class Type;
class PrimitiveType;
class NamedType;

}

template class Q_DECL_IMPORT Model::TypedList<OOModel::Class>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::Project>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::Library>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::Module>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::Method>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::StatementItem>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::FormalArgument>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::FormalResult>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::Field>;

template class Q_DECL_IMPORT Model::TypedList<OOModel::Statement>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::Block>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::BreakStatement>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::ContinueStatement>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::ForEachStatement>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::IfStatement>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::LoopStatement>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::ReturnStatement>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::SwitchCase>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::SwitchStatement>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::VariableDeclaration>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::MethodCallStatement>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::AssignmentStatement>;

template class Q_DECL_IMPORT Model::TypedList<OOModel::Expression>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::IntegerLiteral>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::FloatLiteral>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::StringLiteral>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::BooleanLiteral>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::CharacterLiteral>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::NullLiteral>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::ThisExpression>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::VariableAccess>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::NewExpression>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::MethodCallExpression>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::UnaryOperation>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::BinaryOperation>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::CastExpression>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::ReferenceExpression>;

template class Q_DECL_IMPORT Model::TypedList<OOModel::Type>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::PrimitiveType>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::NamedType>;


#endif /* TYPEDLISTINSTANTIATIONS_H_ */
