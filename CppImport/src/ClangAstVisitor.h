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

#include "cppimport_api.h"
#include "TranslateManager.h"
#include "CppImportLogger.h"

namespace CppImport {

class ClangAstVisitor : public clang::RecursiveASTVisitor <ClangAstVisitor>
{
	typedef clang::RecursiveASTVisitor<ClangAstVisitor> Base;
	public:
		ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject, CppImportLogger* logger);
		~ClangAstVisitor();

		bool TraverseNamespaceDecl(clang::NamespaceDecl* namespaceDecl);
		bool TraverseCXXRecordDecl(clang::CXXRecordDecl* recordDecl);
		bool TraverseCXXMethodDecl(clang::CXXMethodDecl* methodDecl);
		bool TraverseFunctionDecl(clang::FunctionDecl* functionDecl);
		bool TraverseCXXConstructorDecl(clang::CXXConstructorDecl* constructorDecl);
		bool TraverseCXXDestructorDecl(clang::CXXDestructorDecl* destructorDecl);

		bool TraverseIfStmt(clang::IfStmt* ifStmt);
		bool TraverseConditionalOperator(clang::ConditionalOperator* conditionalOperator);
		bool TraverseWhileStmt(clang::WhileStmt* whileStmt);
		bool TraverseForStmt(clang::ForStmt* forStmt);
		bool TraverseSwitchStmt(clang::SwitchStmt* switchStmt);
		bool TraverseReturnStmt(clang::ReturnStmt* returnStmt);


		bool TraverseCXXTryStmt(clang::CXXTryStmt* tryStmt);
		bool TraverseCXXCatchStmt(clang::CXXCatchStmt* catchStmt);
		bool TraverseCXXThrowExpr(clang::CXXThrowExpr* throwExpr);

		bool TraverseLambdaExpr(clang::LambdaExpr* lambdaExpr);


		bool TraverseStmt(clang::Stmt *S);
		bool TraverseVarDecl(clang::VarDecl* varDecl);

		bool TraverseEnumDecl(clang::EnumDecl* enumDecl);

		// casts
		bool TraverseCStyleCastExpr(clang::CStyleCastExpr* castExpr);
		bool TraverseCXXConstCastExpr(clang::CXXConstCastExpr* castExpr);
		bool TraverseCXXDynamicCastExpr(clang::CXXDynamicCastExpr* castExpr);
		bool TraverseCXXReinterpretCastExpr(clang::CXXReinterpretCastExpr* castExpr);
		bool TraverseCXXStaticCastExpr(clang::CXXStaticCastExpr* castExpr);
		bool TraverseCXXFunctionalCastExpr(clang::CXXFunctionalCastExpr* castExpr);

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
		//     binary assigns
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


		bool VisitStmt(clang::Stmt* S);
		bool VisitFieldDecl(clang::FieldDecl* fieldDecl);

		bool TraverseCXXMemberCallExpr(clang::CXXMemberCallExpr* callExpr);
		bool TraverseCXXNewExpr(clang::CXXNewExpr* newExpr);
		bool VisitIntegerLiteral(clang::IntegerLiteral* intLit);
		bool VisitCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr* boolLitExpr);
		bool VisitFloatingLiteral(clang::FloatingLiteral* floatLiteral);
		bool VisitCharacterLiteral(clang::CharacterLiteral* charLiteral);
		bool VisitStringLiteral(clang::StringLiteral* stringLiteral);
		bool VisitDeclRefExpr(clang::DeclRefExpr* declRefExpr);
		bool VisitCXXUnresolvedConstructorExpr(clang::CXXUnresolvedConstructExpr* unresolvedConstructorExpr);
		bool TraverseParenExpr(clang::ParenExpr* parenthesizedExpr);
		bool TraverseArraySubscriptExpr(clang::ArraySubscriptExpr* arraySubsrciptExpr);
		bool VisitCXXThisExpr(clang::CXXThisExpr* thisExpr);
		bool VisitMemberExpr(clang::MemberExpr* memberExpr);

		bool TraverseCXXConstructExpr(clang::CXXConstructExpr* constructExpr);

		bool TraverseCaseStmt(clang::CaseStmt* caseStmt);
		bool TraverseDefaultStmt(clang::DefaultStmt* defaultStmt);

		bool TraverseInitListExpr(clang::InitListExpr* initListExpr);

		bool VisitBreakStmt(clang::BreakStmt* breakStmt);
		bool VisitContinueStmt(clang::ContinueStmt* continueStmt);

		bool shouldUseDataRecursionFor(clang::Stmt* S);

	private:
		bool TraverseBinaryOp(clang::BinaryOperator* binaryOperator);
		bool TraverseAssignment(clang::BinaryOperator* binaryOperator);

		bool TraverseUnaryOp(clang::UnaryOperator* unaryOperator);

		bool TraverseExplCastExpr(clang::ExplicitCastExpr* castExpr, OOModel::CastExpression::CastKind kind);

		QStack<Model::Node*> ooStack_;
		QStack<OOModel::Expression*> ooExprStack_;
		QStack<OOModel::SwitchCase*> ooSwitchCaseStack_;

		Model::Model* currentModel_{};
		OOModel::Project* currentProject_{};
		clang::ASTContext* astContext_{};
		TranslateManager* trMngr_{};
		CppImportLogger* log_{};
		CppImportUtilities* utils_{};
		bool inBody_{true};
		QString className_{"ClangAstVisitor"};
};

