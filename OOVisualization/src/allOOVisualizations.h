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

#include "top_level/VProject.h"
#include "top_level/VModule.h"
#include "top_level/VClass.h"
#include "top_level/VMethod.h"

#include "elements/VField.h"
#include "elements/VEnumerator.h"
#include "elements/VFormalArgument.h"
#include "elements/VFormalResult.h"
#include "elements/VFormalTypeArgument.h"
#include "elements/VStatementItemList.h"
#include "elements/VCatchClause.h"

#include "expressions/VExpression.h"
#include "expressions/VAssignmentExpression.h"
#include "expressions/VMethodCallExpression.h"
#include "expressions/VReferenceExpression.h"
#include "expressions/VThisExpression.h"
#include "expressions/VCastExpression.h"
#include "expressions/VNewExpression.h"
#include "expressions/VUnaryOperation.h"
#include "expressions/VBinaryOperation.h"
#include "expressions/VArrayInitializer.h"
#include "expressions/VCommaExpression.h"
#include "expressions/VVariableDeclaration.h"
#include "expressions/VConditionalExpression.h"
#include "expressions/VThrowExpression.h"
#include "expressions/VLambdaExpression.h"

#include "expressions/VEmptyExpression.h"
#include "expressions/VErrorExpression.h"
#include "expressions/VUnfinishedOperator.h"

#include "literals/VStringLiteral.h"
#include "literals/VIntegerLiteral.h"
#include "literals/VFloatLiteral.h"
#include "literals/VCharacterLiteral.h"
#include "literals/VBooleanLiteral.h"
#include "literals/VNullLiteral.h"

#include "statements/VStatementItem.h"
#include "statements/VBlock.h"
#include "statements/VReturnStatement.h"
#include "statements/VIfStatement.h"
#include "statements/VLoopStatement.h"
#include "statements/VForEachStatement.h"
#include "statements/VBreakStatement.h"
#include "statements/VContinueStatement.h"
#include "statements/VExpressionStatement.h"
#include "statements/VTryCatchFinally.h"

#include "types/VClassType.h"
#include "types/VPrimitiveType.h"
#include "types/VArrayType.h"
#include "types/VPointerType.h"
#include "types/VReferenceType.h"

#include "alternative/VKeywordMethodCall.h"
