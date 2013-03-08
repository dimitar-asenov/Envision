#include "ClangAstVisitor.h"

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
        if(llvm::isa<clang::IfStmt>(S))
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

bool ClangAstVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* rd)
{
    std::cout << "Visiting ClassDecl " << rd->getName().str() <<std::endl;


    if(llvm::isa<clang::CXXRecordDecl>(rd))
    {
        clang::CXXRecordDecl* recDecl = llvm::cast<clang::CXXRecordDecl>(rd);
        if(recDecl->isClass())
        {
            OOModel::Class* ooClass = nullptr;

            if (!currentProject_) ooClass = dynamic_cast<OOModel::Class*> (currentModel_->createRoot("Class"));
            currentModel_->beginModification(currentProject_ ? static_cast<Model::Node*> (currentProject_) : ooClass, "Adding a class.");
            if (!ooClass)
            {
                ooClass = new OOModel::Class();
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

bool ClangAstVisitor::VisitVarDecl(clang::VarDecl* vd)
{
    std::cout << "Visiting VarDecl " << vd->getName().str() <<std::endl;
    if(currentMethod_ && vd->isFunctionOrMethodVarDecl())
    {
        OOModel::VariableDeclaration* varDecl = new OOModel::VariableDeclaration();
        varDecl->setName(QString::fromStdString(vd->getName().str()));

        OOModel::Expression* type = CppImportUtilities::convertClangType(vd->getType());
        if(type) varDecl->setVarType(type);

        trMngr_->insertVar(vd,varDecl);

        currentMethod_->beginModification("Adding a Variable");
        currentMethod_->items()->append(varDecl);
        currentMethod_->endModification();
    }
    else
    {
        if(!llvm::isa<clang::ParmVarDecl>(vd))
            std::cout << "--->WARNING : THIS VARIABLE IS NOT SUPPORTED : " << vd->getName().str() << std::endl;
    }
    return true;
}

bool ClangAstVisitor::VisitFieldDecl(clang::FieldDecl* fd)
{
    std::cout << "Visiting FieldDecl " << fd->getName().str() << std::endl;

    OOModel::Field* field = new OOModel::Field();

    OOModel::Expression* type = CppImportUtilities::convertClangType(fd->getType());
    if(type) field->setTypeExpression(type);
    field->setName(QString::fromStdString(fd->getName().str()));
    currentClass_->beginModification("Adding a Field");
    currentClass_->fields()->append(field);
    currentClass_->endModification();
    return true;
}

bool ClangAstVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl *methodDecl)
{
    if(llvm::isa<clang::CXXConstructorDecl>(methodDecl))
        return true;
    std::cout << "Visiting FunctionDecl " << methodDecl->getName().str() << std::endl;

    OOModel::Method* method = trMngr_->insertMethodDecl(methodDecl);
    if(method) currentMethod_ = method;
    else
        std::cout << "___________ERROR NO OOMODEL::METHOD FOR THIS DECL_______" << std::endl;
    VisitStmt(methodDecl->getBody());

    //decide where to add


    clang::CXXRecordDecl* parentF = methodDecl->getParent();
    std::cout << "-----FUNCTION-->   " << methodDecl->getName().str() << " ----PARENT ---> "<<parentF->getName().str() << std::endl;
    //    currentClass_->beginModification("Adding a Method");
    //    currentClass_->methods()->append(method);
    //    currentClass_->endModification();



    return true;
}
