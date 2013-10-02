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

#include "CppImportUtilities.h"
#include "CppImportException.h"
#include "visitors/ExpressionVisitor.h"

namespace CppImport {


CppImportUtilities::CppImportUtilities(CppImportLogger* logger, ExpressionVisitor* visitor)
: log_{logger}, exprVisitor_{visitor}
{}

OOModel::Expression* CppImportUtilities::translateQualifiedType(const clang::QualType qualType,
																					 const clang::SourceLocation& location)
{
	OOModel::Expression* translatedType = translateTypePtr(qualType.getTypePtr(), location);

	if(qualType.isConstQualified() && qualType.isVolatileQualified())
		return new OOModel::TypeQualifierExpression(OOModel::Type::CONST, new OOModel::TypeQualifierExpression
																  (OOModel::Type::VOLATILE, translatedType));
	else if(qualType.isConstQualified())
		return new OOModel::TypeQualifierExpression(OOModel::Type::CONST, translatedType);
	else if(qualType.isVolatileQualified())
		return new OOModel::TypeQualifierExpression(OOModel::Type::VOLATILE, translatedType);

	return translatedType;
}

OOModel::BinaryOperation::OperatorTypes CppImportUtilities::translateBinaryOp
(const clang::BinaryOperatorKind& binaryOpKind)
{
	switch (binaryOpKind)
	{
		case clang::BO_PtrMemD: return OOModel::BinaryOperation::POINTER_TO_MEMBER;
		case clang::BO_PtrMemI: return OOModel::BinaryOperation::POINTER_POINTER_TO_MEMBER;
		case clang::BO_Mul: return OOModel::BinaryOperation::TIMES;
		case clang::BO_Div: return OOModel::BinaryOperation::DIVIDE;
		case clang::BO_Rem: return OOModel::BinaryOperation::REMAINDER;
		case clang::BO_Add: return OOModel::BinaryOperation::PLUS;
		case clang::BO_Sub: return OOModel::BinaryOperation::MINUS;
		case clang::BO_Shl: return OOModel::BinaryOperation::LEFT_SHIFT;
		case clang::BO_Shr: return OOModel::BinaryOperation::RIGHT_SHIFT_SIGNED;
		case clang::BO_LT: return OOModel::BinaryOperation::LESS;
		case clang::BO_GT: return OOModel::BinaryOperation::GREATER;
		case clang::BO_LE: return OOModel::BinaryOperation::LESS_EQUALS;
		case clang::BO_GE: return OOModel::BinaryOperation::GREATER_EQUALS;
		case clang::BO_EQ: return OOModel::BinaryOperation::EQUALS;
		case clang::BO_NE: return OOModel::BinaryOperation::NOT_EQUALS;
		case clang::BO_And: return OOModel::BinaryOperation::AND;
		case clang::BO_Xor: return OOModel::BinaryOperation::XOR;
		case clang::BO_Or: return OOModel::BinaryOperation::OR;
		case clang::BO_LAnd: return OOModel::BinaryOperation::CONDITIONAL_AND;
		case clang::BO_LOr: return OOModel::BinaryOperation::CONDITIONAL_OR;
		default: throw CppImportException("Impossible binary operator");
	}
}

OOModel::AssignmentExpression::AssignmentTypes CppImportUtilities::translateAssignOp
(const clang::BinaryOperatorKind& assignOpKind)
{
	switch (assignOpKind)
	{
		case clang::BO_Assign: return OOModel::AssignmentExpression::ASSIGN;
		case clang::BO_MulAssign: return OOModel::AssignmentExpression::TIMES_ASSIGN;
		case clang::BO_DivAssign: return OOModel::AssignmentExpression::DIVIDE_ASSIGN;
		case clang::BO_RemAssign: return OOModel::AssignmentExpression::REMAINDER_ASSIGN;
		case clang::BO_AddAssign: return OOModel::AssignmentExpression::PLUS_ASSIGN;
		case clang::BO_SubAssign: return OOModel::AssignmentExpression::MINUS_ASSIGN;
		case clang::BO_ShlAssign: return OOModel::AssignmentExpression::LEFT_SHIFT_ASSIGN;
		case clang::BO_ShrAssign: return OOModel::AssignmentExpression::RIGHT_SHIFT_SIGNED_ASSIGN;
		case clang::BO_AndAssign: return OOModel::AssignmentExpression::BIT_AND_ASSIGN;
		case clang::BO_XorAssign: return OOModel::AssignmentExpression::BIT_XOR_ASSIGN;
		case clang::BO_OrAssign: return OOModel::AssignmentExpression::BIT_OR_ASSIGN;
		default: throw CppImportException("Impossible assign operator");
	}
}

OOModel::UnaryOperation::OperatorTypes CppImportUtilities::translateUnaryOp
(const clang::UnaryOperatorKind& unaryOpKind)
{
	switch (unaryOpKind)
	{
		case clang::UO_PostInc: return OOModel::UnaryOperation::POSTINCREMENT;
		case clang::UO_PostDec: return OOModel::UnaryOperation::POSTDECREMENT;
		case clang::UO_PreInc: return OOModel::UnaryOperation::PREINCREMENT;
		case clang::UO_PreDec: return OOModel::UnaryOperation::PREDECREMENT;
		case clang::UO_AddrOf: return OOModel::UnaryOperation::ADDRESSOF;
		case clang::UO_Deref: return OOModel::UnaryOperation::DEREFERENCE;
		case clang::UO_Plus: return OOModel::UnaryOperation::PLUS;
		case clang::UO_Minus: return OOModel::UnaryOperation::MINUS;
		case clang::UO_Not: return OOModel::UnaryOperation::COMPLEMENT;
		case clang::UO_LNot: return OOModel::UnaryOperation::NOT;
		default: throw CppImportException("Unary operation not supported");
	}
}

OOModel::Modifier::ModifierFlag CppImportUtilities::translateAccessSpecifier
(const clang::AccessSpecifier& acessSpecifier)
{
	switch (acessSpecifier)
	{
		case clang::AS_public: return OOModel::Modifier::Public;
		case clang::AS_protected: return OOModel::Modifier::Protected;
		case clang::AS_private: return OOModel::Modifier::Private;
		case clang::AS_none: return OOModel::Modifier::None;
		default: return OOModel::Modifier::None;
	}
}

OOModel::Modifier::ModifierFlag CppImportUtilities::translateStorageSpecifier
(const clang::StorageClass& storageClass)
{
	switch(storageClass)
	{
		case clang::SC_None: return OOModel::Modifier::None;
		case clang::SC_Static: return OOModel::Modifier::Static;
		default:
			log_->storageClassNotSupported(storageClass);
			return OOModel::Modifier::None;
	}
}

OOModel::Expression* CppImportUtilities::translateNestedNameSpecifier
(const clang::NestedNameSpecifier* nestedName, const clang::SourceLocation& location, OOModel::Expression* base)
{
	OOModel::ReferenceExpression* currentRef = nullptr;
	OOModel::Expression* returnExpr = nullptr;
	switch(nestedName->getKind())
	{
		case clang::NestedNameSpecifier::Identifier:
			currentRef = new OOModel::ReferenceExpression(nestedName->getAsIdentifier()->getNameStart());
			break;
		case clang::NestedNameSpecifier::Namespace:
			currentRef = new OOModel::ReferenceExpression
					(QString::fromStdString(nestedName->getAsNamespace()->getNameAsString()));
			break;
		case clang::NestedNameSpecifier::NamespaceAlias:
			currentRef = new OOModel::ReferenceExpression
					(QString::fromStdString(nestedName->getAsNamespaceAlias()->getNameAsString()));
			break;
		case clang::NestedNameSpecifier::TypeSpec:
			// TODO: handle the case where this cast is not a reference
			if(auto typeRef = DCast<OOModel::ReferenceExpression>
					(translateTypePtr(nestedName->getAsType(), location)))
				currentRef = typeRef;
			else
			{
				log_->writeError(className_, location, CppImportLogger::Reason::OTHER,
									  "Unsupported NestedNameSpecifier TypeSpecNameSpecifier");
				returnExpr = createErrorExpression("TypeSpecNameSpecifier");
			}
			break;
		case clang::NestedNameSpecifier::TypeSpecWithTemplate:
			// TODO: handle the case where this cast is not a reference
			if(auto typeRef = DCast<OOModel::ReferenceExpression>
					(translateTypePtr(nestedName->getAsType(), location)))
				currentRef = typeRef;
			else
			{
				log_->writeError(className_, location, CppImportLogger::Reason::OTHER,
									  "Unsupported NestedNameSpecifier TypeSpecWithTemplate");
				returnExpr = createErrorExpression("Could not translate TypeSpecWithTemplate");
			}
			break;
		case clang::NestedNameSpecifier::Global:
			// if we have the Global specifier there can not be a prefix() other wise it is invalid C++
			Q_ASSERT(!nestedName->getPrefix());
			returnExpr = new OOModel::GlobalScopeExpression();
	}
	if(auto prefix = nestedName->getPrefix())
		currentRef->setPrefix(translateNestedNameSpecifier(prefix, location, base));
	else if(currentRef && base)
		currentRef->setPrefix(base);
	else if(returnExpr && base)
		throw CppImportException("Invalid c++ expression");
	if(currentRef) return currentRef;
	return returnExpr;
}

OOModel::Expression* CppImportUtilities::translateTemplateArgument(const clang::TemplateArgument& templateArg,
																						 const clang::SourceLocation& location)
{
	switch(templateArg.getKind())
	{
		case clang::TemplateArgument::ArgKind::Null:
			return new OOModel::EmptyExpression();
		case clang::TemplateArgument::ArgKind::Type:
			return translateQualifiedType(templateArg.getAsType(), location);
		case clang::TemplateArgument::ArgKind::Declaration:
			return new OOModel::ReferenceExpression(QString::fromStdString(templateArg.getAsDecl()->getNameAsString()));
		case clang::TemplateArgument::ArgKind::NullPtr:
			return new OOModel::NullLiteral();
		case clang::TemplateArgument::ArgKind::Integral:
			return new OOModel::IntegerLiteral(templateArg.getAsIntegral().getLimitedValue());
		case clang::TemplateArgument::ArgKind::Template:
			return new OOModel::ReferenceExpression(
						QString::fromStdString(templateArg.getAsTemplate().getAsTemplateDecl()->getNameAsString()));
		case clang::TemplateArgument::ArgKind::TemplateExpansion:
			// TODO: add support
			log_->writeError(className_, location, CppImportLogger::Reason::OTHER,
								  "Unsupported TemplateArgument EXPANSION");
			return createErrorExpression("Unsupported TemplateArgument EXPANSION");
		case clang::TemplateArgument::ArgKind::Expression:
			return exprVisitor_->translateExpression(templateArg.getAsExpr());
		case clang::TemplateArgument::ArgKind::Pack:
			// TODO: add support
			log_->writeError(className_, location, CppImportLogger::Reason::OTHER,
								  "Unsupported TemplateArgument PACK");
			return createErrorExpression("Unsupported TemplateArgument PACK");
		default:
			throw CppImportException("Invalid Template Argument kind");
	}
}

OOModel::BinaryOperation::OperatorTypes CppImportUtilities::translateBinaryOverloadOp
(const clang::OverloadedOperatorKind& overloadOpKind)
{
	switch(overloadOpKind)
	{
		case clang::OO_Plus: return OOModel::BinaryOperation::PLUS;
		case clang::OO_Minus: return OOModel::BinaryOperation::MINUS;
		case clang::OO_Star: return OOModel::BinaryOperation::TIMES;
		case clang::OO_Slash: return OOModel::BinaryOperation::DIVIDE;
		case clang::OO_Percent: return OOModel::BinaryOperation::REMAINDER;
		case clang::OO_Caret: return OOModel::BinaryOperation::XOR;
		case clang::OO_Amp: return OOModel::BinaryOperation::AND;
		case clang::OO_Pipe: return OOModel::BinaryOperation::OR;
		case clang::OO_Less: return OOModel::BinaryOperation::LESS;
		case clang::OO_Greater: return OOModel::BinaryOperation::GREATER;
		case clang::OO_LessLess: return OOModel::BinaryOperation::LEFT_SHIFT;
		case clang::OO_GreaterGreater: return OOModel::BinaryOperation::RIGHT_SHIFT_SIGNED;
		case clang::OO_EqualEqual: return OOModel::BinaryOperation::EQUALS;
		case clang::OO_ExclaimEqual: return OOModel::BinaryOperation::NOT_EQUALS;
		case clang::OO_LessEqual: return OOModel::BinaryOperation::LESS_EQUALS;
		case clang::OO_GreaterEqual: return OOModel::BinaryOperation::GREATER_EQUALS;
		case clang::OO_AmpAmp: return OOModel::BinaryOperation::CONDITIONAL_AND;
		case clang::OO_PipePipe: return OOModel::BinaryOperation::CONDITIONAL_OR;
		case clang::OO_ArrowStar: return OOModel::BinaryOperation::POINTER_POINTER_TO_MEMBER;
		case clang::OO_Subscript: return OOModel::BinaryOperation::ARRAY_INDEX;
		default: throw( new CppImportException("Invalid binary overload operator"));
	}
}

OOModel::AssignmentExpression::AssignmentTypes CppImportUtilities::translateAssignOverloadOp
(const clang::OverloadedOperatorKind& overloadOpKind)
{
	switch(overloadOpKind)
	{
		case clang::OO_Equal: return OOModel::AssignmentExpression::ASSIGN;
		case clang::OO_PlusEqual: return OOModel::AssignmentExpression::PLUS_ASSIGN;
		case clang::OO_MinusEqual: return OOModel::AssignmentExpression::MINUS_ASSIGN;
		case clang::OO_StarEqual: return OOModel::AssignmentExpression::TIMES_ASSIGN;
		case clang::OO_SlashEqual: return OOModel::AssignmentExpression::DIVIDE_ASSIGN;
		case clang::OO_PercentEqual: return OOModel::AssignmentExpression::REMAINDER_ASSIGN;
		case clang::OO_CaretEqual: return OOModel::AssignmentExpression::BIT_XOR_ASSIGN;
		case clang::OO_AmpEqual: return OOModel::AssignmentExpression::BIT_AND_ASSIGN;
		case clang::OO_PipeEqual: return OOModel::AssignmentExpression::BIT_OR_ASSIGN;
		case clang::OO_LessLessEqual: return OOModel::AssignmentExpression::LEFT_SHIFT_ASSIGN;
		case clang::OO_GreaterGreaterEqual: return OOModel::AssignmentExpression::RIGHT_SHIFT_SIGNED_ASSIGN;
		default: throw( new CppImportException("Invalid assign overload operator"));
	}
}

OOModel::UnaryOperation::OperatorTypes CppImportUtilities::translateUnaryOverloadOp
(const clang::OverloadedOperatorKind& overloadOpKind, const unsigned numArgs)
{
	switch(overloadOpKind)
	{
		case clang::OO_Plus: return OOModel::UnaryOperation::PLUS;
		case clang::OO_Minus: return OOModel::UnaryOperation::MINUS;
		case clang::OO_Star: return OOModel::UnaryOperation::DEREFERENCE;
		case clang::OO_Amp: return OOModel::UnaryOperation::ADDRESSOF;
		case clang::OO_Tilde: return OOModel::UnaryOperation::COMPLEMENT;
		case clang::OO_Exclaim: return OOModel::UnaryOperation::NOT;
		case clang::OO_PlusPlus: if(1 == numArgs) return OOModel::UnaryOperation::PREINCREMENT;
			return OOModel::UnaryOperation::POSTINCREMENT;
		case clang::OO_MinusMinus: if(1 == numArgs) return OOModel::UnaryOperation::PREDECREMENT;
			return OOModel::UnaryOperation::POSTDECREMENT;
		default: throw( new CppImportException("Invalid unary overload operator"));
	}
}

CppImportUtilities::OverloadKind CppImportUtilities::getOverloadKind
(const clang::OverloadedOperatorKind& overloadOpKind, unsigned numArgs)
{
	switch(overloadOpKind)
	{
		// The first four cases are not supported therefore they fallthrough
		case clang::OO_New:
		case clang::OO_Delete:
		case clang::OO_Array_New:
		case clang::OO_Array_Delete: return OverloadKind::Unsupported;
		case clang::OO_Plus: if(1 == numArgs) return OverloadKind::Unary;
			return OverloadKind::Binary;
		case clang::OO_Minus: if(1 == numArgs) return OverloadKind::Unary;
			return OverloadKind::Binary;
		case clang::OO_Star: if(1 == numArgs) return OverloadKind::Unary;
			return OverloadKind::Binary;
		case clang::OO_Slash: return OverloadKind::Binary;
		case clang::OO_Percent: return OverloadKind::Binary;
		case clang::OO_Caret: return OverloadKind::Binary;
		case clang::OO_Amp: if(1 == numArgs) return OverloadKind::Unary;
			return OverloadKind::Binary;
		case clang::OO_Pipe: return OverloadKind::Binary;
		case clang::OO_Tilde: return OverloadKind::Unary;
		case clang::OO_Exclaim: return OverloadKind::Unary;
		case clang::OO_Equal: return OverloadKind::Assign;
		case clang::OO_Less: return OverloadKind::Binary;
		case clang::OO_Greater: return OverloadKind::Binary;
		case clang::OO_PlusEqual: return OverloadKind::Assign;
		case clang::OO_MinusEqual: return OverloadKind::Assign;
		case clang::OO_StarEqual: return OverloadKind::Assign;
		case clang::OO_SlashEqual: return OverloadKind::Assign;
		case clang::OO_PercentEqual: return OverloadKind::Assign;
		case clang::OO_CaretEqual: return OverloadKind::Assign;
		case clang::OO_AmpEqual: return OverloadKind::Assign;
		case clang::OO_PipeEqual: return OverloadKind::Assign;
		case clang::OO_LessLess: return OverloadKind::Binary;
		case clang::OO_GreaterGreater: return OverloadKind::Binary;
		case clang::OO_LessLessEqual: return OverloadKind::Assign;
		case clang::OO_GreaterGreaterEqual: return OverloadKind::Assign;
		case clang::OO_EqualEqual: return OverloadKind::Binary;
		case clang::OO_ExclaimEqual: return OverloadKind::Binary;
		case clang::OO_LessEqual: return OverloadKind::Binary;
		case clang::OO_GreaterEqual: return OverloadKind::Binary;
		case clang::OO_AmpAmp: return OverloadKind::Binary;
		case clang::OO_PipePipe: return OverloadKind::Binary;
		case clang::OO_PlusPlus: return OverloadKind::Unary;
		case clang::OO_MinusMinus: return OverloadKind::Unary;
		case clang::OO_Comma: return OverloadKind::Comma;
		case clang::OO_ArrowStar: return OverloadKind::Binary;
		case clang::OO_Arrow: return OverloadKind::ReferenceExpr;
		case clang::OO_Call: return OverloadKind::MethodCall;
		case clang::OO_Subscript: return OverloadKind::Binary;
		default: return OverloadKind::Unsupported;
	}
}

OOModel::MemberInitializer* CppImportUtilities::translateMemberInit(const clang::CXXCtorInitializer* initializer)
{
	OOModel::MemberInitializer* ooMemberInit = nullptr;
	OOModel::Expression* initExpression = exprVisitor_->translateExpression(initializer->getInit());

	if(initializer->isBaseInitializer())
	{
		if(auto memberRef = DCast<OOModel::ReferenceExpression>
				(translateTypePtr(initializer->getBaseClass(), initializer->getSourceLocation())))
			ooMemberInit = new OOModel::MemberInitializer(memberRef, initExpression);
		else
			log_->writeError(className_, initializer->getLParenLoc(), CppImportLogger::Reason::NOT_SUPPORTED);
	}
	else if(initializer->isMemberInitializer())
		ooMemberInit = new OOModel::MemberInitializer(
					new OOModel::ReferenceExpression(
						QString::fromStdString(initializer->getMember()->getNameAsString())), initExpression);
	else if(initializer->isDelegatingInitializer())
		// we do not need a reference because the initExpression is a method call to another constructor
		ooMemberInit = new OOModel::MemberInitializer(initExpression);

	if(!ooMemberInit)
		log_->writeError(className_, initializer->getLParenLoc(), CppImportLogger::Reason::NOT_SUPPORTED);
	return ooMemberInit;
}

OOModel::Expression* CppImportUtilities::createErrorExpression(const QString& reason)
{
	OOModel::ErrorExpression* ooError = new OOModel::ErrorExpression();
	ooError->setPrefix("#");
	ooError->setArg(new OOModel::StringLiteral(reason));
	return ooError;
}

OOModel::Expression* CppImportUtilities::translateBuiltInClangType(const clang::BuiltinType* type)
{
	const clang::BuiltinType* builtinType = type->getAs<clang::BuiltinType>();
	switch(builtinType->getKind())
	{
		case clang::BuiltinType::Void:
			return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::VOID);
			// unsigned types
		case clang::BuiltinType::Bool:
			return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN);
		case clang::BuiltinType::Char_U:
			log_->primitiveTypeNotSupported("char unsigned target");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::UChar:
			log_->primitiveTypeNotSupported("unsigned char");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::WChar_U:
			log_->primitiveTypeNotSupported("wchar_t unsigned target");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::Char16:
			log_->primitiveTypeNotSupported("char16_t");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::Char32:
			log_->primitiveTypeNotSupported("char32_t");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::UShort:
			log_->primitiveTypeNotSupported("unsigned short");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT);
		case clang::BuiltinType::UInt:
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT);
		case clang::BuiltinType::ULong:
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG);
		case clang::BuiltinType::ULongLong:
			log_->primitiveTypeNotSupported("unsigned long long");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG);
		case clang::BuiltinType::UInt128:
			log_->primitiveTypeNotSupported("__uint128_t");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG);
			// signed types
		case clang::BuiltinType::Char_S:
			return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::SChar:
			log_->primitiveTypeNotSupported("signed char");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::WChar_S:
			log_->primitiveTypeNotSupported("wchar_t");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::Short:
			log_->primitiveTypeNotSupported("short");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::INT);
		case clang::BuiltinType::Int:
			return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::INT);
		case clang::BuiltinType::Long:
			return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::LONG);
		case clang::BuiltinType::LongLong:
			log_->primitiveTypeNotSupported("long long");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::LONG);
		case clang::BuiltinType::Int128:
			log_->primitiveTypeNotSupported("__int128_t");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::LONG);
			// float types
		case clang::BuiltinType::Half:
			log_->primitiveTypeNotSupported("half");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::FLOAT);
		case clang::BuiltinType::Float:
			return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::FLOAT);
		case clang::BuiltinType::Double:
			return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::DOUBLE);
		case clang::BuiltinType::LongDouble:
			log_->primitiveTypeNotSupported("long double");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::DOUBLE);
			// c++ specific
		case clang::BuiltinType::NullPtr:
			log_->primitiveTypeNotSupported("nullptr");
			return createErrorExpression("Unsupported type");
		default:
			log_->primitiveTypeNotSupported("other type ?");
			return createErrorExpression("Unsupported type");
	}
}

