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

#pragma once

#include "../cppimport_api.h"
#include "../CppImportLogger.h"
#include "OOModel/src/allOOModelNodes.h"

namespace CppImport {

class ClangAstVisitor;
class CppImportUtilities;

/**
 * The expression visitor translates clang expression to envision expression.
 * The way to interact it with it is to call TraverseStmt(someExpr)
 * and then get the translated expression with the  \a getLastExpression method
 */
class CPPIMPORT_API ExpressionVisitor : public clang::RecursiveASTVisitor <ExpressionVisitor>
{
	public:
		ExpressionVisitor(ClangAstVisitor* visitor, CppImportLogger* log);
		/**
		 * This function should be called directly after instantiating this visitor.
		 * The two classes have a dependency on each other.
		 */
		void setUtilities(CppImportUtilities* utils);

		/**
		 * Translates the expression \a s.
		 * Note that \a s should be an expression it is only declared as stmt for ease of use.
		 * Returns the translated expression or an error expression if it could not translate it.
		 */
		OOModel::Expression* translateExpression(clang::Stmt* s);

		bool VisitExpr(clang::Expr* e);

		// memberExpr
		bool TraverseMemberExpr(clang::MemberExpr* memberExpr);
		bool TraverseUnresolvedMemberExpr(clang::UnresolvedMemberExpr* unresolvedMember);
		bool TraverseCXXDependentScopeMemberExpr(clang::CXXDependentScopeMemberExpr* dependentScopeMember);
		// declRefExpr
		bool TraverseDeclRefExpr(clang::DeclRefExpr* declRefExpr);
		bool TraverseDependentScopeDeclRefExpr(clang::DependentScopeDeclRefExpr* dependentScope);
		// callExpr
		bool TraverseCXXMemberCallExpr(clang::CXXMemberCallExpr* callExpr);
		bool TraverseCallExpr(clang::CallExpr* callExpr);
		bool TraverseCXXOperatorCallExpr(clang::CXXOperatorCallExpr* callExpr);
		// memory
		bool TraverseCXXNewExpr(clang::CXXNewExpr* newExpr);
		bool TraverseCXXDeleteExpr(clang::CXXDeleteExpr* deleteExpr);
		// Literals
		bool TraverseIntegerLiteral(clang::IntegerLiteral* intLit);
		bool TraverseCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr* boolLitExpr);
		bool TraverseCXXNullPtrLiteralExpr(clang::CXXNullPtrLiteralExpr*);
		bool TraverseFloatingLiteral(clang::FloatingLiteral* floatLiteral);
		bool TraverseCharacterLiteral(clang::CharacterLiteral* charLiteral);
		bool TraverseStringLiteral(clang::StringLiteral* stringLiteral);
		// construct expr
		bool TraverseCXXConstructExpr(clang::CXXConstructExpr* constructExpr);
		bool TraverseCXXUnresolvedConstructExpr(clang::CXXUnresolvedConstructExpr* unresolvedConstruct);

		bool TraverseParenExpr(clang::ParenExpr* parenthesizedExpr);
		bool TraverseArraySubscriptExpr(clang::ArraySubscriptExpr* arraySubsrciptExpr);

		bool TraverseCXXThisExpr(clang::CXXThisExpr* thisExpr);

		bool TraverseCXXTypeidExpr(clang::CXXTypeidExpr* typeIdExpr);

		bool WalkUpFromOverloadExpr(clang::OverloadExpr* overloadExpr);

		bool TraverseLambdaExpr(clang::LambdaExpr* lambdaExpr);

		bool TraverseConditionalOperator(clang::ConditionalOperator* conditionalOperator);

		bool TraverseCXXThrowExpr(clang::CXXThrowExpr* throwExpr);

