/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "OOOperatorDescriptorList.h"

#include "CommandDescriptor.h"
#include "CompoundObjectDescriptor.h"
#include "commands/CreateLambda.h"

#include "OOModel/src/allOOModelNodes.h"

using namespace OOModel;
using namespace Interaction;

namespace OOInteraction {

OOOperatorDescriptorList* OOOperatorDescriptorList::instance()
{
	static OOOperatorDescriptorList theInstance;
	return &theInstance;
}

template <typename T>
void OOOperatorDescriptorList::extractCommaInto (Expression* expression, T* destination, bool ignoreEmpty,
																 bool convertTrailingCommaErrorsIntoEmptyExpressions)
{
	auto expressionToExpand = expression;
	auto expressionToCheckForError = expression;

	// If this is an unfinished operator, still try to process the commas
	if (convertTrailingCommaErrorsIntoEmptyExpressions)
		if (auto unf = DCast<UnfinishedOperator>(expression))
		{
			if (unf->delimiters()->size() >= 2 && unf->delimiters()->at(1)->get() == "," && unf->operands()->size() > 0 )
			{
				expressionToExpand = unf->operands()->first();
				unf->operands()->replaceChild(expressionToExpand, new EmptyExpression{});

				if (unf->operands()->size() > 1)
				{
					Q_ASSERT(unf->operands()->size() == 2);
					expressionToCheckForError = unf->operands()->at(1);
					unf->operands()->replaceChild(expressionToCheckForError, new EmptyExpression{});
				}
				else expressionToCheckForError = nullptr;

				SAFE_DELETE(unf);
			}
		}

	// Collect some infor about the error
	auto errorExpression = DCast<ErrorExpression>(expressionToCheckForError);
	bool errorIsTrailingCommas = errorExpression;
	int errorCommaCount = 0;
	if (errorExpression)
	{
		errorIsTrailingCommas = errorIsTrailingCommas && DCast<EmptyExpression>(errorExpression->arg());
		errorIsTrailingCommas = errorIsTrailingCommas
				&& (errorExpression->prefix().count(',') == errorExpression->prefix().size());
		errorIsTrailingCommas = errorIsTrailingCommas
				&& (errorExpression->postfix().count(',') == errorExpression->postfix().size());

		errorCommaCount = errorExpression->prefix().size() + errorExpression->postfix().size();
	}

	// Expand all the commas
	if (auto comma = DCast<CommaExpression>(expressionToExpand))
	{
		for (auto arg : comma->allSubOperands(true)) destination->append(arg);
		SAFE_DELETE(comma);
	}
	else
	{
		if (!ignoreEmpty || !DCast<EmptyExpression>(expressionToExpand) )
			destination->append(expressionToExpand);
	}

	// Add any trailing commas as empty expressions
	if (convertTrailingCommaErrorsIntoEmptyExpressions)
	{
		auto emptyToAdd = 0;
		if (expressionToCheckForError == nullptr) emptyToAdd = 1;
		else
		{
			if (expressionToCheckForError == expression)
			{
				if (errorIsTrailingCommas)
				{
					Q_ASSERT(destination->size() == 1);
					destination->remove(errorExpression);
					emptyToAdd += 1 + errorCommaCount;
				}
			}
			else
			{
				Q_ASSERT(errorExpression);
				if (errorIsTrailingCommas) emptyToAdd += 1 + errorCommaCount;
				else destination->append(errorExpression);
			}
		}

		for (int i = 0; i < emptyToAdd; ++i) destination->append( new EmptyExpression{});
	}
}

void OOOperatorDescriptorList::initializeWithDefaultOperators()
{
	using OD = OOOperatorDescriptor;
	// Unary operators
	add(new OD{"preincrement", "++ expr", 2, OD::RightAssociative,	OD::unary<UnaryOperation::PREINCREMENT>});
	add(new OD{"predecrement", "-- expr", 2, OD::RightAssociative,	OD::unary<UnaryOperation::PREDECREMENT>});
	add(new OD{"postincrement", "expr ++", 2, OD::LeftAssociative, OD::unary<UnaryOperation::POSTINCREMENT>});
	add(new OD{"postdecrement", "expr --", 2, OD::LeftAssociative, OD::unary<UnaryOperation::POSTDECREMENT>});
	add(new OD{"unary plus", "+ expr", 2, OD::RightAssociative,	OD::unary<UnaryOperation::PLUS>});
	add(new OD{"unary minus", "- expr", 2, OD::RightAssociative, OD::unary<UnaryOperation::MINUS>});
	add(new OD{"not", "! expr", 2, OD::RightAssociative, OD::unary<UnaryOperation::NOT>});
	add(new OD{"complement", "~ expr", 2, OD::RightAssociative, OD::unary<UnaryOperation::COMPLEMENT>});
	add(new OD{"parenthesis", "( typeOrExpr )", 1, OD::NotAssociative, OD::unary<UnaryOperation::PARENTHESIS>});
	add(new OD{"dereference", "* expr", 2, OD::RightAssociative, OD::unary<UnaryOperation::DEREFERENCE>});
	add(new OD{"addressof", "& expr", 2, OD::RightAssociative, OD::unary<UnaryOperation::ADDRESSOF>});

	// Binary operators
	add(new OD{"times", "expr * expr", 3, OD::LeftAssociative, OD::binary<BinaryOperation::TIMES>});
	add(new OD{"divide", "expr / expr", 3, OD::LeftAssociative, OD::binary<BinaryOperation::DIVIDE>});
	add(new OD{"remainder", "expr % expr", 3, OD::LeftAssociative, OD::binary<BinaryOperation::REMAINDER>});
	add(new OD{"plus", "expr + expr", 4, OD::LeftAssociative, OD::binary<BinaryOperation::PLUS>});
	add(new OD{"minus", "expr - expr", 4, OD::LeftAssociative, OD::binary<BinaryOperation::MINUS>});
	add(new OD{"left shift", "expr << expr", 5, OD::LeftAssociative, OD::binary<BinaryOperation::LEFT_SHIFT>});
	add(new OD{"right shift signed", "expr >> expr", 5, OD::LeftAssociative,
			OD::binary<BinaryOperation::RIGHT_SHIFT_SIGNED>});
	add(new OD{"righ shift unsigned", "expr >>> expr", 5, OD::LeftAssociative,
			OD::binary<BinaryOperation::RIGHT_SHIFT_UNSIGNED>});
	add(new OD{"less", "expr < expr", 6, OD::LeftAssociative, OD::binary<BinaryOperation::LESS>});
	add(new OD{"greater", "expr > expr", 6, OD::LeftAssociative, OD::binary<BinaryOperation::GREATER>});
	add(new OD{"less equals", "expr <= expr", 6, OD::LeftAssociative, OD::binary<BinaryOperation::LESS_EQUALS>});
	add(new OD{"greater equals", "expr >= expr", 6, OD::LeftAssociative,
			OD::binary<BinaryOperation::GREATER_EQUALS>});
	add(new OD{"equals", "expr == expr", 7, OD::LeftAssociative, OD::binary<BinaryOperation::EQUALS>});
	add(new OD{"not equals", "expr != expr", 7, OD::LeftAssociative, OD::binary<BinaryOperation::NOT_EQUALS>});
	add(new OD{"bitwise xor", "expr ^ expr", 9, OD::LeftAssociative, OD::binary<BinaryOperation::XOR>});
	add(new OD{"bitwise and", "expr & expr", 8, OD::LeftAssociative, OD::binary<BinaryOperation::AND>});
	add(new OD{"bitwise or", "expr | expr", 10, OD::LeftAssociative, OD::binary<BinaryOperation::OR>});
	add(new OD{"conditional and", "expr && expr", 11, OD::LeftAssociative,
			OD::binary<BinaryOperation::CONDITIONAL_AND>});
	add(new OD{"conditional or", "expr || expr", 12, OD::LeftAssociative,
			OD::binary<BinaryOperation::CONDITIONAL_OR>});
	add(new OD{"array index", "expr [ expr ]", 1, OD::LeftAssociative, OD::binary<BinaryOperation::ARRAY_INDEX>});

	// Ternary operator
	add(new OD{"conditional expression", "expr ? expr : expr", 13, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new ConditionalExpression{};
		opr->setCondition(operands.first());
		opr->setTrueExpression(operands.at(1));
		opr->setFalseExpression(operands.last());
		return opr;
	}});

