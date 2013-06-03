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
#include "../TranslateManager.h"
#include "../CppImportLogger.h"
#include "ExpressionVisitor.h"

namespace CppImport {

//forward declare expressionvisitor
class ExpressionVisitor;

class ClangAstVisitor : public clang::RecursiveASTVisitor <ClangAstVisitor>
{
	typedef clang::RecursiveASTVisitor<ClangAstVisitor> Base;
	public:
		ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject, CppImportLogger* logger);
		~ClangAstVisitor();

		bool TraverseNamespaceDecl(clang::NamespaceDecl* namespaceDecl);
		bool TraverseCXXRecordDecl(clang::CXXRecordDecl* recordDecl);
		bool TraverseFunctionDecl(clang::FunctionDecl* functionDecl);

		bool TraverseIfStmt(clang::IfStmt* ifStmt);
		bool TraverseWhileStmt(clang::WhileStmt* whileStmt);
		bool TraverseForStmt(clang::ForStmt* forStmt);
		bool TraverseSwitchStmt(clang::SwitchStmt* switchStmt);
		bool TraverseReturnStmt(clang::ReturnStmt* returnStmt);

		bool TraverseCXXTryStmt(clang::CXXTryStmt* tryStmt);
		bool TraverseCXXCatchStmt(clang::CXXCatchStmt* catchStmt);

		bool TraverseLambdaExpr(clang::LambdaExpr* lambdaExpr);

		bool TraverseStmt(clang::Stmt *S);
		bool TraverseVarDecl(clang::VarDecl* varDecl);

		bool TraverseEnumDecl(clang::EnumDecl* enumDecl);

		// methods
		bool TraverseCXXMethodDecl(clang::CXXMethodDecl* methodDecl);
		bool TraverseCXXConstructorDecl(clang::CXXConstructorDecl* constructorDecl);
		bool TraverseCXXDestructorDecl(clang::CXXDestructorDecl* destructorDecl);

		bool VisitStmt(clang::Stmt* S);
		bool VisitFieldDecl(clang::FieldDecl* fieldDecl);

		bool TraverseCaseStmt(clang::CaseStmt* caseStmt);
		bool TraverseDefaultStmt(clang::DefaultStmt* defaultStmt);

		bool VisitBreakStmt(clang::BreakStmt* breakStmt);
		bool VisitContinueStmt(clang::ContinueStmt* continueStmt);

		bool VisitTypedefNameDecl(clang::TypedefNameDecl* typedefDecl);

		bool shouldUseDataRecursionFor(clang::Stmt* S);

	private:
		bool TraverseBinaryOp(clang::BinaryOperator* binaryOperator);
		bool TraverseAssignment(clang::BinaryOperator* binaryOperator);

		bool TraverseUnaryOp(clang::UnaryOperator* unaryOperator);

		bool TraverseExplCastExpr(clang::ExplicitCastExpr* castExpr, OOModel::CastExpression::CastKind kind);

		bool TraverseMethodDecl(clang::CXXMethodDecl* methodDecl, OOModel::Method::MethodKind kind);

		void insertFriendClass(clang::TypeSourceInfo* typeInfo, OOModel::Class* ooClass);
		void insertFriendFunction(clang::FunctionDecl* friendFunction, OOModel::Class* ooClass);

		QStack<Model::Node*> ooStack_;
		QStack<OOModel::Expression*> ooExprStack_;
		QStack<OOModel::SwitchCase*> ooSwitchCaseStack_;

		Model::Model* currentModel_{};
		OOModel::Project* currentProject_{};
		clang::ASTContext* astContext_{};
		TranslateManager* trMngr_{};
		CppImportLogger* log_{};
		CppImportUtilities* utils_{};
		ExpressionVisitor* exprVisitor_{};
		bool inBody_{true};
		QString className_{"ClangAstVisitor"};
};

// method
inline bool ClangAstVisitor::TraverseCXXMethodDecl(clang::CXXMethodDecl *methodDecl)
{return TraverseMethodDecl(methodDecl, OOModel::Method::MethodKind::Default); }
inline bool ClangAstVisitor::TraverseCXXConstructorDecl(clang::CXXConstructorDecl *constructorDecl)
{return TraverseMethodDecl(constructorDecl, OOModel::Method::MethodKind::Constructor); }
inline bool ClangAstVisitor::TraverseCXXDestructorDecl(clang::CXXDestructorDecl *destructorDecl)
{return TraverseMethodDecl(destructorDecl, OOModel::Method::MethodKind::Destructor); }

}
