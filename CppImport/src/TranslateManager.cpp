#include "TranslateManager.h"

TranslateManager::TranslateManager(Model::Model* model, OOModel::Project* project) :
    model_(model), project_(project)
{
}

void TranslateManager::insertClass(clang::CXXRecordDecl* rDecl, OOModel::Class *ooClass)
{
    //if rdecl is not managed yet add it:
    if(!classMap_.contains(rDecl))
    {
        std::cout << "-------------------------->CLASS " << rDecl->getName().str() << " NOT IN MAP " << std::endl;
        classMap_.insert(rDecl,ooClass);
    }
}

void TranslateManager::insertMethodDecl(clang::CXXMethodDecl* mDecl, OOModel::Method *ooMethod)
{
//    mDecl->getParent();
    if(!methodMap_.contains(mDecl))
    {
        if(methodMap_.contains(mDecl->getCorrespondingMethodInClass(mDecl->getParent())))
            std::cout << "IS DECLARED BEFORE" << std::endl;

        std::cout << "-------------------------->METHOD " << mDecl->getName().str() << " NOT IN MAP ---->ID " << mDecl->isThisDeclarationADefinition() <<std::endl;
        OOModel::Class* parent = classMap_.value(mDecl->getParent());


        parent->beginModification("Adding a Method");
        parent->methods()->append(ooMethod);
        parent->endModification();


        methodMap_.insert(mDecl,ooMethod);
    }
}

void TranslateManager::insertField(clang::FieldDecl* fDecl)
{
    fDecl->getParent();
}

void TranslateManager::insertVar(clang::VarDecl* vDecl, OOModel::VariableDeclaration* ooVarDecl)
{
    if(vDecl->getParentFunctionOrMethod())
    {
        //add to method
//        clang::FunctionDecl* parentFunc = dynamic_cast<clang::FunctionDecl*>(vDecl->getParentFunctionOrMethod());
//        if(methodMap_.contains(llvm::dyn_cast<clang::CXXMethodDecl*>(vDecl->getParentFunctionOrMethod())))
//        {
//            std::cout << "FOUND A SUITABLE FUNCTION FOR VAR ------------> " << vDecl->getName().str() << std::endl;
//        }
        ooVarDecl->getAllAttributes();
    }
}
