#pragma once

#include "cppimport_api.h"
#include "CppImportUtilities.h"

class TranslateManager
{
public:
    TranslateManager(Model::Model* model, OOModel::Project* project);
    void insertClass(clang::CXXRecordDecl* rDecl,OOModel::Class* ooClass);
    OOModel::Method* insertMethodDecl(clang::CXXMethodDecl* mDecl);
    void insertField(clang::FieldDecl* fDecl);
    void insertVar(clang::VarDecl* vDecl, OOModel::VariableDeclaration* ooVarDecl);
    OOModel::IfStatement* insertIfStmt(clang::IfStmt* ifStmt);


private:
    Model::Model* model_;
    OOModel::Project* project_;

    QMap<clang::CXXRecordDecl*, OOModel::Class*> classMap_;
    QMap<clang::CXXMethodDecl*, OOModel::Method*> methodMap_;
    QMap<clang::VarDecl*, OOModel::VariableDeclaration*> varMap_;
    QMap<clang::IfStmt*, OOModel::IfStatement*> ifStmtMap_;
};
