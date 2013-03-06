#ifndef CLASSMANAGER_H
#define CLASSMANAGER_H

#include <iostream>

#include "OOModel/src/allOOModelNodes.h"
#include "clang/AST/DeclCXX.h"

class TranslateManager
{
public:
    TranslateManager(Model::Model* model, OOModel::Project* project);
    void insertClass(clang::CXXRecordDecl* rDecl);
    void insertMethodDecl(clang::CXXMethodDecl* mDecl);
    void insertField(clang::FieldDecl* fDecl);
    void insertVar(clang::VarDecl* vDecl);


private:
    Model::Model* model_;
    OOModel::Project* project_;

    QMap<clang::CXXRecordDecl*, OOModel::Class*> classMap_;
};

#endif // CLASSMANAGER_H
