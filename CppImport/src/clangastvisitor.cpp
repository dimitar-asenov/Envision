#include "clangastvisitor.h"



using namespace clang;
using namespace llvm;
using namespace OOModel;

ClangAstVisitor::ClangAstVisitor(Model::Model* model,OOModel::Project *currentProject = nullptr)
{
    this->currentModel_ = model;
    this->currentProject_ = currentProject;
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
    if(isa<CXXRecordDecl>(D))
    {
        CXXRecordDecl *recDecl = cast<CXXRecordDecl>(D);
        if(recDecl->isClass())
        {
            Class* cl = nullptr;

            if (!currentProject_) cl = dynamic_cast<Class*> (currentModel_->createRoot("Class"));
            currentModel_->beginModification(currentProject_ ? static_cast<Model::Node*> (currentProject_) :cl, "Adding a class.");
            if (!cl)
            {
                cl = new Class();
                currentProject_->classes()->append(cl);
            }

            //TODO is name correct?
            cl->setName(QString::fromStdString(recDecl->getName().str()));

            currentModel_->endModification();
            return cl;

        }
    }

    return true;
}
