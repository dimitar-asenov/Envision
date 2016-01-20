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

#include "CppImportUtilities.h"
#include "CppImportException.h"
#include "visitors/ExpressionVisitor.h"
#include "visitors/ClangAstVisitor.h"

namespace CppImport {


CppImportUtilities::CppImportUtilities(CppImportLogger* logger, ExpressionVisitor* visitor, ClangHelpers& clang)
	: log_{logger}, exprVisitor_{visitor}, clang_{clang}
{}

OOModel::Expression* CppImportUtilities::translateQualifiedType(clang::TypeLoc typeLoc)
{
	if (auto qualifiedTypeLoc = typeLoc.getAs<clang::QualifiedTypeLoc>())
	{
		auto translatedTypeExpression = translateTypePtr(qualifiedTypeLoc.getUnqualifiedLoc());

		auto qualifiedType = qualifiedTypeLoc.getType();
		if (qualifiedType.isConstQualified() && qualifiedType.isVolatileQualified())
		{
			auto volatileTypeExpression = clang_.createNode<OOModel::TypeQualifierExpression>(typeLoc.getSourceRange(),
																														 OOModel::Type::VOLATILE,
																														 translatedTypeExpression);
			return clang_.createNode<OOModel::TypeQualifierExpression>(typeLoc.getSourceRange(), OOModel::Type::CONST,
																						  volatileTypeExpression);
		}
		else if (qualifiedType.isConstQualified())
			return clang_.createNode<OOModel::TypeQualifierExpression>(typeLoc.getSourceRange(), OOModel::Type::CONST,
																						  translatedTypeExpression);
		else if (qualifiedType.isVolatileQualified())
			return clang_.createNode<OOModel::TypeQualifierExpression>(typeLoc.getSourceRange(), OOModel::Type::VOLATILE,
																						  translatedTypeExpression);

		return translatedTypeExpression;
	}

	return translateTypePtr(typeLoc);
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
	switch (storageClass)
	{
		case clang::SC_None: return OOModel::Modifier::None;
		case clang::SC_Static: return OOModel::Modifier::Static;
		default:
			log_->storageClassNotSupported(storageClass);
			return OOModel::Modifier::None;
	}
}

OOModel::ReferenceExpression* CppImportUtilities::setReferencePrefix(OOModel::ReferenceExpression* reference,
																						clang::NestedNameSpecifierLoc nestedNameSpecifierLoc,
																						clang::Expr* base)
{
	Q_ASSERT(reference);
	if (nestedNameSpecifierLoc)
	{
		Q_ASSERT(!base); // just testing whether this can ever happen, remove if seen but never occured
		reference->setPrefix(translateNestedNameSpecifier(nestedNameSpecifierLoc, base));
		reference->setMemberKind(OOModel::ReferenceExpression::MemberKind::Static);
	}
	else if (base)
	{
		reference->setPrefix(exprVisitor_->translateExpression(base));
		if (base->getType().getTypePtr()->isPointerType())
			reference->setMemberKind(OOModel::ReferenceExpression::MemberKind::Pointer);
	}
	return reference;
}

OOModel::Expression* CppImportUtilities::translateNestedNameSpecifier
(const clang::NestedNameSpecifierLoc nestedNameLoc, clang::Expr* base)
{
	OOModel::ReferenceExpression* currentRef = nullptr;
	switch (nestedNameLoc.getNestedNameSpecifier()->getKind())
	{
		case clang::NestedNameSpecifier::Identifier:
		case clang::NestedNameSpecifier::Namespace:
		case clang::NestedNameSpecifier::NamespaceAlias:
			currentRef = clang_.createReference(nestedNameLoc.getLocalSourceRange());
			break;
		case clang::NestedNameSpecifier::TypeSpec:
			// TODO: handle the case where this cast is not a reference
			if (auto typeRef = DCast<OOModel::ReferenceExpression>(translateTypePtr(nestedNameLoc.getTypeLoc())))
				currentRef = typeRef;
			else
				return createErrorExpression("TypeSpecNameSpecifier", nestedNameLoc.getLocalSourceRange());
			break;
		case clang::NestedNameSpecifier::TypeSpecWithTemplate:
			// TODO: handle the case where this cast is not a reference
			if (auto typeRef = DCast<OOModel::ReferenceExpression>(translateTypePtr(nestedNameLoc.getTypeLoc())))
				currentRef = typeRef;
			else
				return createErrorExpression("Could not translate TypeSpecWithTemplate",
													  nestedNameLoc.getLocalSourceRange());
			break;
		case clang::NestedNameSpecifier::Global:
			// if we have the Global specifier there can not be a prefix() other wise it is invalid C++
			Q_ASSERT(!nestedNameLoc.getPrefix());
			return clang_.createNode<OOModel::GlobalScopeExpression>(nestedNameLoc.getSourceRange());
		default:
			// In version 3.6 this is only NestedNameSpecifier::Super, which is a Microsoft specific extension (_super).
			throw new CppImportException{QString("Unsupported nested name specifier kind: %1")
												  .arg(nestedNameLoc.getNestedNameSpecifier()->getKind())};
	}
	return setReferencePrefix(currentRef, nestedNameLoc.getPrefix(), base);
}

OOModel::Expression* CppImportUtilities::translateTemplateArgument(const clang::TemplateArgumentLoc& templateArgLoc)
{
	auto templateArg = templateArgLoc.getArgument();
	if (templateArg.getKind() == clang::TemplateArgument::ArgKind::Expression)
		return exprVisitor_->translateExpression(templateArg.getAsExpr());

	auto sourceRange = templateArgLoc.getTypeSourceInfo()->getTypeLoc().getSourceRange();
	switch (templateArg.getKind())
	{
		case clang::TemplateArgument::ArgKind::Null:
			return clang_.createNode<OOModel::EmptyExpression>(sourceRange);
		case clang::TemplateArgument::ArgKind::Type:
			return translateQualifiedType(templateArgLoc.getTypeSourceInfo()->getTypeLoc());
		case clang::TemplateArgument::ArgKind::Declaration:
		case clang::TemplateArgument::ArgKind::Template:
			return clang_.createReference(sourceRange);
		case clang::TemplateArgument::ArgKind::NullPtr:
			return clang_.createNode<OOModel::NullLiteral>(sourceRange);
		case clang::TemplateArgument::ArgKind::Integral:
			return clang_.createNode<OOModel::IntegerLiteral>(sourceRange,
																			  (int)templateArg.getAsIntegral().getLimitedValue());
		case clang::TemplateArgument::ArgKind::TemplateExpansion:
			// TODO: add support
			return createErrorExpression("Unsupported TemplateArgument EXPANSION", sourceRange);
		case clang::TemplateArgument::ArgKind::Pack:
			// TODO: add support
			return createErrorExpression("Unsupported TemplateArgument PACK", sourceRange);
		default:
			throw CppImportException("Invalid Template Argument kind");
	}
}

OOModel::BinaryOperation::OperatorTypes CppImportUtilities::translateBinaryOverloadOp
(const clang::OverloadedOperatorKind& overloadOpKind)
{
	switch (overloadOpKind)
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
		default: throw new CppImportException{"Invalid binary overload operator"};
	}
}

