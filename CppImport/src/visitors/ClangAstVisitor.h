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
#include "../manager/TranslateManager.h"
#include "../CppImportLogger.h"

namespace CppImport {

class ExpressionVisitor;
class TemplateArgumentVisitor;

class ClangAstVisitor : public clang::RecursiveASTVisitor <ClangAstVisitor>
{
	typedef clang::RecursiveASTVisitor<ClangAstVisitor> Base;
	public:
		ClangAstVisitor(OOModel::Project* project, CppImportLogger* logger);
		~ClangAstVisitor();
		void setSourceManager(clang::SourceManager* sourceManager);

		Model::Node* ooStackTop();
		void pushOOStack(Model::Node* node);
		Model::Node* popOOStack();

		bool TraverseNamespaceDecl(clang::NamespaceDecl* namespaceDecl);
		bool TraverseClassTemplateDecl(clang::ClassTemplateDecl* classTemplate);
		bool TraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* specializationDecl);
		bool TraverseCXXRecordDecl(clang::CXXRecordDecl* recordDecl);

		bool TraverseFunctionDecl(clang::FunctionDecl* functionDecl);

		bool TraverseIfStmt(clang::IfStmt* ifStmt);
		bool TraverseWhileStmt(clang::WhileStmt* whileStmt);
		bool TraverseDoStmt(clang::DoStmt* doStmt);
		bool TraverseForStmt(clang::ForStmt* forStmt);
		bool TraverseCXXForRangeStmt(clang::CXXForRangeStmt* forRangeStmt);
		bool TraverseSwitchStmt(clang::SwitchStmt* switchStmt);
		bool TraverseReturnStmt(clang::ReturnStmt* returnStmt);
		bool TraverseDeclStmt(clang::DeclStmt* declStmt);

		bool TraverseCXXTryStmt(clang::CXXTryStmt* tryStmt);
		bool TraverseCXXCatchStmt(clang::CXXCatchStmt* catchStmt);

		bool TraverseStmt(clang::Stmt *S);
		bool TraverseVarDecl(clang::VarDecl* varDecl);

		bool TraverseEnumDecl(clang::EnumDecl* enumDecl);

		// methods
		bool TraverseCXXMethodDecl(clang::CXXMethodDecl* methodDecl);
		bool TraverseCXXConstructorDecl(clang::CXXConstructorDecl* constructorDecl);
		bool TraverseCXXDestructorDecl(clang::CXXDestructorDecl* destructorDecl);
		bool TraverseCXXConversionDecl(clang::CXXConversionDecl* conversionDecl);

		bool TraverseFieldDecl(clang::FieldDecl* fieldDecl);

		bool TraverseCaseStmt(clang::CaseStmt* caseStmt);
		bool TraverseDefaultStmt(clang::DefaultStmt* defaultStmt);

		bool VisitBreakStmt(clang::BreakStmt* breakStmt);
		bool VisitContinueStmt(clang::ContinueStmt* continueStmt);

		bool VisitTypedefNameDecl(clang::TypedefNameDecl* typedefDecl);
		bool VisitNamespaceAliasDecl(clang::NamespaceAliasDecl* namespaceAlias);
		bool VisitUsingDecl(clang::UsingDecl* usingDecl);
		bool VisitUsingDirectiveDecl(clang::UsingDirectiveDecl* usingDirectiveDecl);

		bool shouldUseDataRecursionFor(clang::Stmt* S);

	private:
		bool TraverseBinaryOp(clang::BinaryOperator* binaryOperator);
		bool TraverseAssignment(clang::BinaryOperator* binaryOperator);

		bool TraverseUnaryOp(clang::UnaryOperator* unaryOperator);

		bool TraverseExplCastExpr(clang::ExplicitCastExpr* castExpr, OOModel::CastExpression::CastKind kind);

		bool TraverseMethodDecl(clang::CXXMethodDecl* methodDecl, OOModel::Method::MethodKind kind);
		void TraverseClass(clang::CXXRecordDecl* recordDecl, OOModel::Class* ooClass);
		void TraverseFunction(clang::FunctionDecl* functionDecl, OOModel::Method* ooFunction);

		OOModel::Class* createClass(clang::CXXRecordDecl* recordDecl);

		void insertFriendClass(clang::TypeSourceInfo* typeInfo, OOModel::Class* ooClass);
		void insertFriendFunction(clang::FunctionDecl* friendFunction, OOModel::Class* ooClass);

		bool shouldModel(clang::SourceLocation location);

		QStack<Model::Node*> ooStack_;
		QStack<OOModel::Expression*> ooExprStack_;
		QStack<OOModel::SwitchCase*> ooSwitchCaseStack_;

		clang::ASTContext* astContext_{};
		TranslateManager* trMngr_{};
		CppImportLogger* log_{};
		CppImportUtilities* utils_{};
		ExpressionVisitor* exprVisitor_{};
		TemplateArgumentVisitor* templArgVisitor_{};
		clang::SourceManager* sourceManager_{};
		bool modelSysHeader_{false};
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
inline bool ClangAstVisitor::TraverseCXXConversionDecl(clang::CXXConversionDecl *conversionDecl)
// TODO handle explicit keyword
{return TraverseMethodDecl(conversionDecl, OOModel::Method::MethodKind::Conversion); }

}
