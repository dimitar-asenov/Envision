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

#include "expression_editor/OOOperatorDescriptorList.h"

#include "expression_editor/CommandDescriptor.h"
#include "expression_editor/CompoundObjectDescriptor.h"

#include "OOModel/src/allOOModelNodes.h"

using namespace OOModel;
using namespace Interaction;

namespace OOInteraction {

OOOperatorDescriptorList* OOOperatorDescriptorList::instance()
{
	static OOOperatorDescriptorList theInstance;
	return &theInstance;
}

void OOOperatorDescriptorList::initializeWithDefaultOperators()
{
	using OD = OOOperatorDescriptor;
	// Unary operators
	add(new OD("preincrement", "++ expr", 2, OD::RightAssociative,	OD::unary<UnaryOperation::PREINCREMENT>));
	add(new OD("predecrement", "-- expr", 2, OD::RightAssociative,	OD::unary<UnaryOperation::PREDECREMENT>));
	add(new OD("postincrement", "expr ++", 2, OD::LeftAssociative, OD::unary<UnaryOperation::POSTINCREMENT>));
	add(new OD("postdecrement", "expr --", 2, OD::LeftAssociative, OD::unary<UnaryOperation::POSTDECREMENT>));
	add(new OD("unary plus", "+ expr", 2, OD::RightAssociative,	OD::unary<UnaryOperation::PLUS>));
	add(new OD("unary minus", "- expr", 2, OD::RightAssociative, OD::unary<UnaryOperation::MINUS>));
	add(new OD("not", "! expr", 2, OD::RightAssociative, OD::unary<UnaryOperation::NOT>));
	add(new OD("complement", "~ expr", 2, OD::RightAssociative, OD::unary<UnaryOperation::COMPLEMENT>));
	add(new OD("parenthesis", "( expr )", 1, OD::NotAssociative, OD::unary<UnaryOperation::PARENTHESIS>));
	add(new OD("dereference", "* expr", 2, OD::RightAssociative, OD::unary<UnaryOperation::DEREFERENCE>));
	add(new OD("addressof", "& expr", 2, OD::RightAssociative, OD::unary<UnaryOperation::ADDRESSOF>));

	// Binary operators
	add(new OD("times", "expr * expr", 3, OD::LeftAssociative, OD::binary<BinaryOperation::TIMES>));
	add(new OD("divide", "expr / expr", 3, OD::LeftAssociative, OD::binary<BinaryOperation::DIVIDE>));
	add(new OD("remainder", "expr % expr", 3, OD::LeftAssociative, OD::binary<BinaryOperation::REMAINDER>));
	add(new OD("plus", "expr + expr", 4, OD::LeftAssociative, OD::binary<BinaryOperation::PLUS>));
	add(new OD("minus", "expr - expr", 4, OD::LeftAssociative, OD::binary<BinaryOperation::MINUS>));
	add(new OD("left shift", "expr << expr", 5, OD::LeftAssociative, OD::binary<BinaryOperation::LEFT_SHIFT>));
	add(new OD("right shift signed", "expr >> expr", 5, OD::LeftAssociative,
			OD::binary<BinaryOperation::RIGHT_SHIFT_SIGNED>));
	add(new OD("righ shift unsigned", "expr >>> expr", 5, OD::LeftAssociative,
			OD::binary<BinaryOperation::RIGHT_SHIFT_UNSIGNED>));
	add(new OD("less", "expr < expr", 6, OD::LeftAssociative, OD::binary<BinaryOperation::LESS>));
	add(new OD("greater", "expr > expr", 6, OD::LeftAssociative, OD::binary<BinaryOperation::GREATER>));
	add(new OD("less equals", "expr <= expr", 6, OD::LeftAssociative, OD::binary<BinaryOperation::LESS_EQUALS>));
	add(new OD("greater equals", "expr >= expr", 6, OD::LeftAssociative,
			OD::binary<BinaryOperation::GREATER_EQUALS>));
	add(new OD("equals", "expr == expr", 7, OD::LeftAssociative, OD::binary<BinaryOperation::EQUALS>));
	add(new OD("not equals", "expr != expr", 7, OD::LeftAssociative, OD::binary<BinaryOperation::NOT_EQUALS>));
	add(new OD("bitwise xor", "expr ^ expr", 9, OD::LeftAssociative, OD::binary<BinaryOperation::XOR>));
	add(new OD("bitwise and", "expr & expr", 8, OD::LeftAssociative, OD::binary<BinaryOperation::AND>));
	add(new OD("bitwise or", "expr | expr", 10, OD::LeftAssociative, OD::binary<BinaryOperation::OR>));
	add(new OD("conditional and", "expr && expr", 11, OD::LeftAssociative,
			OD::binary<BinaryOperation::CONDITIONAL_AND>));
	add(new OD("conditional or", "expr || expr", 12, OD::LeftAssociative,
			OD::binary<BinaryOperation::CONDITIONAL_OR>));
	add(new OD("array index", "expr [ expr ]", 1, OD::LeftAssociative, OD::binary<BinaryOperation::ARRAY_INDEX>));

	// Ternary operator
	add(new OD("conditional expression", "expr ? expr : expr", 13, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new ConditionalExpression();
		opr->setCondition(operands.first());
		opr->setTrueExpression(operands.at(1));
		opr->setFalseExpression(operands.last());
		return opr;
	}));

