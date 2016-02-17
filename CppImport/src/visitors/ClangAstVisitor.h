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

#include "../cppimport_api.h"
#include "../manager/TranslateManager.h"
#include "../macro/MacroImporter.h"
#include "../CppImportLogger.h"
#include "../comments/CommentParser.h"
#include "../Comment.h"

namespace CppImport {

class ExpressionVisitor;
class TemplateArgumentVisitor;
/**
 * This is the core visitor of the CppImport plugin it translates declarations and statements
 * from clang's AST to Envision's AST.
 *
 * One can specify to also import system headers with the \a importSysHeader_ variable.
 */
class CPPIMPORT_API ClangAstVisitor : public clang::RecursiveASTVisitor <ClangAstVisitor>
{
	public:
		ClangAstVisitor(OOModel::Project* project, const QString& projectPath,  CppImportLogger* logger);
		~ClangAstVisitor();
		void setSourceManagerAndPreprocessor(const clang::SourceManager* sourceManager,
														 const clang::Preprocessor* preprocessor);

		// helper functions to interact with the stack from other classes
		Model::Node* ooStackTop();
		void pushOOStack(Model::Node* node);
		Model::Node* popOOStack();

		// method only for debugging
		bool VisitDecl(clang::Decl* decl);
		bool TraverseDecl(clang::Decl* decl);

		bool TraverseNamespaceDecl(clang::NamespaceDecl* namespaceDecl);
		bool TraverseClassTemplateDecl(clang::ClassTemplateDecl* classTemplate);
		bool TraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* specializationDecl);
		bool TraverseCXXRecordDecl(clang::CXXRecordDecl* recordDecl);

		// methods
		bool TraverseCXXMethodDecl(clang::CXXMethodDecl* methodDecl);
		bool TraverseCXXConstructorDecl(clang::CXXConstructorDecl* constructorDecl);
		bool TraverseCXXDestructorDecl(clang::CXXDestructorDecl* destructorDecl);
		bool TraverseCXXConversionDecl(clang::CXXConversionDecl* conversionDecl);
		bool TraverseFunctionDecl(clang::FunctionDecl* functionDecl);
		bool TraverseFunctionTemplateDecl(clang::FunctionTemplateDecl* functionDecl);

		bool TraverseVarDecl(clang::VarDecl* varDecl);
		bool TraverseFieldDecl(clang::FieldDecl* fieldDecl);
		bool TraverseEnumDecl(clang::EnumDecl* enumDecl);

		bool WalkUpFromTypedefNameDecl(clang::TypedefNameDecl* typedefDecl);
		bool TraverseTypeAliasTemplateDecl(clang::TypeAliasTemplateDecl* typeAliasTemplate);
		bool TraverseNamespaceAliasDecl(clang::NamespaceAliasDecl* namespaceAlias);
		bool TraverseUsingDecl(clang::UsingDecl* usingDecl);
		bool TraverseUsingDirectiveDecl(clang::UsingDirectiveDecl* usingDirectiveDecl);
		bool TraverseUnresolvedUsingValueDecl(clang::UnresolvedUsingValueDecl* unresolvedUsing);
		bool TraverseStaticAssertDecl(clang::StaticAssertDecl* staticAssertDecl);

		/**
		 * Traverses the stmt \a S and if it is an expression dispatches to the expression visitor
		 */
		bool TraverseStmt(clang::Stmt* S);
		// method only for debugging
		bool VisitStmt(clang::Stmt* S);

		bool TraverseCompoundStmt(clang::CompoundStmt* compoundStmt);

		bool TraverseIfStmt(clang::IfStmt* ifStmt);
		bool TraverseWhileStmt(clang::WhileStmt* whileStmt);
		bool TraverseDoStmt(clang::DoStmt* doStmt);
		bool TraverseForStmt(clang::ForStmt* forStmt);
		bool TraverseCXXForRangeStmt(clang::CXXForRangeStmt* forRangeStmt);
		bool TraverseReturnStmt(clang::ReturnStmt* returnStmt);
		bool TraverseDeclStmt(clang::DeclStmt* declStmt);

