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

namespace CppImport {


CppImportUtilities::CppImportUtilities(CppImportLogger* logger) : log_(logger)
{
}

OOModel::Expression* CppImportUtilities::convertClangType(clang::QualType qualType)
{
	const clang::Type* type = qualType.getTypePtr();
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
			translatedType = /*new OOModel::ClassTypeExpression(*/
						new OOModel::ReferenceExpression(QString::fromStdString(recordType->getDecl()->getNameAsString())/*)*/);
	}
	else if(auto pointerType = llvm::dyn_cast<clang::PointerType>(type))
	{
		OOModel::PointerTypeExpression* ooPtr = new OOModel::PointerTypeExpression();
		ooPtr->setTypeExpression(convertClangType(pointerType->getPointeeType()));
		translatedType = ooPtr;
	}
	else if(auto refType = llvm::dyn_cast<clang::ReferenceType>(type))
	{
		OOModel::ReferenceTypeExpression* ooRef = new OOModel::ReferenceTypeExpression();
		ooRef->setTypeExpression(convertClangType(refType->getPointeeType()));
		translatedType = ooRef;
	}
	else if(auto enumType = llvm::dyn_cast<clang::EnumType>(type))
	{
		OOModel::ReferenceExpression* ooRef = new OOModel::ReferenceExpression
				(QString::fromStdString(enumType->getDecl()->getNameAsString()));
		translatedType = ooRef;
	}
	else if(auto constArrayType = llvm::dyn_cast<clang::ConstantArrayType>(type))
	{
		OOModel::ArrayTypeExpression* ooArrayType = new OOModel::ArrayTypeExpression();
		ooArrayType->setTypeExpression(convertClangType(constArrayType->getElementType()));
		ooArrayType->setFixedSize(new OOModel::IntegerLiteral(constArrayType->getSize().getLimitedValue()));
		translatedType = ooArrayType;
	}
	else if(auto incompleteArrayType = llvm::dyn_cast<clang::IncompleteArrayType>(type))
	{
		OOModel::ArrayTypeExpression* ooArrayType = new OOModel::ArrayTypeExpression();
		ooArrayType->setTypeExpression(convertClangType(incompleteArrayType->getElementType()));
		translatedType = ooArrayType;
	}
	else if(auto parenType = llvm::dyn_cast<clang::ParenType>(type))
	{
		// TODO: this might not always be a nice solution
		// to just return the inner type of a parenthesized type
		translatedType = convertClangType(parenType->getInnerType());
	}
	else if(auto typeDefType = llvm::dyn_cast<clang::TypedefType>(type))
	{
		translatedType = new OOModel::ReferenceExpression(QString::fromStdString(typeDefType->getDecl()->getNameAsString()));
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
		ooFunctionType->results()->append(convertClangType(functionProtoType->getResultType()));
		for(auto argIt = functionProtoType->arg_type_begin(); argIt != functionProtoType->arg_type_end(); ++argIt)
		{
			ooFunctionType->arguments()->append(convertClangType(*argIt));
		}
		translatedType = ooFunctionType;
	}
	else if(auto elaboratedType = llvm::dyn_cast<clang::ElaboratedType>(type))
	{
		// TODO: handle this correctly
		translatedType = convertClangType(elaboratedType->getNamedType());
	}
	else if(auto templateSpecialization = llvm::dyn_cast<clang::TemplateSpecializationType>(type))
	{
		OOModel::ReferenceExpression* ooRef = new OOModel::ReferenceExpression();
		ooRef->setName(QString::fromStdString(
								templateSpecialization->getTemplateName().getAsTemplateDecl()->getNameAsString()));
		for(auto argIt = templateSpecialization->begin(); argIt != templateSpecialization->end(); ++argIt)
			ooRef->typeArguments()->append(convertTemplateArgument(*argIt));
		translatedType = ooRef;
	}
	else if(auto dependentType = llvm::dyn_cast<clang::DependentNameType>(type))
	{
		OOModel::ReferenceExpression* ooRef = new OOModel::ReferenceExpression
				(QString(dependentType->getIdentifier()->getNameStart()));
		if(dependentType->getKeyword() == clang::ETK_Typename)
		{
			OOModel::TypeNameOperator* ooTypeName = new OOModel::TypeNameOperator();
			ooTypeName->setTypeExpression(ooRef);
			return ooTypeName;
		}
		return ooRef;
	}
	else if(type->isBuiltinType())
	{
		translatedType = convertBuiltInClangType(type);
	}
	else
	{
		log_->typeNotSupported(QString(type->getTypeClassName()));
		translatedType = createErrorExpression("Unsupported Type");
	}

