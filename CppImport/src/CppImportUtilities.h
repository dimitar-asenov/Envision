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

#pragma once

#include "cppimport_api.h"
#include "CppImportLogger.h"
#include "OOModel/src/allOOModelNodes.h"
#include "ClangHelpers.h"

namespace CppImport {

class ExpressionVisitor;

/**
 * This is a helper class for translation of types operators type arguments and similar simple constructs.
 */
class CPPIMPORT_API CppImportUtilities
{
	public:
		CppImportUtilities(CppImportLogger* logger, ExpressionVisitor* visitor, ClangHelpers& clang);

		/**
		 * Translates a qualified type to the corresponding Envision type expression.
		 * The \a location is used for warnings whenever translation fails,
		 * to report the file and line of the \a location.
		 */
		OOModel::Expression* translateQualifiedType(clang::TypeLoc typeLoc);
		OOModel::BinaryOperation::OperatorTypes translateBinaryOp(const clang::BinaryOperatorKind& binaryOpKind);
		OOModel::AssignmentExpression::AssignmentTypes translateAssignOp(const clang::BinaryOperatorKind& assignOpKind);
		OOModel::UnaryOperation::OperatorTypes translateUnaryOp(const clang::UnaryOperatorKind& unaryOpKind);
		OOModel::Modifier::ModifierFlag translateAccessSpecifier(const clang::AccessSpecifier& acessSpecifier);
		OOModel::Modifier::ModifierFlag translateStorageSpecifier(const clang::StorageClass& storageClass);

		OOModel::ReferenceExpression* setReferencePrefix(OOModel::ReferenceExpression* reference,
																		 clang::NestedNameSpecifierLoc nestedNameSpecifierLoc,
																		 clang::Expr* base = nullptr);

		/**
		 * Translates nested name specifiers. If there is a\a base specified
		 * it will be put as a prefix of the first namespecifier
		 * e.g. \c { obj.A::ref } here obj would be the \a base and A the \a nestedName
		 */
		OOModel::Expression* translateNestedNameSpecifier(const clang::NestedNameSpecifierLoc nestedNameLoc,
																		  clang::Expr* base = nullptr);

		OOModel::Expression* translateTemplateArgument(const clang::TemplateArgumentLoc& templateArgLoc);

		OOModel::BinaryOperation::OperatorTypes translateBinaryOverloadOp
		(const clang::OverloadedOperatorKind& overloadOpKind);
		OOModel::AssignmentExpression::AssignmentTypes translateAssignOverloadOp
		(const clang::OverloadedOperatorKind& overloadOpKind);
		OOModel::UnaryOperation::OperatorTypes translateUnaryOverloadOp
		(const clang::OverloadedOperatorKind& overloadOpKind, const unsigned numArgs);
		static QString overloadOperatorToString(const clang::OverloadedOperatorKind& overloadOpKind);

		enum class OverloadKind : int {Unsupported, Unary, Binary, Assign, MethodCall, ReferenceExpr, Comma};
		/**
		 * Determines the kind of an overload operation. e.g. an arrow will return ReferenceExpr
		 */
		OverloadKind getOverloadKind(const clang::OverloadedOperatorKind& overloadOpKind, unsigned numArgs);

		OOModel::MemberInitializer* translateMemberInit(const clang::CXXCtorInitializer* initializer);

		/**
		 * Creates an error expression which is clearly visible inside envision
		 * In envision it is represented as a red # and the \a reason string
		 */
		OOModel::Expression* createErrorExpression(const QString& reason, clang::SourceRange sourceRange);
	private:
		/**
		 * Translates \a type to the nearest primitive OOModel type.
		 */
		OOModel::Expression* translateBuiltInClangType(const clang::BuiltinTypeLoc typeLoc);
		OOModel::Expression* translateTypePtr(const clang::TypeLoc typeLoc);
		const QString className_{"CppImportUtilities"};
		CppImportLogger* log_;
		ExpressionVisitor* exprVisitor_{};
		ClangHelpers& clang_;
		clang::SourceRange calculateSourceRangeWithQualifier(QString qualifier, clang::QualifiedTypeLoc qualifiedTypeLoc);
};
}
