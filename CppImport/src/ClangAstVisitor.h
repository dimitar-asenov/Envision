#pragma once

#include "cppimport_api.h"
#include "CppImportUtilities.h"
#include "TranslateManager.h"

using clang::RecursiveASTVisitor;

class ClangAstVisitor : public RecursiveASTVisitor <ClangAstVisitor>
{
public:
    ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject);
    bool VisitStmt(clang::Stmt* S);
    bool VisitDecl(clang::Decl* D);
    bool VisitCXXRecordDecl(clang::CXXRecordDecl* rd);
    bool VisitVarDecl(clang::VarDecl* vd);
    bool VisitFieldDecl(clang::FieldDecl* fd);
    bool VisitCXXMethodDecl(clang::CXXMethodDecl* methotDecl);

protected:
    OOModel::Project* currentProject_;
    Model::Model* currentModel_;
    clang::ASTContext* astContext_;
    OOModel::Class* currentClass_;
    OOModel::Method* currentMethod_;
    Model::Node* currentNode_;
    TranslateManager* trMngr_;

};
