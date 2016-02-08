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

#include "StringComponents.h"

#include "ModelBase/src/nodes/Node.h"
#include "../expression_editor/CompoundObjectDescriptor.h"

#include "OOModel/src/allOOModelNodes.h"

namespace OOInteraction {

QMap<int, QStringList(*)(Model::Node* node)>& StringComponents::componentFunctions()
{
	static QMap<int, QStringList(*)(Model::Node* node)> funcs;
	return funcs;
}

StringComponents::StringComponents(Model::Node* node) : node_{node}
{}

StringComponents::~StringComponents()
{}

QStringList StringComponents::components()
{
	//if (!node_) return QStringList{};
	Q_ASSERT(node_);

	auto iter = componentFunctions().find(node_->typeId());
	if (iter != componentFunctions().end()) return iter.value()(node_);
	if (auto listNode = DCast<Model::List>(node_)) return c( list(listNode) );

	throw OOInteractionException{"No string component function registered for node of type " + node_->typeName()};
}

QString StringComponents::stringForNode(Model::Node* node)
{
	QString res = componentsForNode(node).join("");
	if (res.isNull()) res = "";
	return res;
}

QStringList StringComponents::componentsForNode(Model::Node* node)
{
	if (!node) return QStringList{};
	else return StringComponents{node}.components();
}

StringComponents::Optional StringComponents::list(Model::List* listNode)
{
	QStringList result;
	if (!listNode) return result;

	for (int i=0; i< listNode->size(); ++i)
		result << stringForNode(listNode->at<Model::Node>(i));

	return result;
}

StringComponents::Optional StringComponents::list(Model::List* listNode, const QString& prefix,
		const QString& separator, const QString& postfix, bool nothingIfEmpty, bool collapse)
{
	if (nothingIfEmpty && listNode->size() == 0)
		return Optional{};

	QStringList list;
	list << prefix;
	for (int i=0; i< listNode->size(); ++i)
	{
		if (i>0) list << separator;
		list << stringForNode(listNode->at<Model::Node>(i));
	}
	list << postfix;

	if (collapse) return list.join("");
	else return list;
}

using namespace OOModel;
void StringComponents::initConversions()
{
	// Types
	add<ArrayTypeExpression>([](ArrayTypeExpression* e){ return c( Optional{e->typeExpression(), AUTO}, "[]"); });
	add<ReferenceTypeExpression>([](ReferenceTypeExpression* e){ return c( Optional{e->typeExpression(), AUTO}, "&"); });
	add<PointerTypeExpression>([](PointerTypeExpression* e){ return c( Optional{e->typeExpression(), AUTO}, "*"); });
	add<ClassTypeExpression>([](ClassTypeExpression* e){ return c( Optional{e->typeExpression(), AUTO} ); });
	add<PrimitiveTypeExpression>([](PrimitiveTypeExpression* e){ return c(
		choose(e->typeValue(),
			PrimitiveTypeExpression::PrimitiveTypes::INT, "int",
			PrimitiveTypeExpression::PrimitiveTypes::LONG, "long",
			PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT, "uint",
			PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG, "ulong",
			PrimitiveTypeExpression::PrimitiveTypes::FLOAT, "float",
			PrimitiveTypeExpression::PrimitiveTypes::DOUBLE, "double",
			PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN, "bool",
			PrimitiveTypeExpression::PrimitiveTypes::CHAR, "char",
			PrimitiveTypeExpression::PrimitiveTypes::VOID, "void")
	); });
	add<TypeQualifierExpression>([](TypeQualifierExpression* e ){ return c(
			choose(e->qualifier(),
					TypeQualifierExpression::Qualifier::CONST, "const",
					TypeQualifierExpression::Qualifier::VOLATILE, "volatile"),
			" ", e->typeExpression() ); });
	add<AutoTypeExpression>([](AutoTypeExpression* ){ return c( "auto" ); });
	add<FunctionTypeExpression>([](FunctionTypeExpression* e){ return c( "[]",
			list(e->arguments(), "(", ",", ")", false, true),
			Optional{"->", e->results()->size() > 0},
			list(e->results(), "(", ",", ")", true, true)
	);});

	// Operators
	add<AssignmentExpression>([](AssignmentExpression* e){ return c(
		QString{}, e->left(), choose(e->op(),
			AssignmentExpression::ASSIGN, "=",
			AssignmentExpression::PLUS_ASSIGN, "+=",
			AssignmentExpression::MINUS_ASSIGN, "-=",
			AssignmentExpression::TIMES_ASSIGN, "*=",
			AssignmentExpression::DIVIDE_ASSIGN, "/=",
			AssignmentExpression::BIT_AND_ASSIGN, "&=",
			AssignmentExpression::BIT_OR_ASSIGN, "|=",
			AssignmentExpression::BIT_XOR_ASSIGN, "^=",
			AssignmentExpression::REMAINDER_ASSIGN, "%=",
			AssignmentExpression::LEFT_SHIFT_ASSIGN, "<<=",
			AssignmentExpression::RIGHT_SHIFT_SIGNED_ASSIGN, ">>=",
			AssignmentExpression::RIGHT_SHIFT_UNSIGNED_ASSIGN, ">>>="),
		e->right(), QString{}
	); });
	add<BinaryOperation>([](BinaryOperation* e){ return c(
		QString{}, e->left(), choose(e->op(),
			BinaryOperation::TIMES, "*",
			BinaryOperation::DIVIDE, "/",
			BinaryOperation::REMAINDER, "%",
			BinaryOperation::PLUS, "+",
			BinaryOperation::MINUS, "-",
			BinaryOperation::LEFT_SHIFT, "<<",
			BinaryOperation::RIGHT_SHIFT_SIGNED, ">>",
			BinaryOperation::RIGHT_SHIFT_UNSIGNED, ">>>",
			BinaryOperation::LESS, "<",
			BinaryOperation::GREATER, ">",
			BinaryOperation::LESS_EQUALS, "<=",
			BinaryOperation::GREATER_EQUALS, ">=",
			BinaryOperation::EQUALS, "==",
			BinaryOperation::NOT_EQUALS, "!=",
			BinaryOperation::XOR, "^",
			BinaryOperation::AND, "&",
			BinaryOperation::OR, "|",
			BinaryOperation::CONDITIONAL_AND, "&&",
			BinaryOperation::CONDITIONAL_OR, "||",
			BinaryOperation::ARRAY_INDEX, "["),
		e->right(),
		e->op() == OOModel::BinaryOperation::ARRAY_INDEX ? "]" : QString{}
	); });
	add<UnaryOperation>([](UnaryOperation* e){ return c(
		choose(e->op(),
			UnaryOperation::PREINCREMENT, "++",
			UnaryOperation::PREDECREMENT, "--",
			UnaryOperation::POSTINCREMENT, Optional{},
			UnaryOperation::POSTDECREMENT, Optional{},
			UnaryOperation::PLUS, "+",
			UnaryOperation::MINUS, "-",
			UnaryOperation::NOT, "!",
			UnaryOperation::COMPLEMENT, "~",
			UnaryOperation::PARENTHESIS, "(",
			UnaryOperation::DEREFERENCE, "*",
			UnaryOperation::ADDRESSOF, "&"),
		e->operand(),
		choose(e->op(),
			UnaryOperation::PREINCREMENT, Optional{},
			UnaryOperation::PREDECREMENT, Optional{},
			UnaryOperation::POSTINCREMENT, "++",
			UnaryOperation::POSTDECREMENT, "--",
			UnaryOperation::PLUS, Optional{},
			UnaryOperation::MINUS, Optional{},
			UnaryOperation::NOT, Optional{},
			UnaryOperation::COMPLEMENT, Optional{},
			UnaryOperation::PARENTHESIS, ")",
			UnaryOperation::DEREFERENCE, Optional{},
			UnaryOperation::ADDRESSOF, Optional{})
	); });
	add<TypeTraitExpression>([](TypeTraitExpression* e){ return c(
			choose( static_cast<int>(e->typeTraitKind()),
				static_cast<int>(TypeTraitExpression::TypeTraitKind::SizeOf), "sizeof",
				static_cast<int>(TypeTraitExpression::TypeTraitKind::AlignOf), "alignof",
				static_cast<int>(TypeTraitExpression::TypeTraitKind::TypeId), "typeid"),
			"(",
			e->operand(),
			")"
	); });


	// Literals
	add<BooleanLiteral>([](BooleanLiteral* e){ return c( e->value() ? "true" : "false" ); });
	add<IntegerLiteral>([](IntegerLiteral* e){ return c( e->value() ); });
	add<FloatLiteral>([](FloatLiteral* e){ return c( e->value() ); });
	add<NullLiteral>([](NullLiteral*){ return c( "null" ); });
	add<StringLiteral>([](StringLiteral* e){ return c( "\"", e->value(), "\"" ); });
	add<CharacterLiteral>([](CharacterLiteral* e){ return c( "'", e->value(), "'" ); });

	// Misc
	add<CastExpression>([](CastExpression* e){ return c( "(", e->castType(), ")", e->expr() ); });
	add<InstanceOfExpression>(
			[](InstanceOfExpression* e){ return c( e->expr(), " ", "instanceof", " ", e->typeExpression() ); });
	add<CommaExpression>([](CommaExpression* e){ return c( QString{}, e->left(), ",", e->right(), QString{} ); });
	add<ConditionalExpression>([](ConditionalExpression* e){ return c(
		QString{}, e->condition(), "?", e->trueExpression(), ":", e->falseExpression(), QString{} ); });
	add<SuperExpression>([](SuperExpression* ){ return c( "super" ); });
	add<ThisExpression>([](ThisExpression* ){ return c( "this" ); });
	add<GlobalScopeExpression>([](GlobalScopeExpression* ){ return c( "::" ); });
	add<ThrowExpression>([](ThrowExpression* e ){ return c( "throw", " ", e->expr() ); });
	add<TypeNameOperator>([](TypeNameOperator* e ){ return c( "typename", " ", e->typeExpression() ); });
	add<DeleteExpression>([](DeleteExpression* e ){ return c( e->isArray() ? "delete[]":"delete", " ", e->expr() ); });
	add<VariableDeclarationExpression>([](VariableDeclarationExpression* e ){ return c( e->decl()->typeExpression(), " ",
			e->decl()->name(), Optional{"=", e->decl()->initialValue()}, Optional{e->decl()->initialValue()}); });

	add<LambdaExpression>([](LambdaExpression* e ){ return c( CompoundObjectDescriptor::storeExpression(e)); });

	add<ArrayInitializer>([](ArrayInitializer* e){ return c( list(e->values(), "{", ",", "}", false, false) ); });

	add<MethodCallExpression>([](MethodCallExpression* e){ return c(
		e->callee(), list(e->arguments(), "(", ",", ")", false, true) ); });

	add<MetaCallExpression>([](MetaCallExpression* e){ return c( "#",
		e->callee(), list(e->arguments(), "(", ",", ")", false, true) ); });

	add<NewExpression>([](NewExpression* e){ return c( "new", " ",
			Optional{ (e->dimensions()->size() > 0 || !e->initializer()) ? e->newType() : nullptr, AUTO},
			list(e->dimensions(), "[", ",", "]", true, true), Optional{e->initializer(), AUTO} ); });

	add<ReferenceExpression>([](ReferenceExpression* e){ return c(
		Optional{e->prefix(), AUTO}, Optional{
						(e->memberKind() == ReferenceExpression::MemberKind::Dot ? "." :
						 e->memberKind() == ReferenceExpression::MemberKind::Pointer ? "->" :
						 e->memberKind() == ReferenceExpression::MemberKind::Static ? "::" : "::template"),
						e->prefix()}, e->name(),
		list(e->typeArguments(), "<", ",", ">", true, true) ); });

	add<OOReference>([](OOReference* e){ return c( e->name() ); });

	// Flexible input expressions
	add<EmptyExpression>([](EmptyExpression*){ return c( "" ); });
	add<ErrorExpression>([](ErrorExpression* e){ return c(
		Optional{e->prefix(), !e->prefix().isEmpty()}, e->arg(), Optional{e->postfix(), !e->postfix().isEmpty()} ); });
	add<UnfinishedOperator>([](UnfinishedOperator* e)
	{
		QStringList result;

		for (int i=0; i< e->operands()->size(); ++i)
		{
			QString delim = e->delimiters()->at(i)->get();

			result << delim << stringForNode(e->operands()->at(i));
		}

		if (e->delimiters()->size() > e->operands()->size())
		{
			QString delim = e->delimiters()->last()->get();
			result << delim;
		}

		return result;
	});
}

}
