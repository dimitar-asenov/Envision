#include "clangastvisitor.h"



using namespace clang;
using namespace llvm;
using namespace OOModel;

ClangAstVisitor::ClangAstVisitor(ASTContext *context, Model::Model* model, OOModel::Project* currentProject = nullptr)
{
    this->currentModel_ = model;
    this->currentProject_ = currentProject;
    this->astContext_ = context;
}

bool ClangAstVisitor::VisitStmt(clang::Stmt *S)
{
    if(isa<IfStmt>(S))
    {

    }
    return true;
}



bool ClangAstVisitor::VisitDecl(clang::Decl *D)
{
    std::cout << "visiting DECL " << D->getDeclKindName() <<  D->getDeclKindName() <<std::endl;


//    if(isa<CXXRecordDecl>(D))
//    {
//        CXXRecordDecl *recDecl = cast<CXXRecordDecl>(D);
//        if(recDecl->isClass())
//        {
//            Class* ooClass = nullptr;

//            if (!currentProject_) ooClass = dynamic_cast<Class*> (currentModel_->createRoot("Class"));
//            currentModel_->beginModification(currentProject_ ? static_cast<Model::Node*> (currentProject_) : ooClass, "Adding a class.");
//            if (!ooClass)
//            {
//                ooClass = new Class();
//                currentProject_->classes()->append(ooClass);
//            }

//            //TODO is name correct?
//            ooClass->setName(QString::fromStdString(recDecl->getName().str()));

//            currentModel_->endModification();
//        }
//    }

    return true;
}

bool ClangAstVisitor::VisitCXXRecordDecl(CXXRecordDecl *rd)
{
    std::cout << "Visiting ClassDecl " << rd->getName().str() <<std::endl;

    if(isa<CXXRecordDecl>(rd))
    {
        CXXRecordDecl *recDecl = cast<CXXRecordDecl>(rd);
        if(recDecl->isClass())
        {
            Class* ooClass = nullptr;

            if (!currentProject_) ooClass = dynamic_cast<Class*> (currentModel_->createRoot("Class"));
            currentModel_->beginModification(currentProject_ ? static_cast<Model::Node*> (currentProject_) : ooClass, "Adding a class.");
            if (!ooClass)
            {
                ooClass = new Class();
                currentProject_->classes()->append(ooClass);
            }

            //TODO is name correct?
            ooClass->setName(QString::fromStdString(recDecl->getName().str()));

            currentModel_->endModification();
        }
    }
    return true;
}

bool ClangAstVisitor::VisitVarDecl(VarDecl *vd)
{
    std::cout << "Visiting VarDecl " << vd->getName().str() <<std::endl;
    return true;
}