		bool TraverseCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr* tempObjectExpr);

		bool TraverseInitListExpr(clang::InitListExpr* initListExpr);

		bool TraverseUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr* typeTrait);

		// inlined methods:
		// binary ops
		bool TraverseBinMul(clang::BinaryOperator* binOp);
		bool TraverseBinDiv(clang::BinaryOperator* binOp);
		bool TraverseBinRem(clang::BinaryOperator* binOp);
		bool TraverseBinAdd(clang::BinaryOperator* binOp);
		bool TraverseBinSub(clang::BinaryOperator* binOp);
		bool TraverseBinLT(clang::BinaryOperator* binOp);
		bool TraverseBinGT(clang::BinaryOperator* binOp);
		bool TraverseBinLE(clang::BinaryOperator* binOp);
		bool TraverseBinGE(clang::BinaryOperator* binOp);
		bool TraverseBinEQ(clang::BinaryOperator* binOp);
		bool TraverseBinNE(clang::BinaryOperator* binOp);
		bool TraverseBinAnd(clang::BinaryOperator* binOp);
		bool TraverseBinXor(clang::BinaryOperator* binOp);
		bool TraverseBinOr(clang::BinaryOperator* binOp);
		bool TraverseBinLAnd(clang::BinaryOperator* binOp);
		bool TraverseBinLOr(clang::BinaryOperator* binOp);
		bool TraverseBinComma(clang::BinaryOperator* binOp);
		bool TraverseBinPtrMemD(clang::BinaryOperator* binOp);
		bool TraverseBinPtrMemI(clang::BinaryOperator* binOp);
		// binary assigns
		bool TraverseBinAssign(clang::BinaryOperator* binOp);
		bool TraverseBinMulAssign(clang::CompoundAssignOperator* binOp);
		bool TraverseBinDivAssign(clang::CompoundAssignOperator* binOp);
		bool TraverseBinRemAssign(clang::CompoundAssignOperator* binOp);
		bool TraverseBinAddAssign(clang::CompoundAssignOperator* binOp);
		bool TraverseBinSubAssign(clang::CompoundAssignOperator* binOp);
		bool TraverseBinShlAssign(clang::CompoundAssignOperator* binOp);
		bool TraverseBinShrAssign(clang::CompoundAssignOperator* binOp);
		bool TraverseBinAndAssign(clang::CompoundAssignOperator* binOp);
		bool TraverseBinXorAssign(clang::CompoundAssignOperator* binOp);
		bool TraverseBinOrAssign(clang::CompoundAssignOperator* binOp);
		// unary ops
		bool TraverseUnaryPostInc(clang::UnaryOperator* uOp);
		bool TraverseUnaryPostDec(clang::UnaryOperator* uOp);
		bool TraverseUnaryPreInc(clang::UnaryOperator* uOp);
		bool TraverseUnaryPreDec(clang::UnaryOperator* uOp);
		bool TraverseUnaryAddrOf(clang::UnaryOperator* uOp);
		bool TraverseUnaryDeref(clang::UnaryOperator* uOp);
		bool TraverseUnaryPlus(clang::UnaryOperator* uOp);
		bool TraverseUnaryMinus(clang::UnaryOperator* uOp);
		bool TraverseUnaryNot(clang::UnaryOperator* uOp);
		bool TraverseUnaryLNot(clang::UnaryOperator* uOp);
		bool TraverseUnaryReal(clang::UnaryOperator* uOp);
		bool TraverseUnaryImag(clang::UnaryOperator* uOp);
		bool TraverseUnaryExtension(clang::UnaryOperator* uOp);
		// casts
		bool TraverseCStyleCastExpr(clang::CStyleCastExpr* castExpr);
		bool TraverseCXXConstCastExpr(clang::CXXConstCastExpr* castExpr);
		bool TraverseCXXDynamicCastExpr(clang::CXXDynamicCastExpr* castExpr);
		bool TraverseCXXReinterpretCastExpr(clang::CXXReinterpretCastExpr* castExpr);
		bool TraverseCXXStaticCastExpr(clang::CXXStaticCastExpr* castExpr);
		bool TraverseCXXFunctionalCastExpr(clang::CXXFunctionalCastExpr* castExpr);

	private:
		using Base = clang::RecursiveASTVisitor<ExpressionVisitor>;

		QStack<OOModel::Expression*> ooExprStack_;
		ClangAstVisitor* baseVisitor_{};
		CppImportLogger* log_{};
		CppImportUtilities* utils_{};
		const QString className_{"ExpressionVisitor"};

		/**
		 * Abstracts common code between all kinds of MemberExpr and DeclRefExpr.
		 * Creates a reference expression with the \a name specified and sets prefixes and type arguments if specified.
		 */
		OOModel::ReferenceExpression* createRef(const QString& name, const clang::SourceLocation& location,
															 const clang::NestedNameSpecifier* qualifier = nullptr,
															 const clang::TemplateArgumentLoc* templateArgs = nullptr,
															 unsigned numTArgs = 0, clang::Expr* base = nullptr);

		/**
		 * Helper Function for all binary operations as clang has for each operator an own Traverse method.
		 */
		bool TraverseBinaryOp(clang::BinaryOperator* binaryOperator);
		/**
		 * Helper Function for all assign operations as clang has for each operator an own Traverse method.
		 */
		bool TraverseAssignment(clang::BinaryOperator* binaryOperator);
		/**
		 * Helper Function for all unary operations as clang has for each operator an own Traverse method.
		 */
		bool TraverseUnaryOp(clang::UnaryOperator* unaryOperator);
		/**
		 * Helper Function for all kind of casts.
		 */
		bool TraverseExplCastExpr(clang::ExplicitCastExpr* castExpr, OOModel::CastExpression::CastKind kind);
};