		bool TraverseCXXTryStmt(clang::CXXTryStmt* tryStmt);
		bool TraverseCXXCatchStmt(clang::CXXCatchStmt* catchStmt);

		bool TraverseSwitchStmt(clang::SwitchStmt* switchStmt);
		bool TraverseCaseStmt(clang::CaseStmt* caseStmt);
		bool TraverseDefaultStmt(clang::DefaultStmt* defaultStmt);

		bool TraverseBreakStmt(clang::BreakStmt* breakStmt);
		bool TraverseContinueStmt(clang::ContinueStmt* continueStmt);
		/**
		 * A helper function called by RecursiveASTVisitor parent class.
		 * In our case this function should return false because we use a custom traversal strategy.
		 */
		bool shouldUseDataRecursionfor (clang::Stmt* S);

		void beforeTranslationUnit(clang::ASTContext& astContext);
		void endTranslationUnit();
		void endEntireImport();

	private:
		using Base = clang::RecursiveASTVisitor<ClangAstVisitor>;

		QHash<QString, QSet<QString>> projectIncludes_;

		QStack<Model::Node*> ooStack_;
		QStack<OOModel::Expression*> ooExprStack_;
		ClangHelpers clang_;
		MacroImporter macroImporter_;

		CppImportLogger* log_{};
		TranslateManager* trMngr_{};
		CppImportUtilities* utils_{};
		ExpressionVisitor* exprVisitor_{};
		TemplateArgumentVisitor* templArgVisitor_{};
		CommentParser* commentParser_{};
		bool importSysHeader_{false};
		bool inBody_{true};
		const QString className_{"ClangAstVisitor"};
		QList<clang::UsingDirectiveDecl*> usingDirectiveDeclarations_;

		/**
		 * Abstract function to handle normal member functions, constructors, destructors and conversion functions.
		 * This method will translate the complete method if \a methodDecl is a definition
		 */
		bool TraverseMethodDecl(clang::CXXMethodDecl* methodDecl, OOModel::Method::MethodKind kind);

		/**
		 * Insert the class in the tree and will visit the body and insert base classes.
		 * It does not visit type arguments they should be handled on the caller site.
		 */
		void TraverseClass(clang::CXXRecordDecl* recordDecl, OOModel::Class* ooClass);

		/**
		 * Abstract function to handle common parts between FunctionDecl and its subclasses.
		 * This includes traversing the body.
		 */
		void TraverseFunction(clang::FunctionDecl* functionDecl, OOModel::Method* ooFunction);

		/**
		 * Inserts a friend class with the \a typeInfo in \a ooClass.
		 */
		void insertFriendClass(clang::FriendDecl* friendDecl, OOModel::Class* ooClass);
		/**
		 * Inserts the friend function \a friendFunction in \a ooClass.
		 */
		void insertFriendFunction(clang::FriendDecl* friendDecl, OOModel::Class* ooClass);

		/**
		 * Returns if it is intended to import the code at the \a location.
		 * The decision is based on wheter the \a location is valid and on the value of \a importSysHeader_
		 */
		bool shouldImport(const clang::SourceLocation& location);

		void addFunctionModifiers(clang::FunctionDecl* functionDecl, OOModel::Method* method);

		void insertUsingDirectiveDeclarations();
};

// method
inline bool ClangAstVisitor::TraverseCXXMethodDecl(clang::CXXMethodDecl *methodDecl)
{return TraverseMethodDecl(methodDecl, OOModel::Method::MethodKind::Default); }
inline bool ClangAstVisitor::TraverseCXXConstructorDecl(clang::CXXConstructorDecl *constructorDecl)
{return TraverseMethodDecl(constructorDecl, OOModel::Method::MethodKind::Constructor); }
inline bool ClangAstVisitor::TraverseCXXDestructorDecl(clang::CXXDestructorDecl *destructorDecl)
{return TraverseMethodDecl(destructorDecl, OOModel::Method::MethodKind::Destructor); }
inline bool ClangAstVisitor::TraverseCXXConversionDecl(clang::CXXConversionDecl *conversionDecl)
// TODO: handle explicit keyword
{return TraverseMethodDecl(conversionDecl, OOModel::Method::MethodKind::Conversion); }

}
