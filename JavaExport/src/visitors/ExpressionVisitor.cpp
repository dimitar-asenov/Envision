/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "ExpressionVisitor.h"
#include "DeclarationVisitor.h"
#include "StatementVisitor.h"
#include "ElementVisitor.h"
#include "../JavaExportException.h"

#include "OOModel/src/expressions/Expression.h"
#include "OOModel/src/expressions/types/ArrayTypeExpression.h"
#include "OOModel/src/expressions/types/ReferenceTypeExpression.h"
#include "OOModel/src/expressions/types/PointerTypeExpression.h"
#include "OOModel/src/expressions/types/FunctionTypeExpression.h"
#include "OOModel/src/expressions/types/ClassTypeExpression.h"
#include "OOModel/src/expressions/types/PrimitiveTypeExpression.h"
#include "OOModel/src/expressions/types/TypeQualifierExpression.h"
#include "OOModel/src/expressions/types/AutoTypeExpression.h"
#include "OOModel/src/expressions/AssignmentExpression.h"
#include "OOModel/src/expressions/BinaryOperation.h"
#include "OOModel/src/expressions/UnaryOperation.h"
#include "OOModel/src/expressions/TypeTraitExpression.h"
#include "OOModel/src/expressions/BooleanLiteral.h"
#include "OOModel/src/expressions/IntegerLiteral.h"
#include "OOModel/src/expressions/FloatLiteral.h"
#include "OOModel/src/expressions/NullLiteral.h"
#include "OOModel/src/expressions/StringLiteral.h"
#include "OOModel/src/expressions/CharacterLiteral.h"
#include "OOModel/src/expressions/CastExpression.h"
#include "OOModel/src/expressions/InstanceOfExpression.h"
#include "OOModel/src/expressions/CommaExpression.h"
#include "OOModel/src/expressions/ConditionalExpression.h"
#include "OOModel/src/expressions/SuperExpression.h"
#include "OOModel/src/expressions/ThisExpression.h"
#include "OOModel/src/expressions/GlobalScopeExpression.h"
#include "OOModel/src/expressions/ThrowExpression.h"
#include "OOModel/src/expressions/TypeNameOperator.h"
#include "OOModel/src/expressions/DeleteExpression.h"
#include "OOModel/src/expressions/VariableDeclarationExpression.h"
#include "OOModel/src/expressions/LambdaExpression.h"
#include "OOModel/src/expressions/ArrayInitializer.h"
#include "OOModel/src/expressions/MethodCallExpression.h"
#include "OOModel/src/expressions/MetaCallExpression.h"
#include "OOModel/src/statements/Statement.h"
#include "OOModel/src/expressions/NewExpression.h"
#include "OOModel/src/types/PointerType.h"
#include "OOModel/src/types/SymbolProviderType.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "OOModel/src/expressions/ErrorExpression.h"
#include "OOModel/src/expressions/UnfinishedOperator.h"

using namespace Export;
using namespace OOModel;

