/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#pragma once

#include "ModelBase/src/nodes/TypedList.h"

namespace OOModel {

class Class;
class Project;
class Library;
class Module;
class Method;
class StatementItem;
class FormalArgument;
class FormalResult;
class FormalTypeArgument;
class Field;
class OOReference;
class CatchClause;

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
class ExpressionStatement;
class TryCatchFinallyStatement;

class Expression;
class IntegerLiteral;
class FloatLiteral;
class StringLiteral;
class BooleanLiteral;
class CharacterLiteral;
class NullLiteral;
class ThisExpression;
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
class AssignmentExpression;
class ConditionalExpression;
class ThrowExpression;
class LambdaExpression;

class TypeExpression;
class PrimitiveTypeExpression;
class ClassTypeExpression;
class ArrayTypeExpression;

}

extern template class Q_DECL_IMPORT Model::TypedList<OOModel::Class>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::Project>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::Library>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::Module>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::Method>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::StatementItem>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::FormalArgument>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::FormalResult>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::FormalTypeArgument>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::Field>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::OOReference>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::CatchClause>;

extern template class Q_DECL_IMPORT Model::TypedList<OOModel::Statement>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::Block>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::BreakStatement>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::ContinueStatement>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::ForEachStatement>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::IfStatement>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::LoopStatement>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::ReturnStatement>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::SwitchCase>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::SwitchStatement>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::VariableDeclaration>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::ExpressionStatement>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::TryCatchFinallyStatement>;

extern template class Q_DECL_IMPORT Model::TypedList<OOModel::Expression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::IntegerLiteral>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::FloatLiteral>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::StringLiteral>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::BooleanLiteral>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::CharacterLiteral>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::NullLiteral>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::ThisExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::NewExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::MethodCallExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::UnaryOperation>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::BinaryOperation>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::CastExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::ReferenceExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::ArrayInitializer>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::EmptyExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::ErrorExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::UnfinishedOperator>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::CommaExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::AssignmentExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::ConditionalExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::ThrowExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::LambdaExpression>;

extern template class Q_DECL_IMPORT Model::TypedList<OOModel::TypeExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::PrimitiveTypeExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::ClassTypeExpression>;
extern template class Q_DECL_IMPORT Model::TypedList<OOModel::ArrayTypeExpression>;
