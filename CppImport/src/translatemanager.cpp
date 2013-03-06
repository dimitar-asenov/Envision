#include "translatemanager.h"

TranslateManager::TranslateManager(Model::Model* model, OOModel::Project* project) :
    model_(model), project_(project)
{
}

void TranslateManager::insertClass(clang::CXXRecordDecl* rDecl)
{
    //if rdecl is not managed yet add it:
    if(!classMap_.contains(rDecl))
    {
        std::cout << "CLASS " << rDecl->getName().str() << " NOT IN MAP " << std::endl;
    }
}

void TranslateManager::insertMethodDecl(clang::CXXMethodDecl* mDecl)
{
    mDecl->getParent();
}

void TranslateManager::insertField(clang::FieldDecl* fDecl)
{
    fDecl->getParent();
}

void TranslateManager::insertVar(clang::VarDecl* vDecl)
{
    vDecl->getParentFunctionOrMethod();
}
