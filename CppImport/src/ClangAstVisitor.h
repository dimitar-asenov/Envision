#pragma once

#include "cppimport_api.h"
#include "CppImportUtilities.h"
#include "TranslateManager.h"

class ClangAstVisitor : public clang::RecursiveASTVisitor <ClangAstVisitor>
{
    typedef clang::RecursiveASTVisitor<ClangAstVisitor> Base;
public:
    ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject);

    bool TraverseCXXRecordDecl(clang::CXXRecordDecl* rd);
    bool TraverseCXXMethodDecl(clang::CXXMethodDecl* methodDecl);
    bool TraverseIfStmt(clang::IfStmt* ifStmt);

    bool TraverseStmt(clang::Stmt *S);
    bool TraverseVarDecl(clang::VarDecl* vd);
    bool TraverseBinaryOperator(clang::BinaryOperator* binOp);

    bool VisitStmt(clang::Stmt* S);
    bool VisitDecl(clang::Decl* D);
    bool VisitFieldDecl(clang::FieldDecl* fd);

    bool VisitIntegerLiteral(clang::IntegerLiteral* intLit);
    bool VisitDeclRefExpr(clang::DeclRefExpr* declRef);


private:
    QStack<OOModel::Expression*> ooExprStack;

protected:
    OOModel::Project* currentProject_;
    Model::Model* currentModel_;
    clang::ASTContext* astContext_;
    TranslateManager* trMngr_;

    QStack<Model::Node*> ooStack;

};