	if(qualType.isConstQualified() && qualType.isVolatileQualified())
		return new OOModel::TypeQualifierExpression(OOModel::Type::CONST, new OOModel::TypeQualifierExpression
																  (OOModel::Type::VOLATILE, translatedType));
	else if(qualType.isConstQualified())
		return new OOModel::TypeQualifierExpression(OOModel::Type::CONST, translatedType);
	else if(qualType.isVolatileQualified())
		return new OOModel::TypeQualifierExpression(OOModel::Type::VOLATILE, translatedType);

	return translatedType;
}

OOModel::BinaryOperation::OperatorTypes CppImportUtilities::convertClangOpcode
	(clang::BinaryOperatorKind kind)
{
	switch (kind)
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
		default: throw(new CppImportException("Impossible binary operator"));
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
		default: throw(new CppImportException("Impossible assign operator"));
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
		case clang::UO_Not: return OOModel::UnaryOperation::COMPLEMENT;
		case clang::UO_LNot: return OOModel::UnaryOperation::NOT;
		default: throw(new CppImportException("Unary operation not supported"));
	}
}

OOModel::Modifier::ModifierFlag CppImportUtilities::convertAccessSpecifier(clang::AccessSpecifier as)
{
	switch (as)
	{
		case clang::AS_public: return OOModel::Modifier::Public;
		case clang::AS_protected: return OOModel::Modifier::Protected;
		case clang::AS_private: return OOModel::Modifier::Private;
		case clang::AS_none: return OOModel::Modifier::None;
		default: return OOModel::Modifier::None;
	}
}

OOModel::Modifier::ModifierFlag CppImportUtilities::convertStorageSpecifier(clang::StorageClass storage)
{
	switch(storage)
	{
		case clang::SC_None: return OOModel::Modifier::None;
		case clang::SC_Static: return OOModel::Modifier::Static;
		default:
			log_->storageClassNotSupported(storage);
			return OOModel::Modifier::None;
	}
}

OOModel::Expression*CppImportUtilities::translateNestedNameSpecifier(clang::NestedNameSpecifier* nestedName)
{
	OOModel::ReferenceExpression* currentRef = nullptr;
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
			// TODO
//			if(auto typeRef = dynamic_cast<OOModel::ReferenceExpression*>(convertClangType(nestedName->getAsType()->)))
			Q_ASSERT(0);
			break;
		case clang::NestedNameSpecifier::TypeSpecWithTemplate:
			// TODO
			Q_ASSERT(0);
			break;
		case clang::NestedNameSpecifier::Global:
			Q_ASSERT(!nestedName->getPrefix());
			return new OOModel::GlobalScopeExpression();
	}
	if(auto prefix = nestedName->getPrefix())
		currentRef->setPrefix(translateNestedNameSpecifier(prefix));
	return currentRef;
}