	// Assignment
	add(new OD("assign", "expr = expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::ASSIGN>));
	add(new OD("assign", "expr += expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::PLUS_ASSIGN>));
	add(new OD("assign", "expr -= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::MINUS_ASSIGN>));
	add(new OD("assign", "expr *= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::TIMES_ASSIGN>));
	add(new OD("assign", "expr /= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::DIVIDE_ASSIGN>));
	add(new OD("assign", "expr &= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::BIT_AND_ASSIGN>));
	add(new OD("assign", "expr |= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::BIT_OR_ASSIGN>));
	add(new OD("assign", "expr ^= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::BIT_XOR_ASSIGN>));
	add(new OD("assign", "expr %= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::REMAINDER_ASSIGN>));
	add(new OD("assign", "expr <<= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::LEFT_SHIFT_ASSIGN>));
	add(new OD("assign", "expr >>= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::RIGHT_SHIFT_SIGNED_ASSIGN>));
	add(new OD("assign", "expr >>>= expr", 14, OD::RightAssociative,
			OD::assignment<AssignmentExpression::RIGHT_SHIFT_UNSIGNED_ASSIGN>));

	// Primitive Types
	add(new OD( "int", "int", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT); }));
	add(new OD( "long", "long", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::LONG); }));
	add(new OD( "uint", "uint", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT); }));
	add(new OD( "ulong", "ulong", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG); }));
	add(new OD( "float", "float", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::FLOAT); }));
	add(new OD( "double", "double", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::DOUBLE); }));
	add(new OD( "bool", "bool", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN); }));
	add(new OD( "char", "char", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::CHAR); }));
	add(new OD( "void", "void", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID); }));

	// Function Type
	add(new OD( "void function type", "[] ( expr )", 1, OD::NotAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 1);
		auto fte = new FunctionTypeExpression();

		if (auto comma = dynamic_cast<CommaExpression*>(operands.first()))
		{
			for(auto ee : comma->allSubOperands(true))
				fte->arguments()->append(ee);

			SAFE_DELETE(comma);
		}
		else
			fte->arguments()->append(operands.first());

		return fte;
	}));
	add(new OD( "void function type", "[] ( expr ) -> ( expr )", 1, OD::NotAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 2);
		auto fte = new FunctionTypeExpression();

		if (auto comma = dynamic_cast<CommaExpression*>(operands.first()))
		{
			for(auto ee : comma->allSubOperands(true))
				fte->arguments()->append(ee);

			SAFE_DELETE(comma);
		}
		else
			fte->arguments()->append(operands.first());

		if (auto comma = dynamic_cast<CommaExpression*>(operands.last()))
		{
			for(auto ee : comma->allSubOperands(true))
				fte->results()->append(ee);

			SAFE_DELETE(comma);
		}
		else
			fte->results()->append(operands.last());

		return fte;
	}));


