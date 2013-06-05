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

#include "declarations/Project.h"
#include "declarations/Module.h"
#include "declarations/Class.h"
#include "declarations/Method.h"
#include "declarations/Field.h"
#include "declarations/Declaration.h"
#include "declarations/VariableDeclaration.h"
#include "declarations/TypeAlias.h"

#include "statements/Statement.h"
#include "statements/Block.h"
#include "statements/BreakStatement.h"
#include "statements/ContinueStatement.h"
#include "statements/ForEachStatement.h"
#include "statements/IfStatement.h"
#include "statements/LoopStatement.h"
#include "statements/ReturnStatement.h"
#include "statements/SwitchCase.h"
#include "statements/SwitchStatement.h"
#include "statements/ExpressionStatement.h"
#include "statements/DeclarationStatement.h"
#include "statements/TryCatchFinallyStatement.h"

#include "expressions/Expression.h"
#include "expressions/IntegerLiteral.h"
#include "expressions/FloatLiteral.h"
#include "expressions/StringLiteral.h"
#include "expressions/BooleanLiteral.h"
#include "expressions/CharacterLiteral.h"
#include "expressions/NullLiteral.h"
#include "expressions/ThisExpression.h"
#include "expressions/NewExpression.h"
#include "expressions/DeleteExpression.h"
#include "expressions/MethodCallExpression.h"
#include "expressions/UnaryOperation.h"
#include "expressions/BinaryOperation.h"
#include "expressions/CastExpression.h"
#include "expressions/ReferenceExpression.h"
#include "expressions/ArrayInitializer.h"
#include "expressions/EmptyExpression.h"
#include "expressions/ErrorExpression.h"
#include "expressions/UnfinishedOperator.h"
#include "expressions/CommaExpression.h"
#include "expressions/AssignmentExpression.h"
#include "expressions/VariableDeclarationExpression.h"
#include "expressions/ConditionalExpression.h"
#include "expressions/ThrowExpression.h"
#include "expressions/LambdaExpression.h"
#include "expressions/TypeTraitExpression.h"

#include "expressions/types/TypeExpression.h"
#include "expressions/types/PrimitiveTypeExpression.h"
#include "expressions/types/ClassTypeExpression.h"
#include "expressions/types/ArrayTypeExpression.h"
#include "expressions/types/PointerTypeExpression.h"
#include "expressions/types/ReferenceTypeExpression.h"
#include "expressions/types/AutoTypeExpression.h"
#include "expressions/types/TypeQualifierExpression.h"
#include "expressions/types/FunctionTypeExpression.h"

#include "elements/Visibility.h"
#include "elements/Enumerator.h"
#include "elements/StorageSpecifier.h"
#include "elements/StatementItem.h"
#include "elements/StatementItemList.h"
#include "elements/FormalArgument.h"
#include "elements/FormalResult.h"
#include "elements/FormalTypeArgument.h"
#include "elements/OOReference.h"
#include "elements/CatchClause.h"
#include "elements/MemberInitializer.h"