OOModel::Expression*CppImportUtilities::convertTemplateArgument(const clang::TemplateArgument& templateArg)
{
	switch(templateArg.getKind())
	{
		case clang::TemplateArgument::ArgKind::Null:
			std::cout << "NULL #####################################"<<std::endl;
			return createErrorExpression("Unsupported TemplateArgument NULL");
		case clang::TemplateArgument::ArgKind::Type:
			return convertClangType(templateArg.getAsType());
		case clang::TemplateArgument::ArgKind::Declaration:
			return new OOModel::ReferenceExpression(QString::fromStdString(templateArg.getAsDecl()->getNameAsString()));
		case clang::TemplateArgument::ArgKind::NullPtr:
			std::cout << "NULLPTR #####################################"<<std::endl;
			return createErrorExpression("Unsupported TemplateArgument NULLPTR");
		case clang::TemplateArgument::ArgKind::Integral:
			std::cout << "INTEGRAL #####################################"<<std::endl;
			return createErrorExpression("Unsupported TemplateArgument INTEGRAL");
		case clang::TemplateArgument::ArgKind::Template:
			return new OOModel::ReferenceExpression(
						QString::fromStdString(templateArg.getAsTemplate().getAsTemplateDecl()->getNameAsString()));
		case clang::TemplateArgument::ArgKind::TemplateExpansion:
			std::cout << "EXPANSION #####################################"<<std::endl;
			return createErrorExpression("Unsupported TemplateArgument EXPANSION");
		case clang::TemplateArgument::ArgKind::Expression:
			std::cout << "EXPPRESSION #####################################"<<std::endl;
			return createErrorExpression("Unsupported TemplateArgument EXPRESSION");
		case clang::TemplateArgument::ArgKind::Pack:
			std::cout << "PACK #####################################"<<std::endl;
			return createErrorExpression("Unsupported TemplateArgument PACK");
		default:
			std::cout << "####### " << static_cast<int>(templateArg.getKind()) << std::endl;
			return createErrorExpression("Unsupported TemplateArgumentLoc");
	}
}

OOModel::BinaryOperation::OperatorTypes CppImportUtilities::translateBinaryOverloadOp(clang::OverloadedOperatorKind kind)
{
	switch(kind)
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
(clang::OverloadedOperatorKind kind)
{
	switch(kind)
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
(clang::OverloadedOperatorKind kind, unsigned numArgs)
{
	switch(kind)
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
(clang::OverloadedOperatorKind kind, unsigned numArgs)
{
	switch(kind)
	{
		//		case clang::OO_New: return OverloadKind::;
		//		case clang::OO_Delete: return OverloadKind::;
		//		case clang::OO_Array_New: return OverloadKind::;
		//		case clang::OO_Array_Delete: return OverloadKind::;
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

OOModel::Expression*CppImportUtilities::convertBuiltInClangType(const clang::Type* type)
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
			log_->typeNotSupported("char unsigned target");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::UChar:
			log_->typeNotSupported("unsigned char");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::WChar_U:
			log_->typeNotSupported("wchar_t unsigned target");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::Char16:
			log_->typeNotSupported("char16_t");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::Char32:
			log_->typeNotSupported("char32_t");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::UShort:
			log_->typeNotSupported("unsigned short");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT);
		case clang::BuiltinType::UInt:
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT);
		case clang::BuiltinType::ULong:
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG);
		case clang::BuiltinType::ULongLong:
			log_->typeNotSupported("unsigned long long");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG);
		case clang::BuiltinType::UInt128:
			log_->typeNotSupported("__uint128_t");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG);
		// signed types
		case clang::BuiltinType::Char_S:
			return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::SChar:
			log_->typeNotSupported("signed char");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::WChar_S:
			log_->typeNotSupported("wchar_t");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::Short:
			log_->typeNotSupported("short");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::INT);
		case clang::BuiltinType::Int:
			return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::INT);
		case clang::BuiltinType::Long:
			return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::LONG);
		case clang::BuiltinType::LongLong:
			log_->typeNotSupported("long long");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::LONG);
		case clang::BuiltinType::Int128:
			log_->typeNotSupported("__int128_t");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::LONG);
		// float types
		case clang::BuiltinType::Half:
			log_->typeNotSupported("half");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::FLOAT);
		case clang::BuiltinType::Float:
			return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::FLOAT);
		case clang::BuiltinType::Double:
			return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::DOUBLE);
		case clang::BuiltinType::LongDouble:
			log_->typeNotSupported("long double");
			return new OOModel::PrimitiveTypeExpression(
						OOModel::PrimitiveTypeExpression::PrimitiveTypes::DOUBLE);
		// c++ specific
		case clang::BuiltinType::NullPtr:
			log_->typeNotSupported("nullptr");
			return createErrorExpression("Unsupported type");
		default:
			log_->typeNotSupported("other type ?");
			return createErrorExpression("Unsupported type");
	}
}

OOModel::Expression* CppImportUtilities::createErrorExpression(QString reason)
{
	OOModel::ErrorExpression* ooError = new OOModel::ErrorExpression();
	ooError->setPrefix("#");
	ooError->setArg(new OOModel::StringLiteral(reason));
	return ooError;
}

} /* namespace CppImport */
