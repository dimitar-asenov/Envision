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
    trMngr_ = new TranslateManager(model,currentProject);
}

bool ClangAstVisitor::VisitStmt(clang::Stmt* S)
{
    if(S)
    {
        if(isa<IfStmt>(S))
        {

        }
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

            ooClass->setName(QString::fromStdString(recDecl->getName().str()));

            currentModel_->endModification();

            trMngr_->insertClass(rd,ooClass);

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

        Expression* type = CppImportUtilities::convertClangType(vd->getType());
        if(type) varDecl->setVarType(type);

        trMngr_->insertVar(vd,varDecl);

        currentMethod_->beginModification("Adding a Variable");
        currentMethod_->items()->append(varDecl);
        currentMethod_->endModification();
    }
    return true;
}

bool ClangAstVisitor::VisitFieldDecl(FieldDecl* fd)
{
    std::cout << "Visiting FieldDecl " << fd->getName().str() << std::endl;

    Field* field = new Field();

    Expression* type = CppImportUtilities::convertClangType(fd->getType());
    if(type) field->setTypeExpression(type);
    field->setName(QString::fromStdString(fd->getName().str()));
    currentClass_->beginModification("Adding a Field");
    currentClass_->fields()->append(field);
    currentClass_->endModification();
    return true;
}

bool ClangAstVisitor::VisitCXXMethodDecl(CXXMethodDecl *methodDecl)
{
    if(isa<CXXConstructorDecl>(methodDecl))
        return true;
    std::cout << "Visiting FunctionDecl " << methodDecl->getName().str() << std::endl;

    Method* method = new Method();

    method->setName(QString::fromStdString(methodDecl->getName().str()));

    Expression* restype = CppImportUtilities::convertClangType(methodDecl->getResultType());
    if(restype)
    {
        FormalResult* methodResult = new FormalResult();
        methodResult->setTypeExpression(restype);
        method->results()->append(methodResult);
    }

    clang::FunctionDecl::param_const_iterator it = methodDecl->param_begin();
    for(;it != methodDecl->param_end();++it)
    {
        FormalArgument* arg = new FormalArgument();
        arg->setName(QString::fromStdString((*it)->getName().str()));
        Expression* type = CppImportUtilities::convertClangType((*it)->getType());
        if(type) arg->setTypeExpression(type);
        method->arguments()->append(arg);
    }


    trMngr_->insertMethodDecl(methodDecl,method);

    currentMethod_ = method;

    VisitStmt(methodDecl->getBody());

    //decide where to add


    clang::CXXRecordDecl* parentF = methodDecl->getParent();
    std::cout << "-----FUNCTION-->   " << methodDecl->getName().str() << " ----PARENT ---> "<<parentF->getName().str() << std::endl;
    //    currentClass_->beginModification("Adding a Method");
    //    currentClass_->methods()->append(method);
    //    currentClass_->endModification();



    return true;
}