namespace JavaExport {

template <typename T> Export::SourceFragment* ExpressionVisitor::optional(T* node)
{
	return node ? visit(node) : nullptr;
}

SourceFragment* ExpressionVisitor::visit(Expression* expression)
{
	auto fragment = new CompositeFragment{expression};

	// Types ============================================================================================================
	if (auto e = DCast<ArrayTypeExpression>(expression))
		*fragment << visit(e->typeExpression()) << "[" << optional(e->fixedSize()) << "]";
	else if (auto e = DCast<ReferenceTypeExpression>(expression)) notAllowed(e);
	else if (auto e = DCast<PointerTypeExpression>(expression)) notAllowed(e);
	else if (auto e = DCast<ClassTypeExpression>(expression)) *fragment << visit(e->typeExpression());
	else if (auto e = DCast<PrimitiveTypeExpression>(expression))
	{
		switch (e->typeValue())
		{
			case PrimitiveTypeExpression::PrimitiveTypes::INT: *fragment << "int"; break;
			case PrimitiveTypeExpression::PrimitiveTypes::LONG: *fragment << "long"; break;
			case PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT: notAllowed(e); break;
			case PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG: notAllowed(e); break;
			case PrimitiveTypeExpression::PrimitiveTypes::FLOAT: *fragment << "float"; break;
			case PrimitiveTypeExpression::PrimitiveTypes::DOUBLE: *fragment << "double"; break;
			case PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN: *fragment << "boolean"; break;
			case PrimitiveTypeExpression::PrimitiveTypes::CHAR: *fragment << "char"; break;
			case PrimitiveTypeExpression::PrimitiveTypes::VOID: *fragment << "void"; break;
			default: error(e, "Unkown primitive type");
		}
	}
	else if (auto e = DCast<TypeQualifierExpression>(expression))
	{
		switch (e->qualifier())
		{
			case TypeQualifierExpression::Qualifier::CONST: notAllowed(e); break;
			case TypeQualifierExpression::Qualifier::VOLATILE: *fragment << "volatile"; break;
			default: error(e, "Unkown qualifier");
		}
		*fragment << " " << visit(e->typeExpression());
	}
	else if (auto e = DCast<AutoTypeExpression>(expression)) notAllowed(e);
	else if (auto e = DCast<FunctionTypeExpression>(expression)) notAllowed(e);

	// Operators ========================================================================================================

	else if (auto e = DCast<AssignmentExpression>(expression))
	{
		*fragment << visit(e->left()) << " ";
		switch (e->op())
		{
			case AssignmentExpression::ASSIGN: *fragment << "="; break;
			case AssignmentExpression::PLUS_ASSIGN: *fragment << "+="; break;
			case AssignmentExpression::MINUS_ASSIGN: *fragment << "-="; break;
			case AssignmentExpression::TIMES_ASSIGN: *fragment << "*="; break;
			case AssignmentExpression::DIVIDE_ASSIGN: *fragment << "/="; break;
			case AssignmentExpression::BIT_AND_ASSIGN: *fragment << "&="; break;
			case AssignmentExpression::BIT_OR_ASSIGN: *fragment << "|="; break;
			case AssignmentExpression::BIT_XOR_ASSIGN: *fragment << "^="; break;
			case AssignmentExpression::REMAINDER_ASSIGN: *fragment << "%="; break;
			case AssignmentExpression::LEFT_SHIFT_ASSIGN: *fragment << "<<="; break;
			case AssignmentExpression::RIGHT_SHIFT_SIGNED_ASSIGN: *fragment << ">>="; break;
			case AssignmentExpression::RIGHT_SHIFT_UNSIGNED_ASSIGN: *fragment << ">>>="; break;
			default: error(e, "Unkown assignment type");
		}
		*fragment << " " << visit(e->right());
	}
	else if (auto e = DCast<BinaryOperation>(expression))
	{
		*fragment << visit(e->left());
		switch (e->op())
		{
			case BinaryOperation::TIMES: *fragment << "*"; break;
			case BinaryOperation::DIVIDE: *fragment << "/"; break;
			case BinaryOperation::REMAINDER: *fragment << "%"; break;
			case BinaryOperation::PLUS: *fragment << "+"; break;
			case BinaryOperation::MINUS: *fragment << "-"; break;
			case BinaryOperation::LEFT_SHIFT: *fragment << " << "; break;
			case BinaryOperation::RIGHT_SHIFT_SIGNED: *fragment << " >> "; break;
			case BinaryOperation::RIGHT_SHIFT_UNSIGNED: *fragment << " >>> "; break;
			case BinaryOperation::LESS: *fragment << " < "; break;
			case BinaryOperation::GREATER: *fragment << " > "; break;
			case BinaryOperation::LESS_EQUALS: *fragment << " <= "; break;
			case BinaryOperation::GREATER_EQUALS: *fragment << " >= "; break;
			case BinaryOperation::EQUALS: *fragment << " == "; break;
			case BinaryOperation::NOT_EQUALS: *fragment << " != "; break;
			case BinaryOperation::XOR: *fragment << " ^ "; break;
			case BinaryOperation::AND: *fragment << " & "; break;
			case BinaryOperation::OR: *fragment << " | "; break;
			case BinaryOperation::CONDITIONAL_AND: *fragment << " && "; break;
			case BinaryOperation::CONDITIONAL_OR: *fragment << " || "; break;
			case BinaryOperation::ARRAY_INDEX: *fragment << "["; break;
			default: error(e, "Unkown binary operator type");
		}
		*fragment << visit(e->right());
		if (e->op() == BinaryOperation::ARRAY_INDEX) *fragment << "]";
	}
	else if (auto e = DCast<UnaryOperation>(expression))
	{
		switch (e->op())
		{
			case UnaryOperation::PREINCREMENT: *fragment << "++" << visit(e->operand()); break;
			case UnaryOperation::PREDECREMENT: *fragment << "--" << visit(e->operand()); break;
			case UnaryOperation::POSTINCREMENT: *fragment << visit(e->operand()) << "++"; break;
			case UnaryOperation::POSTDECREMENT: *fragment << visit(e->operand()) << "--"; break;
			case UnaryOperation::PLUS: *fragment << "+" << visit(e->operand()); break;
			case UnaryOperation::MINUS: *fragment << "-" << visit(e->operand()); break;
			case UnaryOperation::NOT: *fragment << "!" << visit(e->operand()); break;
			case UnaryOperation::COMPLEMENT: *fragment << "~" << visit(e->operand()); break;
			case UnaryOperation::PARENTHESIS: *fragment << "(" << visit(e->operand()) << ")"; break;
			case UnaryOperation::DEREFERENCE: notAllowed(e); break;
			case UnaryOperation::ADDRESSOF: notAllowed(e); break;
			default: error(e, "Unkown unary operator type");
		}
	}
	else if (auto e = DCast<TypeTraitExpression>(expression)) notAllowed(e);

	// Literals =========================================================================================================

	else if (auto e = DCast<BooleanLiteral>(expression)) *fragment << (e->value() ? "true" : "false");
	else if (auto e = DCast<IntegerLiteral>(expression)) *fragment << e->value();
	else if (auto e = DCast<FloatLiteral>(expression)) *fragment << e->value();
	else if (DCast<NullLiteral>(expression)) *fragment << "null";
	else if (auto e = DCast<StringLiteral>(expression))
		*fragment << "\"" << QString{e->value()}.replace(QRegExp{"\\n"}, "\\n") << "\""; // TODO: also consider \r
	else if (auto e = DCast<CharacterLiteral>(expression)) *fragment << "'" << e->value() << "'";

	// Misc =============================================================================================================

	else if (auto e = DCast<CastExpression>(expression))
		*fragment << "(" << visit(e->castType()) << ") " << visit(e->expr());
	else if (auto e = DCast<InstanceOfExpression>(expression))
		*fragment << visit(e->expr()) << " instanceof " << visit(e->typeExpression());
	else if (auto e = DCast<CommaExpression>(expression)) *fragment << visit(e->left()) << ", " << visit(e->right());
	else if (auto e = DCast<ConditionalExpression>(expression))
		*fragment << visit(e->condition()) << " ? " << visit(e->trueExpression()) << " : " << visit(e->falseExpression());
	else if (DCast<SuperExpression>(expression)) *fragment << "super";
	else if (DCast<ThisExpression>(expression)) *fragment << "this";
	else if (auto e = DCast<GlobalScopeExpression>(expression)) notAllowed(e);
	else if (auto e = DCast<ThrowExpression>(expression)) *fragment << "throw " << visit(e->expr());
	else if (auto e = DCast<TypeNameOperator>(expression)) notAllowed(e);
	else if (auto e = DCast<DeleteExpression>(expression)) notAllowed(e);
	else if (auto e = DCast<VariableDeclarationExpression>(expression)) *fragment << declaration(e->decl());
	else if (auto e = DCast<LambdaExpression>(expression))
	{
		*fragment << list(e->arguments(), ElementVisitor{data()}, "argsList") << " -> ";
		*fragment << list(e->body(), StatementVisitor{data()}, "body");

		if (e->results()->size() > 1) error(e->results(), "Cannot have more than one return value in Java");
	}
	else if (auto e = DCast<ArrayInitializer>(expression)) *fragment << list(e->values(), this, "initializerList");
	else if (auto e = DCast<MethodCallExpression>(expression))
		*fragment << visit(e->callee()) << list(e->arguments(), this, "argsList");
	else if (auto e = DCast<MetaCallExpression>(expression)) notAllowed(e);
	else if (auto e = DCast<NewExpression>(expression))
	{
		*fragment << "new " << visit(e->newType());
		for (auto dim : *e->dimensions())
			*fragment << "[" << visit(dim) << "]";
		if (e->initializer()) *fragment << visit(e->initializer());
	}
	else if (auto e = DCast<ReferenceExpression>(expression))
	{
		if (e->prefix()) *fragment << visit(e->prefix()) << ".";
		*fragment << e->name();
		if (!e->typeArguments()->isEmpty()) *fragment << list(e->typeArguments(), this, "typeArgsList");
	}

	// Flexible input support ===========================================================================================

	else if (DCast<EmptyExpression>(expression)); // do nothing
	else if (auto e = DCast<ErrorExpression>(expression))
	{
		if (!e->prefix().isEmpty()) *fragment << e->prefix();
		*fragment << visit(e->arg());
		if (!e->postfix().isEmpty()) *fragment << e->postfix();
	}
	else if (auto e = DCast<UnfinishedOperator>(expression))
	{
		QStringList result;

		for (int i=0; i< e->operands()->size(); ++i)
			*fragment << e->delimiters()->at(i) << visit(e->operands()->at(i));

		if (e->delimiters()->size() > e->operands()->size())
			*fragment << e->delimiters()->last();
	}
	else
	{
		throw JavaExportException{"Unhandled expression of type " + expression->typeName()};
	}

	return fragment;
}

}