QString CppImportUtilities::overloadOperatorToString(const clang::OverloadedOperatorKind& overloadOpKind)
{
	switch (overloadOpKind)
	{
		case clang::OO_New: return "new";
		case clang::OO_Delete: return "delete";
		case clang::OO_Array_New: return "new[]";
		case clang::OO_Array_Delete: return "delete[]";
		case clang::OO_Plus: return "+";
		case clang::OO_Minus: return "-";
		case clang::OO_Star: return "*";
		case clang::OO_Slash: return "/";
		case clang::OO_Percent: return "%";
		case clang::OO_Caret: return "^";
		case clang::OO_Amp: return "&";
		case clang::OO_Pipe: return "|";
		case clang::OO_Tilde: return "~";
		case clang::OO_Exclaim: return "!";
		case clang::OO_Equal: return "=";
		case clang::OO_Less: return "<";
		case clang::OO_Greater: return ">";
		case clang::OO_PlusEqual: return "+=";
		case clang::OO_MinusEqual: return "-=";
		case clang::OO_StarEqual: return "*=";
		case clang::OO_SlashEqual: return "/=";
		case clang::OO_PercentEqual: return "%=";
		case clang::OO_CaretEqual: return "^=";
		case clang::OO_AmpEqual: return "&=";
		case clang::OO_PipeEqual: return "|=";
		case clang::OO_LessLess: return "<<";
		case clang::OO_GreaterGreater: return ">>";
		case clang::OO_LessLessEqual: return "<<=";
		case clang::OO_GreaterGreaterEqual: return ">>=";
		case clang::OO_EqualEqual: return "==";
		case clang::OO_ExclaimEqual: return "!=";
		case clang::OO_LessEqual: return "<=";
		case clang::OO_GreaterEqual: return ">=";
		case clang::OO_AmpAmp: return "&&";
		case clang::OO_PipePipe: return "||";
		case clang::OO_PlusPlus: return "++";
		case clang::OO_MinusMinus: return "--";
		case clang::OO_Comma: return ",";
		case clang::OO_ArrowStar: return "->*";
		case clang::OO_Arrow: return "->";
		case clang::OO_Call: return "()";
		case clang::OO_Subscript: return "[]";
		case clang::OO_Conditional: return "?";
		default: throw new CppImportException{"Invalid overload operator"};
	}
}

