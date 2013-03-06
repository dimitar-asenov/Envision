#include "translatemanager.h"

TranslateManager::TranslateManager(Model::Model* model, OOModel::Project* project) :
    model_(model), project_(project)
{
}

void TranslateManager::insertClass(clang::CXXRecordDecl* rDecl)
{
    //if rdecl is not managed yet add it:

}

void TranslateManager::insertMethodDecl(clang::CXXMethodDecl* mDecl)
{
}

void TranslateManager::insertField(clang::FieldDecl* fDecl)
{
}

void TranslateManager::insertVar(clang::VarDecl* vDecl)
{
}