OOModel::Expression* CppImportUtilities::translateTypePtr(const clang::Type* type, const clang::SourceLocation& location)
{
	// Note that llvm::dyn_cast is for clang types in fact as fast as static_cast
	// We had a version of this function which was using switch case and static_cast
	// the runtime was the same than this version
	OOModel::Expression* translatedType = nullptr;
	Q_ASSERT(type);
	if(llvm::isa<clang::AutoType>(type))
	{
		translatedType = new OOModel::AutoTypeExpression();
	}
	else if(auto typedefType = llvm::dyn_cast<clang::TypedefType>(type))
	{
		translatedType = new OOModel::ReferenceExpression(
					QString::fromStdString(typedefType->getDecl()->getNameAsString()));
	}
	else if(auto recordType = llvm::dyn_cast<clang::RecordType>(type))
	{
		OOModel::ReferenceExpression* ooRef = new OOModel::ReferenceExpression
				(QString::fromStdString(recordType->getDecl()->getNameAsString()));
		if(auto qualifier = recordType->getDecl()->getQualifier())
			ooRef->setPrefix(translateNestedNameSpecifier(qualifier, location));
		translatedType = ooRef;
	}
	else if(auto pointerType = llvm::dyn_cast<clang::PointerType>(type))
	{
		OOModel::PointerTypeExpression* ooPtr = new OOModel::PointerTypeExpression();
		ooPtr->setTypeExpression(translateQualifiedType(pointerType->getPointeeType(), location));
		translatedType = ooPtr;
	}
	else if(auto refType = llvm::dyn_cast<clang::ReferenceType>(type))
	{
		OOModel::ReferenceTypeExpression* ooRef = new OOModel::ReferenceTypeExpression();
		ooRef->setTypeExpression(translateQualifiedType(refType->getPointeeType(), location));
		translatedType = ooRef;
	}
	else if(auto enumType = llvm::dyn_cast<clang::EnumType>(type))
	{
		OOModel::ReferenceExpression* ooRef = new OOModel::ReferenceExpression
				(QString::fromStdString(enumType->getDecl()->getNameAsString()));
		if(auto qualifier = enumType->getDecl()->getQualifier())
			ooRef->setPrefix(translateNestedNameSpecifier(qualifier, location));
		translatedType = ooRef;
	}
	else if(auto constArrayType = llvm::dyn_cast<clang::ConstantArrayType>(type))
	{
		OOModel::ArrayTypeExpression* ooArrayType = new OOModel::ArrayTypeExpression();
		ooArrayType->setTypeExpression(translateQualifiedType(constArrayType->getElementType(), location));
		ooArrayType->setFixedSize(new OOModel::IntegerLiteral(constArrayType->getSize().getLimitedValue()));
		translatedType = ooArrayType;
	}
	else if(auto incompleteArrayType = llvm::dyn_cast<clang::IncompleteArrayType>(type))
	{
		OOModel::ArrayTypeExpression* ooArrayType = new OOModel::ArrayTypeExpression();
		ooArrayType->setTypeExpression(translateQualifiedType(incompleteArrayType->getElementType(), location));
		translatedType = ooArrayType;
	}
	else if(auto parenType = llvm::dyn_cast<clang::ParenType>(type))
	{
		// TODO: this might not always be a nice solution, to just return the inner type of a parenthesized type.
		translatedType = translateQualifiedType(parenType->getInnerType(), location);
	}
	else if(auto typeDefType = llvm::dyn_cast<clang::TypedefType>(type))
	{
		translatedType = new OOModel::ReferenceExpression
				(QString::fromStdString(typeDefType->getDecl()->getNameAsString()));
	}
	else if(auto templateParmType = llvm::dyn_cast<clang::TemplateTypeParmType>(type))
	{
		translatedType = new OOModel::ReferenceExpression(
					QString::fromStdString(templateParmType->getDecl()->getNameAsString()));
	}
	else if(auto functionProtoType = llvm::dyn_cast<clang::FunctionProtoType>(type))
	{
		// TODO: include templates. (and more?)
		OOModel::FunctionTypeExpression* ooFunctionType = new OOModel::FunctionTypeExpression();
		ooFunctionType->results()->append(translateQualifiedType(functionProtoType->getResultType(), location));
		for(auto argIt = functionProtoType->arg_type_begin(); argIt != functionProtoType->arg_type_end(); ++argIt)
		{
			ooFunctionType->arguments()->append(translateQualifiedType(*argIt, location));
		}
		translatedType = ooFunctionType;
	}
	else if(auto elaboratedType = llvm::dyn_cast<clang::ElaboratedType>(type))
	{
		// TODO: this might also have a keyword in front (like e.g. class, typename)
		translatedType = translateQualifiedType(elaboratedType->getNamedType(), location);
		if(auto qualifier = elaboratedType->getQualifier())
			if(auto ooRef = DCast<OOModel::ReferenceExpression>(translatedType))
				ooRef->setPrefix(translateNestedNameSpecifier(qualifier, location));
	}
	else if(auto templateSpecialization = llvm::dyn_cast<clang::TemplateSpecializationType>(type))
	{
		OOModel::ReferenceExpression* ooRef = new OOModel::ReferenceExpression();
		ooRef->setName(QString::fromStdString(
								templateSpecialization->getTemplateName().getAsTemplateDecl()->getNameAsString()));
		for(auto argIt = templateSpecialization->begin(); argIt != templateSpecialization->end(); ++argIt)
			ooRef->typeArguments()->append(translateTemplateArgument(*argIt, location));
		translatedType = ooRef;
	}
	else if(auto dependentType = llvm::dyn_cast<clang::DependentNameType>(type))
	{
		OOModel::ReferenceExpression* ooRef = new OOModel::ReferenceExpression
				(QString(dependentType->getIdentifier()->getNameStart()));
		if(auto qualifier = dependentType->getQualifier())
			ooRef->setPrefix(translateNestedNameSpecifier(qualifier, location));
		if(dependentType->getKeyword() == clang::ETK_Typename)
		{
			OOModel::TypeNameOperator* ooTypeName = new OOModel::TypeNameOperator();
			ooTypeName->setTypeExpression(ooRef);
			return ooTypeName;
		}
		return ooRef;
	}
	else if(auto injectedClass = llvm::dyn_cast<clang::InjectedClassNameType>(type))
	{
		translatedType = translateTypePtr(injectedClass->getInjectedTST(), location);
	}
	else if(auto substTemplateParm = llvm::dyn_cast<clang::SubstTemplateTypeParmType>(type))
	{
		translatedType = translateQualifiedType(substTemplateParm->getReplacementType(), location);
	}
	else if(auto builtIn = llvm::dyn_cast<clang::BuiltinType>(type))
	{
		translatedType = translateBuiltInClangType(builtIn);
	}
	else
	{
		log_->typeNotSupported(type, location);
		translatedType = createErrorExpression("Unsupported Type");
	}
	return translatedType;
}

} /* namespace CppImport */