OOModel::AssignmentExpression::AssignmentTypes CppImportUtilities::translateAssignOverloadOp
(const clang::OverloadedOperatorKind& overloadOpKind)
{
	switch (overloadOpKind)
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
		default: throw new CppImportException{"Invalid assign overload operator"};
	}
}

OOModel::UnaryOperation::OperatorTypes CppImportUtilities::translateUnaryOverloadOp
(const clang::OverloadedOperatorKind& overloadOpKind, const unsigned numArgs)
{
	switch (overloadOpKind)
	{
		case clang::OO_Plus: return OOModel::UnaryOperation::PLUS;
		case clang::OO_Minus: return OOModel::UnaryOperation::MINUS;
		case clang::OO_Star: return OOModel::UnaryOperation::DEREFERENCE;
		case clang::OO_Amp: return OOModel::UnaryOperation::ADDRESSOF;
		case clang::OO_Tilde: return OOModel::UnaryOperation::COMPLEMENT;
		case clang::OO_Exclaim: return OOModel::UnaryOperation::NOT;
		case clang::OO_PlusPlus: if (1 == numArgs) return OOModel::UnaryOperation::PREINCREMENT;
			return OOModel::UnaryOperation::POSTINCREMENT;
		case clang::OO_MinusMinus: if (1 == numArgs) return OOModel::UnaryOperation::PREDECREMENT;
			return OOModel::UnaryOperation::POSTDECREMENT;
		default: throw new CppImportException{"Invalid unary overload operator"};
	}
}

