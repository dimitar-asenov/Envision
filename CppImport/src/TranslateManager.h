#pragma once

#include "cppimport_api.h"

class TranslateManager
{
public:
    TranslateManager(Model::Model* model, OOModel::Project* project);
    void insertClass(clang::CXXRecordDecl* rDecl,OOModel::Class* ooClass);
    void insertMethodDecl(clang::CXXMethodDecl* mDecl, OOModel::Method* ooMethod);
    void insertField(clang::FieldDecl* fDecl);
    void insertVar(clang::VarDecl* vDecl, OOModel::VariableDeclaration* ooVarDecl);


private:
    Model::Model* model_;
    OOModel::Project* project_;

    QMap<clang::CXXRecordDecl*, OOModel::Class*> classMap_;
    QMap<clang::CXXMethodDecl*, OOModel::Method*> methodMap_;
    QMap<clang::VarDecl*, OOModel::VariableDeclaration*> varMap_;
};