// binary ops
inline bool ExpressionVisitor::TraverseBinMul(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinDiv(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinRem(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinAdd(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinSub(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinLT(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinGT(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinLE(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinGE(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinEQ(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinNE(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinAnd(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinXor(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinOr(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinLAnd(clang::BinaryOperator *binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinLOr(clang::BinaryOperator *binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinComma(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinPtrMemD(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ExpressionVisitor::TraverseBinPtrMemI(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}

// binary assigns
inline bool ExpressionVisitor::TraverseBinAssign(clang::BinaryOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ExpressionVisitor::TraverseBinMulAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ExpressionVisitor::TraverseBinDivAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ExpressionVisitor::TraverseBinRemAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ExpressionVisitor::TraverseBinAddAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ExpressionVisitor::TraverseBinSubAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ExpressionVisitor::TraverseBinShlAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ExpressionVisitor::TraverseBinShrAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ExpressionVisitor::TraverseBinAndAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ExpressionVisitor::TraverseBinXorAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ExpressionVisitor::TraverseBinOrAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}

//unary ops
inline bool ExpressionVisitor::TraverseUnaryPostInc(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ExpressionVisitor::TraverseUnaryPostDec(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ExpressionVisitor::TraverseUnaryPreInc(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ExpressionVisitor::TraverseUnaryPreDec(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ExpressionVisitor::TraverseUnaryAddrOf(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ExpressionVisitor::TraverseUnaryDeref(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ExpressionVisitor::TraverseUnaryPlus(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ExpressionVisitor::TraverseUnaryMinus(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ExpressionVisitor::TraverseUnaryNot(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ExpressionVisitor::TraverseUnaryLNot(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ExpressionVisitor::TraverseUnaryReal(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ExpressionVisitor::TraverseUnaryImag(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ExpressionVisitor::TraverseUnaryExtension(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}

// casts
inline bool ExpressionVisitor::TraverseCStyleCastExpr(clang::CStyleCastExpr* castExpr)
{return TraverseExplCastExpr(castExpr, OOModel::CastExpression::CastKind::Default); }
inline bool ExpressionVisitor::TraverseCXXConstCastExpr(clang::CXXConstCastExpr* castExpr)
{return TraverseExplCastExpr(castExpr, OOModel::CastExpression::CastKind::ConstCast); }
inline bool ExpressionVisitor::TraverseCXXDynamicCastExpr(clang::CXXDynamicCastExpr* castExpr)
{return TraverseExplCastExpr(castExpr, OOModel::CastExpression::CastKind::DynamicCast); }
inline bool ExpressionVisitor::TraverseCXXReinterpretCastExpr(clang::CXXReinterpretCastExpr* castExpr)
{return TraverseExplCastExpr(castExpr, OOModel::CastExpression::CastKind::ReinterpretCast); }
inline bool ExpressionVisitor::TraverseCXXStaticCastExpr(clang::CXXStaticCastExpr* castExpr)
{return TraverseExplCastExpr(castExpr, OOModel::CastExpression::CastKind::StaticCast); }
inline bool ExpressionVisitor::TraverseCXXFunctionalCastExpr(clang::CXXFunctionalCastExpr* castExpr)
{return TraverseExplCastExpr(castExpr, OOModel::CastExpression::CastKind::FunctionalCast); }

}
