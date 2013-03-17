#include "ClangAstVisitor.h"

ClangAstVisitor::ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject = nullptr)
{
    this->currentModel_ = model;
    this->currentProject_ = currentProject;
    currentClass_ = nullptr;
    currentMethod_ = nullptr;
    currentIfStmt_ = nullptr;
    trMngr_ = new TranslateManager(model,currentProject);

    ooStack.push(currentProject_);
}

bool ClangAstVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl *rd)
{
    std::cout << "Visiting ClassDecl " << rd->getName().str() <<std::endl;


    if(llvm::isa<clang::CXXRecordDecl>(rd))
    {
        clang::CXXRecordDecl* recDecl = llvm::cast<clang::CXXRecordDecl>(rd);
        if(recDecl->isClass())
        {
            OOModel::Class* ooClass = new OOModel::Class();
            currentProject_->classes()->append(ooClass);
            ooClass->setName(QString::fromStdString(recDecl->getName().str()));

            trMngr_->insertClass(rd,ooClass);
            currentClass_ = ooClass;

            ooStack.push(ooClass);
        }
    }
    return Base::TraverseCXXRecordDecl(rd);
}

//bool ClangAstVisitor::TraverseIfStmt(clang::IfStmt *ifStmt)
//{
//    if(currentMethod_)
//    {
//        OOModel::IfStatement* ooIfStmt = trMngr_->insertIfStmt(ifStmt);
//        currentIfStmt_ = ooIfStmt;
//        currentMethod_->items()->append(ooIfStmt);
//        std::cout << "TRAVERSING IF STMT" << std::endl;
//        VisitStmt(ifStmt->getCond());
//        VisitStmt(ifStmt->getThen());
//        VisitStmt(ifStmt->getElse());
//        std::cout << "TRAVERSING IF STMT END" << std::endl;
//        currentIfStmt_ = nullptr;
//    }
//    return true;
//}

bool ClangAstVisitor::VisitStmt(clang::Stmt* S)
{
    std::cout << "VISITING STMT" << std::endl;
    llvm::errs() << "VISITING STMT" << "\n";
    S->dump();
    return Base::VisitStmt(S);
}

bool ClangAstVisitor::VisitIfStmt(clang::IfStmt *ifStmt)
{
    if(currentMethod_)
    {
        OOModel::IfStatement* ooIfStmt = trMngr_->insertIfStmt(ifStmt);
        currentIfStmt_ = ooIfStmt;
        currentMethod_->items()->append(ooIfStmt);
        llvm::errs() << "TRAVERSING IF STMT" << "\n";
        StmtVisitor* stmtV = new StmtVisitor();
        stmtV->TraverseStmt(ifStmt->getThen());
        ooIfStmt->setThenBranch(stmtV->getItems());
//        VisitStmt(ifStmt->getCond());
//        VisitStmt(ifStmt->getThen());
//        VisitStmt(ifStmt->getElse());
        llvm::errs() << "TRAVERSING IF STMT END" << "\n";
        currentIfStmt_ = nullptr;
    }
    return true;
}



bool ClangAstVisitor::VisitDecl(clang::Decl* D)
{
    std::cout << "visiting DECL " << D->getDeclKindName() <<  D->getDeclKindName() <<std::endl;

    return true;
}

//bool ClangAstVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* rd)
//{
//    std::cout << "Visiting ClassDecl " << rd->getName().str() <<std::endl;


//    if(llvm::isa<clang::CXXRecordDecl>(rd))
//    {
//        clang::CXXRecordDecl* recDecl = llvm::cast<clang::CXXRecordDecl>(rd);
//        if(recDecl->isClass())
//        {
//            OOModel::Class* ooClass = new OOModel::Class();;
//            currentProject_->classes()->append(ooClass);
//            ooClass->setName(QString::fromStdString(recDecl->getName().str()));

//            trMngr_->insertClass(rd,ooClass);
//            currentClass_ = ooClass;
//        }
//    }
//    return true;
//}

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

        if(currentIfStmt_)
        {
            OOModel::StatementItemList* items = new OOModel::StatementItemList();
            items->append(varDecl);
            currentIfStmt_->setThenBranch(items);
        }
        else
            currentMethod_->items()->append(varDecl);
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
    currentClass_->fields()->append(field);
    return true;
}

bool ClangAstVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl *methodDecl)
{
    //Constructors not yet handled
    if(llvm::isa<clang::CXXConstructorDecl>(methodDecl))
        return true;
    std::cout << "Visiting FunctionDecl " << methodDecl->getName().str() << std::endl;

    OOModel::Method* method = trMngr_->insertMethodDecl(methodDecl);
    if(method) currentMethod_ = method;
    else
        std::cout << "___________ERROR NO OOMODEL::METHOD FOR THIS DECL_______" << std::endl;
    return true;
}
