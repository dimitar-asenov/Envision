/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

// NO header guard or '#pragma once' since this file is included multiple times with different definitions.
// This is a variation of the 'X-macros' technique.


BEGIN_STANDARD_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VNewExpression, OOModel::NewExpression)
	PREFIX(true)
	OPERAND(newType)
	INFIX(node()->amount())
	OPERAND(amount)
	POSTFIX(node()->amount())
END_STANDARD_EXPRESSION_VISUALIZATION

BEGIN_STANDARD_ENUMERATION_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VDeleteExpression,
		OOModel::DeleteExpression, isArray)
	PREFIX(true)
	OPERAND(expr)
END_STANDARD_EXPRESSION_VISUALIZATION

BEGIN_STANDARD_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VArrayType, OOModel::ArrayTypeExpression)
	OPERAND(typeExpression)
	POSTFIX(true)
END_STANDARD_EXPRESSION_VISUALIZATION

BEGIN_STANDARD_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VPointerType, OOModel::PointerTypeExpression)
	OPERAND(typeExpression)
	POSTFIX(true)
END_STANDARD_EXPRESSION_VISUALIZATION

BEGIN_STANDARD_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VReferenceType, OOModel::ReferenceTypeExpression)
	OPERAND(typeExpression)
	POSTFIX(true)
END_STANDARD_EXPRESSION_VISUALIZATION

BEGIN_STANDARD_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VCommaExpression, OOModel::CommaExpression)
	OPERAND(left)
	INFIX(true)
	OPERAND(right)
END_STANDARD_EXPRESSION_VISUALIZATION

BEGIN_STANDARD_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VCastExpression, OOModel::CastExpression)
	OPERAND(expr)
	OPERAND(castType)
END_STANDARD_EXPRESSION_VISUALIZATION

BEGIN_STANDARD_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VThrowExpression, OOModel::ThrowExpression)
	PREFIX(true)
	OPERAND(expr)
END_STANDARD_EXPRESSION_VISUALIZATION

BEGIN_STANDARD_ENUMERATION_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VAssignmentExpression,
		OOModel::AssignmentExpression, op)
	OPERAND(left)
	INFIX(true)
	OPERAND(right)
END_STANDARD_EXPRESSION_VISUALIZATION

BEGIN_STANDARD_ENUMERATION_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VUnaryOperation,
		OOModel::UnaryOperation, op)
	PREFIX(!opStyle->preSymbol().isEmpty())
	OPERAND(operand)
	POSTFIX(!opStyle->postSymbol().isEmpty())
END_STANDARD_EXPRESSION_VISUALIZATION

BEGIN_STANDARD_FLAG_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VTypeQualifierExpression,
		OOModel::TypeQualifierExpression, qualifier)
	PREFIX(true)
	OPERAND(typeExpression)
END_STANDARD_EXPRESSION_VISUALIZATION

//BEGIN_STANDARD_ENUMERATION_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VBinaryOperation,
//		OOModel::BinaryOperation, op)
//	PREFIX(!opStyle->preSymbol().isEmpty())
//	OPERAND(left)
//	INFIX(!opStyle->inSymbol().isEmpty())
//	OPERAND(right)
//	POSTFIX(!opStyle->postSymbol().isEmpty())
//END_STANDARD_EXPRESSION_VISUALIZATION

BEGIN_STANDARD_EXPRESSION_VISUALIZATION(OOVISUALIZATION_API, VConditionalExpression, OOModel::ConditionalExpression)
	PREFIX(!opStyle->preSymbol().isEmpty())
	OPERAND(condition)
	INFIX(!opStyle->inSymbol().isEmpty())
	OPERAND(trueExpression)
	INFIX2(!opStyle->in2Symbol().isEmpty())
	OPERAND(falseExpression)
	POSTFIX(!opStyle->postSymbol().isEmpty())
END_STANDARD_EXPRESSION_VISUALIZATION
