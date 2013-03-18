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

    bool TraverseCXXRecordDecl(clang::CXXRecordDecl* rd);
    bool TraverseCXXMethodDecl(clang::CXXMethodDecl* methodDecl);
    bool TraverseIfStmt(clang::IfStmt* ifStmt);

    bool VisitStmt(clang::Stmt* S);
    bool VisitDecl(clang::Decl* D);
    bool VisitVarDecl(clang::VarDecl* vd);
    bool VisitFieldDecl(clang::FieldDecl* fd);

protected:
    OOModel::Project* currentProject_;
    Model::Model* currentModel_;
    clang::ASTContext* astContext_;
    TranslateManager* trMngr_;

    QStack<Model::Node*> ooStack;

};