CppImportUtilities::OverloadKind CppImportUtilities::getOverloadKind
(const clang::OverloadedOperatorKind& overloadOpKind, unsigned numArgs)
{
	switch (overloadOpKind)
	{
		// The first four cases are not supported therefore they fallthrough
		case clang::OO_New:
		case clang::OO_Delete:
		case clang::OO_Array_New:
		case clang::OO_Array_Delete: return OverloadKind::Unsupported;
		case clang::OO_Plus: if (1 == numArgs) return OverloadKind::Unary;
			return OverloadKind::Binary;
		case clang::OO_Minus: if (1 == numArgs) return OverloadKind::Unary;
			return OverloadKind::Binary;
		case clang::OO_Star: if (1 == numArgs) return OverloadKind::Unary;
			return OverloadKind::Binary;
		case clang::OO_Slash: return OverloadKind::Binary;
		case clang::OO_Percent: return OverloadKind::Binary;
		case clang::OO_Caret: return OverloadKind::Binary;
		case clang::OO_Amp: if (1 == numArgs) return OverloadKind::Unary;
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
	auto ooMemberInit = clang_.createNode<OOModel::MemberInitializer>(initializer->getSourceRange());
	if (auto constructExpr = llvm::dyn_cast<clang::CXXConstructExpr>(initializer->getInit()->IgnoreImplicit()))
		for (auto argument : exprVisitor_->translateArguments(constructExpr->getArgs(), constructExpr->getNumArgs()))
			ooMemberInit->arguments()->append(argument);
	else
	{
		if (auto initListExpr = llvm::dyn_cast<clang::InitListExpr>(initializer->getInit()->IgnoreImplicit()))
			for (auto argument : *initListExpr)
				ooMemberInit->arguments()->append(exprVisitor_->translateExpression(argument));
		else
			ooMemberInit->arguments()->append(exprVisitor_->translateExpression(initializer->getInit()->IgnoreImplicit()));
	}
	if (initializer->isBaseInitializer())
	{
		if (auto memberRef = DCast<OOModel::ReferenceExpression>(
				 translateTypePtr(initializer->getTypeSourceInfo()->getTypeLoc())))
			ooMemberInit->setMemberReference(memberRef);
		else
			log_->writeError(className_, initializer->getLParenLoc(), CppImportLogger::Reason::NOT_SUPPORTED);
	}
	else if (initializer->isMemberInitializer())
		ooMemberInit->setMemberReference(clang_.createReference(initializer->getMemberLocation()));
	else if (initializer->isDelegatingInitializer())
		ooMemberInit->setMemberReference(clang_.createReference(initializer->getTypeSourceInfo()
																											  ->getTypeLoc().getSourceRange()));
	else
		log_->writeError(className_, initializer->getLParenLoc(), CppImportLogger::Reason::NOT_SUPPORTED);

	return ooMemberInit;
}

OOModel::Expression* CppImportUtilities::createErrorExpression(const QString& reason, clang::SourceRange sourceRange)
{
	log_->writeError(className_, sourceRange.getBegin(), CppImportLogger::Reason::OTHER, reason);

	auto ooError = clang_.createNode<OOModel::ErrorExpression>(sourceRange);
	ooError->setPrefix("#");
	auto arg = clang_.createNode<OOModel::StringLiteral>(sourceRange, reason);
	ooError->setArg(arg);
	return ooError;
}

OOModel::Expression* CppImportUtilities::translateBuiltInClangType(const clang::BuiltinTypeLoc typeLoc)
{
	switch (typeLoc.getTypePtr()->castAs<clang::BuiltinType>()->getKind())
	{
		case clang::BuiltinType::Void:
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::VOID);
		case clang::BuiltinType::Bool:
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN);
		case clang::BuiltinType::Char_U:
			log_->primitiveTypeNotSupported("char unsigned target");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::UChar:
			log_->primitiveTypeNotSupported("unsigned char");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::WChar_U:
			log_->primitiveTypeNotSupported("wchar_t unsigned target");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::Char16:
			log_->primitiveTypeNotSupported("char16_t");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::Char32:
			log_->primitiveTypeNotSupported("char32_t");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::UShort:
			log_->primitiveTypeNotSupported("unsigned short");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT);
		case clang::BuiltinType::UInt:
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT);
		case clang::BuiltinType::ULong:
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG);
		case clang::BuiltinType::ULongLong:
			log_->primitiveTypeNotSupported("unsigned long long");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG);
		case clang::BuiltinType::UInt128:
			log_->primitiveTypeNotSupported("__uint128_t");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG);
			// signed types
		case clang::BuiltinType::Char_S:
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::SChar:
			log_->primitiveTypeNotSupported("signed char");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::WChar_S:
			log_->primitiveTypeNotSupported("wchar_t");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::CHAR);
		case clang::BuiltinType::Short:
			log_->primitiveTypeNotSupported("short");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::INT);
		case clang::BuiltinType::Int:
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::INT);
		case clang::BuiltinType::Long:
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::LONG);
		case clang::BuiltinType::LongLong:
			log_->primitiveTypeNotSupported("long long");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::LONG);
		case clang::BuiltinType::Int128:
			log_->primitiveTypeNotSupported("__int128_t");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::LONG);
			// float types
		case clang::BuiltinType::Half:
			log_->primitiveTypeNotSupported("half");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::FLOAT);
		case clang::BuiltinType::Float:
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::FLOAT);
		case clang::BuiltinType::Double:
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::DOUBLE);
		case clang::BuiltinType::LongDouble:
			log_->primitiveTypeNotSupported("long double");
			return clang_.createNode<OOModel::PrimitiveTypeExpression>
					(typeLoc.getSourceRange(), OOModel::PrimitiveTypeExpression::PrimitiveTypes::DOUBLE);
			// c++ specific
		case clang::BuiltinType::NullPtr:
			return createErrorExpression("Unsupported type", typeLoc.getSourceRange());
		default:
			return createErrorExpression("Unsupported type", typeLoc.getSourceRange());
	}
}

