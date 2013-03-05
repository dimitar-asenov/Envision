#include "clangastvisitor.h"



using namespace clang;
using namespace llvm;
using namespace OOModel;

ClangAstVisitor::ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject = nullptr)
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

            currentClass_ = ooClass;
        }
    }
    return true;
}

bool ClangAstVisitor::VisitVarDecl(VarDecl *vd)
{
    std::cout << "Visiting VarDecl " << vd->getName().str() <<std::endl;
    return true;
}

bool ClangAstVisitor::VisitFieldDecl(FieldDecl *fd)
{
    std::cout << "Visiting FieldDecl " << fd->getName().str() << std::endl;

    Field* field = nullptr;
    currentClass_->beginModification("Adding a Field");
    field = new Field();

    //TODO HOW TO SUPPORT ALL KINDS OF TYPES
    if(fd->getType().getTypePtr()->isIntegerType())
    {
        field->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
    }
    field->setName(QString::fromStdString(fd->getName().str()));
    currentClass_->fields()->append(field);
    currentClass_->endModification();
    return true;
}