	// Assignment
	add(new OD{"assign", "expr = expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::ASSIGN>});
	add(new OD{"assign", "expr += expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::PLUS_ASSIGN>});
	add(new OD{"assign", "expr -= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::MINUS_ASSIGN>});
	add(new OD{"assign", "expr *= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::TIMES_ASSIGN>});
	add(new OD{"assign", "expr /= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::DIVIDE_ASSIGN>});
	add(new OD{"assign", "expr &= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::BIT_AND_ASSIGN>});
	add(new OD{"assign", "expr |= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::BIT_OR_ASSIGN>});
	add(new OD{"assign", "expr ^= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::BIT_XOR_ASSIGN>});
	add(new OD{"assign", "expr %= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::REMAINDER_ASSIGN>});
	add(new OD{"assign", "expr <<= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::LEFT_SHIFT_ASSIGN>});
	add(new OD{"assign", "expr >>= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::RIGHT_SHIFT_SIGNED_ASSIGN>});
	add(new OD{"assign", "expr >>>= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::RIGHT_SHIFT_UNSIGNED_ASSIGN>});

	// Primitive Types
	add(new OD{ "int", "int", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT}; }});
	add(new OD{ "long", "long", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::LONG}; }});
	add(new OD{ "uint", "uint", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT}; }});
	add(new OD{ "ulong", "ulong", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG}; }});
	add(new OD{ "float", "float", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::FLOAT}; }});
	add(new OD{ "double", "double", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::DOUBLE}; }});
	add(new OD{ "bool", "bool", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN}; }});
	add(new OD{ "char", "char", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::CHAR}; }});
	add(new OD{ "void", "void", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID}; }});

	// Function Type
	add(new OD{ "void function type", "[] ( expr )", 1, OD::NotAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 1);
		auto fte = new FunctionTypeExpression{};

		extractCommaInto(operands.first(), fte->arguments(), false, false);

		return fte;
	}});
	add(new OD{ "non-void function type", "[] ( expr ) -> ( type )", 1, OD::NotAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 2);
		auto fte = new FunctionTypeExpression{};

		extractCommaInto(operands.first(), fte->arguments(), false, false);
		extractCommaInto(operands.last(), fte->results(), false, false);

		return fte;
	}});


	// Keywords
	add(new OD{ "super", "super", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new SuperExpression{}; }});
	add(new OD{ "this", "this", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new ThisExpression{}; }});
	add(new OD{ "::", "::", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
			return new GlobalScopeExpression{}; }});
	add(new OD{ "auto", "auto", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new AutoTypeExpression{}; }});
	add(new OD{ "null", "null", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new NullLiteral{}; }});
	add(new OD{ "true", "true", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new BooleanLiteral{true}; }});
	add(new OD{ "false", "false", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new BooleanLiteral{false}; }});

	// Others
	add(new OD{ "cast", "( type ) expr", 2, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new CastExpression{};
		opr->setType(operands.first());
		opr->setExpr(operands.last());
		return opr;
	}});

	add(new OD{ "instanceof", "expr instanceof type", 6, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new InstanceOfExpression{};
		opr->setExpr(operands.first());
		opr->setTypeExpression(operands.last());
		return opr;
	}});

	add(new OD{ "comma", "typeOrExpr , typeOrExpr", 50, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new CommaExpression{};
		opr->setLeft(operands.first());
		opr->setRight(operands.last());
		return opr;
	}});

	add(new OD{ "initializer", "{ expr }", 0, OD::NotAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new ArrayInitializer{};

		for (auto e: operands) extractCommaInto(e, opr->values(), false, false);

		return opr;
	}});

	add(new OD{ "new object", "new expr", 2, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new NewExpression{};
		//TODO: not quite right, we need to set the operand as the initializer expression and the new type as the
		// first part of the expression. For primitive types, the current implementation is fine.
		opr->setNewType( operands.first());
		return opr;
	}});

	add(new OD{ "new array", "new expr [ expr ]", 2, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new NewExpression{};
		opr->setNewType( operands.first());
		extractCommaInto( operands.last(), opr->dimensions(), false, true);
		return opr;
	}});

	add(new OD{ "new array with init", "new expr [ expr ] expr", 2, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new NewExpression{};
		opr->setNewType( operands.first());
		extractCommaInto( operands.at(1), opr->dimensions(), false, true);
		opr->setInitializer(operands.last());
		return opr;
	}});

	add(new OD{ "delete object", "delete expr", 2, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto expr = new DeleteExpression{};
		expr->setExpr( operands.first());
		return expr;
	}});

	add(new OD{ "delete array", "delete [] expr", 2, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto expr = new DeleteExpression{};
		expr->setIsArray(true);
		expr->setExpr( operands.first());
		return expr;
	}});

	add(new OD{ "member dot", "expr . id", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 2);
		auto ref = DCast<ReferenceExpression>( operands[1]);
		Q_ASSERT(ref);

		auto r = new ReferenceExpression{ ref->name(), operands.first() };
		SAFE_DELETE(ref);
		return r;
	}});
	add(new OD{ "member pointer", "expr -> id", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 2);
		auto ref = DCast<ReferenceExpression>( operands[1]);
		Q_ASSERT(ref);

		auto r = new ReferenceExpression{ ref->name(), operands.first(), ReferenceExpression::MemberKind::Pointer };
		SAFE_DELETE(ref);
		return r;
	}});
	add(new OD{ "member static", "expr :: id", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 2);
		auto ref = DCast<ReferenceExpression>( operands[1]);
		Q_ASSERT(ref);

		auto r = new ReferenceExpression{ ref->name(), operands.first(), ReferenceExpression::MemberKind::Static };
		SAFE_DELETE(ref);
		return r;
	}});
	add(new OD{ "member template", "expr :: template id", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 2);
		auto ref = DCast<ReferenceExpression>( operands[1]);
		Q_ASSERT(ref);

		auto r = new ReferenceExpression{ ref->name(), operands.first(), ReferenceExpression::MemberKind::Template };
		SAFE_DELETE(ref);
		return r;
	}});

	add(new OD{ "member", "expr . id < expr >", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 3 );
		auto ref = DCast<ReferenceExpression>( operands[1]);
		Q_ASSERT(ref);

		auto r = new ReferenceExpression{ ref->name(), operands.first() };
		SAFE_DELETE(ref);

		extractCommaInto(operands.last(), r->typeArguments(), true, false);

		return r;
	}});

	add(new OD{ "call", "expr ( expr )", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 2);

		auto opr = new MethodCallExpression{};
		opr->setCallee(operands.first());

		extractCommaInto(operands.last(), opr->arguments(), true, false);

		return opr;
	}});

	add(new OD{ "meta call", "# expr ( expr )", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 2);

		auto opr = new MetaCallExpression{};
		opr->setCallee(operands.first());

		extractCommaInto(operands.last(), opr->arguments(), true, false);

		return opr;
	}});

	add(new OD{ "type arguments", "id < expr >", 0, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 2);
		auto ref = DCast<ReferenceExpression>( operands.first());
		Q_ASSERT(ref);

		extractCommaInto(operands.last(), ref->typeArguments(), true, false);

		return ref;
	}});

	add(new OD{ "array type", "type []", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto at = new ArrayTypeExpression{};
		at->setTypeExpression(operands.first());
		return at;
	}});

	add(new OD{ "pointer type", "type *", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto at = new PointerTypeExpression{};
		at->setTypeExpression(operands.first());
		return at;
	}});

	add(new OD{ "reference type", "type &", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto at = new ReferenceTypeExpression{};
		at->setTypeExpression(operands.first());
		return at;
	}});

	add(new OD{ "const qualifier", "const type", 1, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto at = new TypeQualifierExpression{};
		at->setQualifier(Type::CONST);
		at->setTypeExpression(operands.first());
		return at;
	}});

	add(new OD{ "volatile qualifier", "volatile type", 1, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto at = new TypeQualifierExpression{};
		at->setQualifier(Type::VOLATILE);
		at->setTypeExpression(operands.first());
		return at;
	}});

	add(new OD{ "throw", "throw expr", 30, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto expr = new ThrowExpression{};
		expr->setExpr( operands.first());
		return expr;
	}});

	add(new OD{ "typename", "typename type", 1, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto expr = new TypeNameOperator{};
		expr->setTypeExpression( operands.first());
		return expr;
	}});


	// Type traits operators
	add(new OD{ "sizeof", "sizeof ( type )", 1, OD::NotAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		return new TypeTraitExpression{TypeTraitExpression::TypeTraitKind::SizeOf, operands.first()};
	}});
	add(new OD{ "alignof", "alignof ( type )", 1, OD::NotAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		return new TypeTraitExpression{TypeTraitExpression::TypeTraitKind::AlignOf, operands.first()};
	}});
	add(new OD{ "typeid", "typeid ( type )", 1, OD::NotAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		return new TypeTraitExpression{TypeTraitExpression::TypeTraitKind::TypeId, operands.first()};
	}});

	// Variable declaration
	auto varDeclFunction = [](const QList<Expression*>& operands) -> Expression*
	{
		auto vd = new VariableDeclarationExpression{};

		vd->decl()->setTypeExpression( operands.first() );
		auto ref = DCast<ReferenceExpression>(operands[1]);
		vd->decl()->setName( ref->name());
		SAFE_DELETE(ref);

		if (operands.size() > 2) vd->decl()->setInitialValue(operands[2]);
		return vd;
	};
	add(new OD{ "variable decl", "type id", 40, OD::RightAssociative, varDeclFunction});
	add(new OD{ "variable decl and initialization", "type id = expr", 40,
			OD::RightAssociative, varDeclFunction});


	// Command descriptors
	add(new CommandDescriptor{ "command without params", "\\ id SPACE", 0, OD::NotAssociative});
	add(new CommandDescriptor{ "command with params", "\\ id ( typeOrExpr )", 0, OD::NotAssociative});

	add(new CompoundObjectDescriptor{ "compound object",
			CompoundObjectDescriptor::compoundSignature(), 0, OD::NotAssociative});

	// Commands for Lambda
	CommandDescriptor::registerCommand(new CreateLambda());
}


}
