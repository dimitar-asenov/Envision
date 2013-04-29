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

namespace CppImport {

OOModel::Expression *CppImportUtilities::convertClangType(clang::QualType type)
{
	if(type.getTypePtr()->isBuiltinType())
	{
		const clang::BuiltinType* builtinType = type.getTypePtr()->getAs<clang::BuiltinType>();
		switch(builtinType->getKind())
		{
			case clang::BuiltinType::Void:
				return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::VOID);
			// unsigned types
			case clang::BuiltinType::Bool:
				return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN);
			case clang::BuiltinType::Char_U:
				return nullptr;
			case clang::BuiltinType::UChar:
				return nullptr;
			case clang::BuiltinType::WChar_U:
				return nullptr;
			case clang::BuiltinType::Char16:
				return nullptr;
			case clang::BuiltinType::Char32:
				return nullptr;
			case clang::BuiltinType::UShort:
				return nullptr;
			case clang::BuiltinType::UInt:
				return new OOModel::PrimitiveTypeExpression(
							OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT);
			case clang::BuiltinType::ULong:
				return new OOModel::PrimitiveTypeExpression(
							OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG);
			case clang::BuiltinType::ULongLong:
				return nullptr;
			case clang::BuiltinType::UInt128:
				return nullptr;
			// signed types
			case clang::BuiltinType::Char_S:
				return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
			case clang::BuiltinType::SChar:
				return nullptr;
			case clang::BuiltinType::WChar_S:
				return nullptr;
			case clang::BuiltinType::Short:
				return nullptr;
			case clang::BuiltinType::Int:
				return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::INT);
			case clang::BuiltinType::Long:
				return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::LONG);
			case clang::BuiltinType::LongLong:
				return nullptr;
			case clang::BuiltinType::Int128:
				return nullptr;
			// float types
			case clang::BuiltinType::Half:
				return nullptr;
			case clang::BuiltinType::Float:
				return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::FLOAT);
			case clang::BuiltinType::Double:
				return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::DOUBLE);
			case clang::BuiltinType::LongDouble:
				return nullptr;
			// c++ specific
			case clang::BuiltinType::NullPtr:
				return nullptr;
			default:
				return nullptr;
		}
	}
	else if(type.getTypePtr()->isRecordType())
	{
		if(clang::CXXRecordDecl* recordDecl = type.getTypePtr()->getAsCXXRecordDecl())
			return new OOModel::ClassTypeExpression(new OOModel::ReferenceExpression(
																	 QString::fromStdString(recordDecl->getNameAsString())));
		return nullptr;
	}
	else if(type.getTypePtr()->isPointerType())
	{
		OOModel::PointerTypeExpression* ooPtr = new OOModel::PointerTypeExpression();
		ooPtr->setTypeExpression(convertClangType(type.getTypePtr()->getPointeeType()));
		return ooPtr;
	}
	else if(type.getTypePtr()->isReferenceType())
	{
		OOModel::ReferenceTypeExpression* ooRef = new OOModel::ReferenceTypeExpression();
		ooRef->setTypeExpression(convertClangType(type.getTypePtr()->getPointeeType()));
		return ooRef;
	}
	else
		return nullptr;
}

OOModel::BinaryOperation::OperatorTypes CppImportUtilities::convertClangOpcode
	(clang::BinaryOperatorKind kind)
{
	switch (kind)
	{
		case clang::BO_Mul: return OOModel::BinaryOperation::TIMES;
		case clang::BO_Div: return OOModel::BinaryOperation::DIVIDE;
		case clang::BO_Rem: return OOModel::BinaryOperation::REMAINDER;
		case clang::BO_Add: return OOModel::BinaryOperation::PLUS;
		case clang::BO_Sub: return OOModel::BinaryOperation::MINUS;
		case clang::BO_LT: return OOModel::BinaryOperation::LESS;
		case clang::BO_GT: return OOModel::BinaryOperation::GREATER;
		case clang::BO_LE: return OOModel::BinaryOperation::LESS_EQUALS;
		case clang::BO_GE: return OOModel::BinaryOperation::GREATER_EQUALS;
		case clang::BO_EQ: return OOModel::BinaryOperation::EQUALS;
		case clang::BO_NE: return OOModel::BinaryOperation::NOT_EQUALS;
		case clang::BO_And: return OOModel::BinaryOperation::AND;
		case clang::BO_Xor: return OOModel::BinaryOperation::XOR;
		case clang::BO_Or: return OOModel::BinaryOperation::OR;
		case clang::BO_LAnd: return OOModel::BinaryOperation::AND;
		case clang::BO_LOr: return OOModel::BinaryOperation::CONDITIONAL_OR;
		default: return OOModel::BinaryOperation::GREATER;
	}
}

OOModel::AssignmentExpression::AssignmentTypes CppImportUtilities::convertClangAssignOpcode
	(clang::BinaryOperatorKind kind)
{
	switch (kind)
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
		default: return OOModel::AssignmentExpression::ASSIGN;
	}
}

OOModel::UnaryOperation::OperatorTypes CppImportUtilities::convertUnaryOpcode(clang::UnaryOperatorKind kind)
{
	switch (kind)
	{
		case clang::UO_PostInc: return OOModel::UnaryOperation::POSTINCREMENT;
		case clang::UO_PostDec: return OOModel::UnaryOperation::POSTDECREMENT;
		case clang::UO_PreInc: return OOModel::UnaryOperation::PREINCREMENT;
		case clang::UO_PreDec: return OOModel::UnaryOperation::PREDECREMENT;
		case clang::UO_AddrOf: return OOModel::UnaryOperation::ADDRESSOF;
		case clang::UO_Deref: return OOModel::UnaryOperation::DEREFERENCE;
		case clang::UO_Plus: return OOModel::UnaryOperation::PLUS;
		case clang::UO_Minus: return OOModel::UnaryOperation::MINUS;
		case clang::UO_Not: return OOModel::UnaryOperation::NOT;
			//    case clang::UO_LNot: return OOModel::UnaryOperation:
			//    case clang::UO_Real: return OOModel::UnaryOperation:
			//    case clang::UO_Imag: return OOModel::UnaryOperation:
			//    case clang::UO_Extension: return OOModel::UnaryOperation:
		default: return OOModel::UnaryOperation::POSTDECREMENT;
	}
}

OOModel::Visibility::VisibilityType CppImportUtilities::convertAccessSpecifier(clang::AccessSpecifier as)
{
	switch (as)
	{
		case clang::AS_public: return OOModel::Visibility::PUBLIC;
		case clang::AS_protected: return OOModel::Visibility::PROTECTED;
		case clang::AS_private: return OOModel::Visibility::PRIVATE;
		case clang::AS_none: return OOModel::Visibility::DEFAULT;
		default: return OOModel::Visibility::DEFAULT;
	}
}

} /* namespace CppImport */
