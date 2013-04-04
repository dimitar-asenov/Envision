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

#include "ModelBase/src/nodes/TypedListDefinition.h"
#include "ModelBase/src/TypedListInstantiations.h"

#include "allOOModelNodes.h"

template class Model::TypedList<OOModel::Class>;
template class Model::TypedList<OOModel::Project>;
template class Model::TypedList<OOModel::Library>;
template class Model::TypedList<OOModel::Module>;
template class Model::TypedList<OOModel::Method>;
template class Model::TypedList<OOModel::StatementItem>;
template class Model::TypedList<OOModel::FormalArgument>;
template class Model::TypedList<OOModel::FormalResult>;
template class Model::TypedList<OOModel::FormalTypeArgument>;
template class Model::TypedList<OOModel::Field>;
template class Model::TypedList<OOModel::OOReference>;
template class Model::TypedList<OOModel::CatchClause>;

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
template class Model::TypedList<OOModel::ExpressionStatement>;
template class Model::TypedList<OOModel::TryCatchFinallyStatement>;

template class Model::TypedList<OOModel::Expression>;
template class Model::TypedList<OOModel::IntegerLiteral>;
template class Model::TypedList<OOModel::FloatLiteral>;
template class Model::TypedList<OOModel::StringLiteral>;
template class Model::TypedList<OOModel::BooleanLiteral>;
template class Model::TypedList<OOModel::CharacterLiteral>;
template class Model::TypedList<OOModel::NullLiteral>;
template class Model::TypedList<OOModel::ThisExpression>;
template class Model::TypedList<OOModel::NewExpression>;
template class Model::TypedList<OOModel::MethodCallExpression>;
template class Model::TypedList<OOModel::UnaryOperation>;
template class Model::TypedList<OOModel::BinaryOperation>;
template class Model::TypedList<OOModel::CastExpression>;
template class Model::TypedList<OOModel::ReferenceExpression>;
template class Model::TypedList<OOModel::ArrayInitializer>;
template class Model::TypedList<OOModel::EmptyExpression>;
template class Model::TypedList<OOModel::ErrorExpression>;
template class Model::TypedList<OOModel::UnfinishedOperator>;
template class Model::TypedList<OOModel::CommaExpression>;
template class Model::TypedList<OOModel::AssignmentExpression>;
template class Model::TypedList<OOModel::ConditionalExpression>;
template class Model::TypedList<OOModel::ThrowExpression>;
template class Model::TypedList<OOModel::LambdaExpression>;

template class Model::TypedList<OOModel::TypeExpression>;
template class Model::TypedList<OOModel::PrimitiveTypeExpression>;
template class Model::TypedList<OOModel::ClassTypeExpression>;
template class Model::TypedList<OOModel::ArrayTypeExpression>;
