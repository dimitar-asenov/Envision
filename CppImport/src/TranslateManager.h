#pragma once

#include "cppimport_api.h"
#include "CppImportUtilities.h"

class TranslateManager
{
public:
    TranslateManager(Model::Model* model, OOModel::Project* project);
    OOModel::Class* insertClass(clang::CXXRecordDecl* rDecl);
    OOModel::Method* insertMethodDecl(clang::CXXMethodDecl* mDecl);
    OOModel::Field* insertField(clang::FieldDecl* fDecl);
    OOModel::VariableDeclaration* insertVar(clang::VarDecl* vDecl);
    OOModel::VariableDeclaration* getVar(clang::VarDecl* vDecl);

private:
    OOModel::Method* addNewMethod(clang::CXXMethodDecl* mDecl);

    Model::Model* model_{};
    OOModel::Project* project_{};

    QMap<clang::CXXRecordDecl*, OOModel::Class*> classMap_{};
    QMap<clang::CXXMethodDecl*, OOModel::Method*> methodMap_{};
    QMap<clang::VarDecl*, OOModel::VariableDeclaration*> varMap_{};
};