// binary ops
inline bool ClangAstVisitor::TraverseBinMul(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinDiv(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinRem(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinAdd(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinSub(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinLT(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinGT(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinLE(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinGE(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinEQ(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinNE(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinAnd(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinXor(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinOr(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinLAnd(clang::BinaryOperator *binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinLOr(clang::BinaryOperator *binOp)
{return TraverseBinaryOp(binOp);}

inline bool ClangAstVisitor::TraverseBinComma(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinPtrMemD(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinPtrMemI(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}

// binary assigns
inline bool ClangAstVisitor::TraverseBinAssign(clang::BinaryOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ClangAstVisitor::TraverseBinMulAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ClangAstVisitor::TraverseBinDivAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ClangAstVisitor::TraverseBinRemAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ClangAstVisitor::TraverseBinAddAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ClangAstVisitor::TraverseBinSubAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ClangAstVisitor::TraverseBinShlAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ClangAstVisitor::TraverseBinShrAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ClangAstVisitor::TraverseBinAndAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ClangAstVisitor::TraverseBinXorAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}
inline bool ClangAstVisitor::TraverseBinOrAssign(clang::CompoundAssignOperator* binOp)
{return TraverseAssignment(binOp);}

//unary ops
inline bool ClangAstVisitor::TraverseUnaryPostInc(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ClangAstVisitor::TraverseUnaryPostDec(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ClangAstVisitor::TraverseUnaryPreInc(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ClangAstVisitor::TraverseUnaryPreDec(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ClangAstVisitor::TraverseUnaryAddrOf(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ClangAstVisitor::TraverseUnaryDeref(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ClangAstVisitor::TraverseUnaryPlus(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ClangAstVisitor::TraverseUnaryMinus(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ClangAstVisitor::TraverseUnaryNot(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ClangAstVisitor::TraverseUnaryLNot(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}

inline bool ClangAstVisitor::TraverseUnaryReal(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ClangAstVisitor::TraverseUnaryImag(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}
inline bool ClangAstVisitor::TraverseUnaryExtension(clang::UnaryOperator* uOp)
{return TraverseUnaryOp(uOp);}

// casts
inline bool ClangAstVisitor::TraverseCStyleCastExpr(clang::CStyleCastExpr* castExpr)
{return TraverseExplCastExpr(castExpr, OOModel::CastExpression::CastKind::Default); }
inline bool ClangAstVisitor::TraverseCXXConstCastExpr(clang::CXXConstCastExpr* castExpr)
{return TraverseExplCastExpr(castExpr, OOModel::CastExpression::CastKind::ConstCast); }
inline bool ClangAstVisitor::TraverseCXXDynamicCastExpr(clang::CXXDynamicCastExpr* castExpr)
{return TraverseExplCastExpr(castExpr, OOModel::CastExpression::CastKind::DynamicCast); }
inline bool ClangAstVisitor::TraverseCXXReinterpretCastExpr(clang::CXXReinterpretCastExpr* castExpr)
{return TraverseExplCastExpr(castExpr, OOModel::CastExpression::CastKind::ReinterpretCast); }
inline bool ClangAstVisitor::TraverseCXXStaticCastExpr(clang::CXXStaticCastExpr* castExpr)
{return TraverseExplCastExpr(castExpr, OOModel::CastExpression::CastKind::StaticCast); }
inline bool ClangAstVisitor::TraverseCXXFunctionalCastExpr(clang::CXXFunctionalCastExpr* castExpr)
{return TraverseExplCastExpr(castExpr, OOModel::CastExpression::CastKind::FunctionalCast); }

}