	// Keywords
	add(new OD( "this", "this", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new ThisExpression(); }));
	add(new OD( "auto", "auto", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new AutoTypeExpression(); }));
	add(new OD( "null", "null", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new NullLiteral(); }));
	add(new OD( "true", "true", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new BooleanLiteral(true); }));
	add(new OD( "false", "false", 0, OD::NotAssociative, [](const QList<Expression*>&) -> Expression* {
		return new BooleanLiteral(false); }));

	// Others
	add(new OD( "cast", "( expr ) expr", 2, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new CastExpression();
		opr->setType(operands.first());
		opr->setExpr(operands.last());
		return opr;
	}));

	add(new OD( "comma", "expr , expr", 50, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new CommaExpression();
		opr->setLeft(operands.first());
		opr->setRight(operands.last());
		return opr;
	}));

	add(new OD( "initializer", "{ expr }", 0, OD::NotAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new ArrayInitializer();

		for(auto e: operands)
		{
			if (auto comma = dynamic_cast<CommaExpression*>(e))
			{
				for(auto ee : comma->allSubOperands(true))
					opr->values()->append(ee);

				SAFE_DELETE(comma);
			}
			else
				opr->values()->append(e);
		}

		return opr;
	}));

	add(new OD( "new object", "new SPACE expr", 2, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new NewExpression();
		opr->setNewType( operands.first());
		return opr;
	}));

	add(new OD( "new array", "new SPACE expr [ expr ]", 2, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto opr = new NewExpression();
		opr->setNewType( operands.first());
		opr->setAmount(operands.last());
		return opr;
	}));

	add(new OD( "delete object", "delete SPACE expr", 2, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto expr = new DeleteExpression();
		expr->setExpr( operands.first());
		return expr;
	}));

	add(new OD( "delete array", "delete [] SPACE expr", 2, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto expr = new DeleteExpression();
		expr->setIsArray(true);
		expr->setExpr( operands.first());
		return expr;
	}));

	add(new OD( "member", "expr . id", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 2);
		auto ref = dynamic_cast<ReferenceExpression*>( operands[1]);
		Q_ASSERT(ref);

		auto r = new ReferenceExpression( ref->name(), operands.first() );
		SAFE_DELETE(ref);
		return r;
	}));

	add(new OD( "member", "expr . id < expr >", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 3 );
		auto ref = dynamic_cast<ReferenceExpression*>( operands[1]);
		Q_ASSERT(ref);

		auto r = new ReferenceExpression( ref->name(), operands.first() );
		SAFE_DELETE(ref);

		if (auto comma = dynamic_cast<CommaExpression*>(operands.last()))
		{
			for(auto arg : comma->allSubOperands(true))
				r->typeArguments()->append(arg);

			SAFE_DELETE(comma);
		}
		else
			if (!dynamic_cast<EmptyExpression*>(operands.last()) )
				r->typeArguments()->append(operands.last());

		return r;
	}));

	add(new OD( "call", "expr ( expr )", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 2);
		auto ref = dynamic_cast<ReferenceExpression*>( operands.first());
		Q_ASSERT(ref);

		auto opr = new MethodCallExpression();
		opr->setRef(ref);

		if (auto comma = dynamic_cast<CommaExpression*>(operands.last()))
		{
			for(auto arg : comma->allSubOperands(true))
				opr->arguments()->append(arg);

			SAFE_DELETE(comma);
		}
		else
			if (!dynamic_cast<EmptyExpression*>(operands.last()) )
					opr->arguments()->append(operands.last());

		return opr;
	}));

	add(new OD( "type arguments", "id < expr >", 0, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		Q_ASSERT(operands.size() == 2);
		auto ref = dynamic_cast<ReferenceExpression*>( operands.first());
		Q_ASSERT(ref);

		if (auto comma = dynamic_cast<CommaExpression*>(operands.last()))
		{
			for(auto arg : comma->allSubOperands(true))
				ref->typeArguments()->append(arg);

			SAFE_DELETE(comma);
		}
		else
			if (!dynamic_cast<EmptyExpression*>(operands.last()) )
					ref->typeArguments()->append(operands.last());

		return ref;
	}));

	add(new OD( "array type", "expr []", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto at = new ArrayTypeExpression();
		at->setTypeExpression(operands.first());
		return at;
	}));

	add(new OD( "pointer type", "expr *", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto at = new PointerTypeExpression();
		at->setTypeExpression(operands.first());
		return at;
	}));

	add(new OD( "reference type", "expr &", 1, OD::LeftAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto at = new ReferenceTypeExpression();
		at->setTypeExpression(operands.first());
		return at;
	}));

	add(new OD( "const qualifier", "const SPACE expr", 1, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto at = new TypeQualifierExpression();
		at->setQualifier(Type::CONST);
		at->setTypeExpression(operands.first());
		return at;
	}));

	add(new OD( "volatile qualifier", "volatile SPACE expr", 1, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto at = new TypeQualifierExpression();
		at->setQualifier(Type::VOLATILE);
		at->setTypeExpression(operands.first());
		return at;
	}));

	// Variable declaration
	auto varDeclFunction = [](const QList<Expression*>& operands) -> Expression*
	{
		auto vd = new VariableDeclarationExpression();

		vd->decl()->setTypeExpression( operands.first() );
		auto ref = dynamic_cast<ReferenceExpression*>(operands[1]);
		vd->decl()->setName( ref->name());
		SAFE_DELETE(ref);

		if (operands.size() > 2) vd->decl()->setInitialValue(operands[2]);
		return vd;
	};
	add(new OD( "variable decl", "expr SPACE id", 40, OD::RightAssociative, varDeclFunction));
	add(new OD( "variable decl and initialization", "expr SPACE id = expr", 40,
			OD::RightAssociative, varDeclFunction));

	add(new OD( "throw", "throw SPACE expr", 30, OD::RightAssociative,
			[](const QList<Expression*>& operands) -> Expression* {
		auto expr = new ThrowExpression();
		expr->setExpr( operands.first());
		return expr;
	}));

	// Command descriptors
	add(new CommandDescriptor( "command without params", "\\ id SPACE", 0, OD::NotAssociative));
	add(new CommandDescriptor( "command with params", "\\ id ( expr )", 0, OD::NotAssociative));

	add(new CompoundObjectDescriptor( "compound object",
			CompoundObjectDescriptor::compoundSignature(), 0, OD::NotAssociative));
}


} /* namespace OOInteraction */
