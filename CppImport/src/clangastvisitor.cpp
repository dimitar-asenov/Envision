#include "clangastvisitor.h"



using namespace clang;
using namespace llvm;
using namespace OOModel;

ClangAstVisitor::ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject = nullptr)
{
    this->currentModel_ = model;
    this->currentProject_ = currentProject;
    currentClass_ = nullptr;
    currentMethod_ = nullptr;
}

bool ClangAstVisitor::VisitStmt(clang::Stmt* S)
{
    if(isa<IfStmt>(S))
    {

    }
    return true;
}



bool ClangAstVisitor::VisitDecl(clang::Decl* D)
{
    std::cout << "visiting DECL " << D->getDeclKindName() <<  D->getDeclKindName() <<std::endl;

    return true;
}

bool ClangAstVisitor::VisitCXXRecordDecl(CXXRecordDecl* rd)
{
    std::cout << "Visiting ClassDecl " << rd->getName().str() <<std::endl;

    if(isa<CXXRecordDecl>(rd))
    {
        CXXRecordDecl* recDecl = cast<CXXRecordDecl>(rd);
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

bool ClangAstVisitor::VisitVarDecl(VarDecl* vd)
{
    std::cout << "Visiting VarDecl " << vd->getName().str() <<std::endl;
    if(currentMethod_)
    {
        VariableDeclaration* varDecl = new VariableDeclaration();
        varDecl->setName(QString::fromStdString(vd->getName().str()));
        if(vd->getType().getTypePtr()->isIntegerType())
        {
            varDecl->setVarType(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
        }

        currentMethod_->beginModification("Adding a Variable");
        currentMethod_->items()->append(varDecl);
        currentMethod_->endModification();
    }
    return true;
}

bool ClangAstVisitor::VisitFieldDecl(FieldDecl* fd)
{
    std::cout << "Visiting FieldDecl " << fd->getName().str() << std::endl;

    Field* field = nullptr;

    field = new Field();

    //TODO HOW TO SUPPORT ALL KINDS OF TYPES
    if(fd->getType().getTypePtr()->isIntegerType())
    {
        field->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
    }
    field->setName(QString::fromStdString(fd->getName().str()));
    currentClass_->beginModification("Adding a Field");
    currentClass_->fields()->append(field);
    currentClass_->endModification();
    return true;
}

bool ClangAstVisitor::VisitFunctionDecl(FunctionDecl* funcdecl)
{
    if(isa<CXXConstructorDecl>(funcdecl))
        return true;
    std::cout << "Visiting FunctionDecl " << funcdecl->getName().str() << std::endl;

    Method* method = new Method();

    method->setName(QString::fromStdString(funcdecl->getName().str()));

    if(funcdecl->getResultType().getTypePtr()->isIntegerType())
    {
            FormalResult* methodResult = new FormalResult();
            methodResult->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
            method->results()->append(methodResult);
    }
//    method->setArguments();
    currentClass_->beginModification("Adding a Method");
    currentClass_->methods()->append(method);
    currentClass_->endModification();

    currentMethod_ = method;

    return true;
}
