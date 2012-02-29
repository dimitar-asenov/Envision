/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

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
class AssignmentStatement;
class ExpressionStatement;

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
class ArrayInitializer;
class EmptyExpression;
class ErrorExpression;
class UnfinishedOperator;
class CommaExpression;

class Type;
class PrimitiveType;
class NamedType;
class ArrayType;

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
template class Q_DECL_IMPORT Model::TypedList<OOModel::AssignmentStatement>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::ExpressionStatement>;

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
template class Q_DECL_IMPORT Model::TypedList<OOModel::ArrayInitializer>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::EmptyExpression>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::ErrorExpression>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::UnfinishedOperator>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::CommaExpression>;

template class Q_DECL_IMPORT Model::TypedList<OOModel::Type>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::PrimitiveType>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::NamedType>;
template class Q_DECL_IMPORT Model::TypedList<OOModel::ArrayType>;


#endif /* TYPEDLISTINSTANTIATIONS_H_ */
