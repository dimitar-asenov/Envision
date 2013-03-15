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

OOModel::Method* TranslateManager::insertMethodDecl(clang::CXXMethodDecl* mDecl)
{
    OOModel::Method* method = nullptr;

    if(!methodMap_.contains(mDecl))
    {
        if(methodMap_.contains(mDecl->getCorrespondingMethodInClass(mDecl->getParent())))
            std::cout << "IS DECLARED BEFORE" << std::endl;
        //Look if there is a function with same name in map
        QMap<clang::CXXMethodDecl*,OOModel::Method*>::iterator it = methodMap_.begin();
        for(;it!=methodMap_.end();++it)
        {
            clang::CXXMethodDecl* inMapDecl = it.key();
            if(mDecl->getName().equals(inMapDecl->getName()) &&
                    !inMapDecl->isThisDeclarationADefinition() && mDecl->isThisDeclarationADefinition())
            {
                //found a pair with same name and only one is defined
                if((mDecl->getResultType() == inMapDecl->getResultType()) &&
                        (mDecl->param_size() == inMapDecl->param_size()))
                {
                    bool matching = true;
                    for(unsigned i = 0; i < mDecl->param_size(); i++)
                    {
                        if(mDecl->getParamDecl(i)->getType() != inMapDecl->getParamDecl(i)->getType())
                            matching = false;
                    }
                    if(matching)
                    {
                        method = it.value();
                        break;
                    }
                }
            }
        }

        if(!method)
        {
            method = new OOModel::Method();
            method->setName(QString::fromStdString(mDecl->getName().str()));

            OOModel::Expression* restype = CppImportUtilities::convertClangType(mDecl->getResultType());
            if(restype)
            {
                OOModel::FormalResult* methodResult = new OOModel::FormalResult();
                methodResult->setTypeExpression(restype);
                method->results()->append(methodResult);
            }

            clang::FunctionDecl::param_const_iterator it = mDecl->param_begin();
            for(;it != mDecl->param_end();++it)
            {
                OOModel::FormalArgument* arg = new OOModel::FormalArgument();
                arg->setName(QString::fromStdString((*it)->getName().str()));
                OOModel::Expression* type = CppImportUtilities::convertClangType((*it)->getType());
                if(type) arg->setTypeExpression(type);
                method->arguments()->append(arg);
            }

            OOModel::Class* parent = classMap_.value(mDecl->getParent());

            parent->methods()->append(method);


            methodMap_.insert(mDecl,method);
        }
    }

    return method;
}

void TranslateManager::insertField(clang::FieldDecl* fDecl)
{
    fDecl->getParent();
}

void TranslateManager::insertVar(clang::VarDecl* vDecl, OOModel::VariableDeclaration* ooVarDecl)
{
    if(!varMap_.contains(vDecl))
    {
        varMap_.insert(vDecl,ooVarDecl);
    }
    else
    {
        std::cout << "---------->VAR : " << vDecl->getName().str() << " IS ALREADY IN THE MAP" << std::endl;
    }



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

OOModel::IfStatement *TranslateManager::insertIfStmt(clang::IfStmt* ifStmt)
{
    OOModel::IfStatement* ooIfStmt = new OOModel::IfStatement();
    ifStmtMap_.insert(ifStmt,ooIfStmt);
    return ooIfStmt;
}
