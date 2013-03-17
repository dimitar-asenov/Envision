#pragma once

#include "cppimport_api.h"
#include "CppImportUtilities.h"
#include "TranslateManager.h"
#include "StmtVisitor.h"

class ClangAstVisitor : public clang::RecursiveASTVisitor <ClangAstVisitor>
{
    typedef clang::RecursiveASTVisitor<ClangAstVisitor> Base;
public:
    ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject);

    //bool TraverseExpr(clang::Expr* expression);
   // bool TraverseStmt(clang::Stmt* S);
    bool TraverseCXXRecordDecl(clang::CXXRecordDecl* rd);
    bool TraverseIfStmt(clang::IfStmt* ifStmt);

    bool VisitStmt(clang::Stmt* S);
//    bool VisitIfStmt(clang::IfStmt* ifStmt);
    bool VisitDecl(clang::Decl* D);
//    bool VisitCXXRecordDecl(clang::CXXRecordDecl* rd);
    bool VisitVarDecl(clang::VarDecl* vd);
    bool VisitFieldDecl(clang::FieldDecl* fd);
    bool VisitCXXMethodDecl(clang::CXXMethodDecl* methotDecl);

protected:
    OOModel::Project* currentProject_;
    Model::Model* currentModel_;
    clang::ASTContext* astContext_;
    OOModel::Class* currentClass_;
    OOModel::Method* currentMethod_;
    OOModel::IfStatement* currentIfStmt_;
    Model::Node* currentNode_;
    TranslateManager* trMngr_;

    QStack<Model::Node*> ooStack;

};
