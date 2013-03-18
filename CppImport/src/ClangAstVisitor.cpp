#include "ClangAstVisitor.h"

ClangAstVisitor::ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject = nullptr)
{
    this->currentModel_ = model;
    this->currentProject_ = currentProject;
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
            ooStack.push(ooClass);
        }
    }
    return Base::TraverseCXXRecordDecl(rd);
}

bool ClangAstVisitor::TraverseCXXMethodDecl(clang::CXXMethodDecl *methodDecl)
{
    //Constructors not yet handled
    if(llvm::isa<clang::CXXConstructorDecl>(methodDecl))
        return true;
    //translation Manager will insert method in correct class
    OOModel::Method* method = trMngr_->insertMethodDecl(methodDecl);
    if(!method)
    {
        std::cout << "___________ERROR NO OOMODEL::METHOD FOR THIS DECL_______" << std::endl;
        //for now return false to see error
        return false;
    }
    //only visit the body if we are at the definition
    if(methodDecl->isThisDeclarationADefinition())
    {
        ooStack.push(method->items());
        TraverseStmt(methodDecl->getBody());
        ooStack.pop();
    }
    return true;
}

bool ClangAstVisitor::TraverseIfStmt(clang::IfStmt *ifStmt)
{
    OOModel::IfStatement* ooIfStmt = trMngr_->insertIfStmt(ifStmt);
    OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack.top());
    if(itemList)
    {
        itemList->append(ooIfStmt);
        std::cout << "TRAVERSING IF STMT" << std::endl;
        TraverseStmt(ifStmt->getCond());
        ooIfStmt->setCondition(ooExprStack.pop());
        ooStack.push(ooIfStmt->thenBranch());
        TraverseStmt(ifStmt->getThen());
        ooStack.pop();
        ooStack.push(ooIfStmt->elseBranch());
        TraverseStmt(ifStmt->getElse());
        ooStack.pop();
        std::cout << "TRAVERSING IF STMT END" << std::endl;
    }

    return true;
}

bool ClangAstVisitor::TraverseStmt(clang::Stmt *S)
{
    if(S && llvm::isa<clang::BinaryOperator>(S))
    {
        std::cout << "TRAVERSE STMT BINARY OPERATOR" << std::endl;
//        TraverseBinaryOperator(llvm::dyn_cast<clang::BinaryOperator>(S));
    }
    return Base::TraverseStmt(S);
}

bool ClangAstVisitor::VisitStmt(clang::Stmt* S)
{
    std::cout << "VISITING STMT" << std::endl;
    llvm::errs() << "VISITING STMT" << "\n";
    S->dump();
    return Base::VisitStmt(S);
}

bool ClangAstVisitor::VisitDecl(clang::Decl* D)
{
    std::cout << "visiting DECL " << D->getDeclKindName() <<  D->getDeclKindName() <<std::endl;

    return Base::VisitDecl(D);
}

bool ClangAstVisitor::TraverseVarDecl(clang::VarDecl* vd)
{
    std::cout << "Visiting VarDecl " << vd->getName().str() <<std::endl;

    OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack.top());
    if(itemList)
    {
        OOModel::VariableDeclaration* varDecl = new OOModel::VariableDeclaration();
        varDecl->setName(QString::fromStdString(vd->getName().str()));

        OOModel::Expression* type = CppImportUtilities::convertClangType(vd->getType());
        if(type) varDecl->setVarType(type);

        if(vd->hasInit())
        {
            TraverseStmt(vd->getInit());
            varDecl->setInitialValue(ooExprStack.pop());
        }

        trMngr_->insertVar(vd,varDecl);

        itemList->append(varDecl);
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
    OOModel::Field* field = trMngr_->insertField(fd);
    if(!field)
    {
        std::cout << "ERROR COULDN'T INSERT FIELD NO CURRENT OOCLASS" << std::endl;
        return false;
    }
    return true;
}


bool ClangAstVisitor::TraverseBinEQ(clang::BinaryOperator* binOp)
{
    std::cout << "BIIIIIINARY OOP" << std::endl;
    OOModel::BinaryOperation::OperatorTypes ooOperatorType = CppImportUtilities::convertClangOpcode(binOp->getOpcode());
    OOModel::BinaryOperation* ooBinOp = new OOModel::BinaryOperation();
    ooBinOp->setOp(ooOperatorType);
    //    ooExprStack.push(ooBinOp->left());
    TraverseStmt(binOp->getLHS());
    ooBinOp->setLeft(ooExprStack.pop());
    //    ooExprStack.push(ooBinOp->right());
    TraverseStmt(binOp->getRHS());
    ooBinOp->setRight(ooExprStack.pop());
    //    OOModel::Expression* expr = dynamic_cast<OOModel::Expression*> (ooStack.top());
    //    if(expr)
    //    {
    //        expr = ooBinOp;

    //    }
    //    else
    //    {
    //        OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*> (ooStack.top());
    //        if(itemList) itemList->append(ooBinOp);
    //    }
    ooExprStack.push(ooBinOp );
    return true;
}

bool ClangAstVisitor::VisitIntegerLiteral(clang::IntegerLiteral *intLit)
{
    OOModel::IntegerLiteral* ooIntLit = new OOModel::IntegerLiteral();
    ooIntLit->setValue(intLit->getValue().getLimitedValue());
    ooExprStack.push(ooIntLit);

    return true;
}

bool ClangAstVisitor::VisitDeclRefExpr(clang::DeclRefExpr *declRef)
{
    OOModel::ReferenceExpression* refExpr = new OOModel::ReferenceExpression(QString::fromStdString(declRef->getNameInfo().getName().getAsString()));
    ooExprStack.push(refExpr);
    return true;
}