OOModel::Expression* CppImportUtilities::translateTypePtr(const clang::TypeLoc type)
{
	Q_ASSERT(type);
	if (type.getAs<clang::AutoTypeLoc>())
		return clang_.createNode<OOModel::AutoTypeExpression>(type.getSourceRange());
	else if (type.getAs<clang::TypedefTypeLoc>())
		return clang_.createReference(type.getSourceRange());
	else if (auto recordTypeLoc = type.getAs<clang::RecordTypeLoc>())
		return setReferencePrefix(clang_.createReference(type.getSourceRange()),
										  recordTypeLoc.getDecl()->getQualifierLoc());
	else if (auto pointerType = type.getAs<clang::PointerTypeLoc>())
		return clang_.createNode<OOModel::PointerTypeExpression>(type.getSourceRange(),
																					translateQualifiedType(pointerType.getNextTypeLoc()));
	else if (auto refType = type.getAs<clang::ReferenceTypeLoc>())
		return clang_.createNode<OOModel::ReferenceTypeExpression>(type.getSourceRange(),
																					  translateQualifiedType(refType.getNextTypeLoc()),
																					  !type.getAs<clang::RValueReferenceTypeLoc>().isNull());
	else if (auto enumType = type.getAs<clang::EnumTypeLoc>())
		return setReferencePrefix(clang_.createReference(type.getSourceRange()), enumType.getDecl()->getQualifierLoc());
	else if (auto constArrayType = type.getAs<clang::ConstantArrayTypeLoc>())
	{
		auto ooArrayType = clang_.createNode<OOModel::ArrayTypeExpression>(type.getSourceRange());
		ooArrayType->setTypeExpression(translateQualifiedType(constArrayType.getElementLoc()));
		auto integerLiteral = clang_.createNode<OOModel::IntegerLiteral>(type.getSourceRange(),
					(int)llvm::dyn_cast<clang::ConstantArrayType>(constArrayType.getTypePtr())->getSize().getLimitedValue());
		ooArrayType->setFixedSize(integerLiteral);
		return ooArrayType;
	}
	else if (auto incompleteArrayType = type.getAs<clang::IncompleteArrayTypeLoc>())
	{
		auto ooArrayType = clang_.createNode<OOModel::ArrayTypeExpression>(type.getSourceRange());
		ooArrayType->setTypeExpression(translateQualifiedType(incompleteArrayType.getElementLoc()));
		return ooArrayType;
	}
	else if (auto parenType = type.getAs<clang::ParenTypeLoc>())
		// TODO: this might not always be a nice solution, to just return the inner type of a parenthesized type.
		return translateQualifiedType(parenType.getInnerLoc());
	else if (type.getAs<clang::TypedefTypeLoc>())
		return clang_.createReference(type.getSourceRange());
	else if (type.getAs<clang::TemplateTypeParmTypeLoc>())
		return clang_.createReference(type.getSourceRange());
	else if (auto functionProtoType = type.getAs<clang::FunctionProtoTypeLoc>())
	{
		// TODO: include templates. (and more?)
		auto ooFunctionType = clang_.createNode<OOModel::FunctionTypeExpression>(type.getSourceRange());
		ooFunctionType->results()->append(translateQualifiedType(functionProtoType.getReturnLoc()));
		for (auto param : functionProtoType.getParams())
		{
			auto variableDeclaration = clang_.createNamedNode<OOModel::VariableDeclaration>(param,
																	translateQualifiedType(param->getTypeSourceInfo()->getTypeLoc()));
			ooFunctionType->arguments()->append(clang_.createNode<OOModel::VariableDeclarationExpression>(
																param->getSourceRange(), variableDeclaration));
		}
		return ooFunctionType;
	}
	else if (auto elaboratedType = type.getAs<clang::ElaboratedTypeLoc>())
	{
		// TODO: this might also have a keyword in front (like e.g. class, typename)
		auto translatedElaboratedType = translateQualifiedType(elaboratedType.getNamedTypeLoc());
		if (auto ooRef = DCast<OOModel::ReferenceExpression>(translatedElaboratedType))
			return setReferencePrefix(ooRef, elaboratedType.getQualifierLoc());
		return translatedElaboratedType;
	}
	else if (auto templateSpecialization = type.getAs<clang::TemplateSpecializationTypeLoc>())
	{
		auto ooRef = clang_.createReference(type.getSourceRange());
		for (unsigned i = 0; i < templateSpecialization.getNumArgs(); i++)
			ooRef->typeArguments()->append(translateTemplateArgument(templateSpecialization.getArgLoc(i)));
		return ooRef;
	}
	else if (auto dependentTypeLoc = type.getAs<clang::DependentNameTypeLoc>())
	{
		auto ooRef = setReferencePrefix(clang_.createReference(dependentTypeLoc.getNameLoc()),
												  dependentTypeLoc.getQualifierLoc());
		if (dependentTypeLoc.getTypePtr()->castAs<clang::DependentNameType>()->getKeyword() == clang::ETK_Typename)
		{
			auto ooTypeName = clang_.createNode<OOModel::TypeNameOperator>(type.getSourceRange());
			ooTypeName->setTypeExpression(ooRef);
			return ooTypeName;
		}
		return ooRef;
	}
	else if (type.getAs<clang::InjectedClassNameTypeLoc>())
		return clang_.createReference(type.getSourceRange());
	else if (auto substTemplateParm = type.getAs<clang::SubstTemplateTypeParmTypeLoc>())
		return translateQualifiedType(substTemplateParm.getNextTypeLoc());
	else if (auto builtIn = type.getAs<clang::BuiltinTypeLoc>())
		return translateBuiltInClangType(builtIn);

	return createErrorExpression("Unsupported Type", type.getSourceRange());
}

}
