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
        std::cout << "-------------------------->CLASS " << rDecl->getName().str() << " NOT IN MAP " << std::endl;
        classMap_.insert(rDecl,nullptr);
    }
}

void TranslateManager::insertMethodDecl(clang::CXXMethodDecl* mDecl)
{
//    mDecl->getParent();
    if(!methodMap_.contains(mDecl))
    {
        if(methodMap_.contains(mDecl->getCorrespondingMethodInClass(mDecl->getParent())))
            std::cout << "IS DECLARED BEFORE" << std::endl;

        std::cout << "-------------------------->METHOD " << mDecl->getName().str() << " NOT IN MAP ---->ID " << mDecl->isThisDeclarationADefinition() <<std::endl;
        methodMap_.insert(mDecl,nullptr);
    }
}

void TranslateManager::insertField(clang::FieldDecl* fDecl)
{
    fDecl->getParent();
}

void TranslateManager::insertVar(clang::VarDecl* vDecl)
{
    vDecl->getParentFunctionOrMethod();
}
