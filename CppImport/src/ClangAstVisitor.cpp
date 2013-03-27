/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/

#include "ClangAstVisitor.h"
#include "CppImportUtilities.h"

ClangAstVisitor::ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject, CppImportLogger* logger) :
    currentModel_(model) , currentProject_(currentProject) , log_(logger)
{
    trMngr_ = new TranslateManager(model,currentProject);
    ooStack_.push(currentProject_);
}

ClangAstVisitor::~ClangAstVisitor()
{
    delete trMngr_;
}

bool ClangAstVisitor::TraverseNamespaceDecl(clang::NamespaceDecl *nd)
{
    OOModel::Module* ooModule = trMngr_->insertNamespace(nd);
    // insert it in model
    if(OOModel::Project* curProject = dynamic_cast<OOModel::Project*>(ooStack_.top()))
        curProject->modules()->append(ooModule);
    else if(OOModel::Module* curModel = dynamic_cast<OOModel::Module*>(ooStack_.top()))
        curModel->modules()->append(ooModule);
    else
        log_->writeError(className_,QString("uknown where to put namespace"),QString("NamespaceDecl"),nd->getNameAsString());

    ooStack_.push(ooModule);
    clang::DeclContext::decl_iterator it = nd->decls_begin();
    for(;it!=nd->decls_end();++it)
    {
        TraverseDecl(*it);
    }
    ooStack_.pop();
    return true;
}

bool ClangAstVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl *rd)
{   
    if(rd->getParent()->isNamespace())
        std::cout <<  "CLASS " << rd->getNameAsString() << " has Namespace as Parent" << std::endl;
    if(rd->isClass() || rd->isStruct())
    {
        OOModel::Class* ooClass;
//        if(rd->isClass())
            ooClass = trMngr_->insertClass(rd);
//        else
//            ooClass = trMngr_->insertStruct(rd);
        // check if there was an error inserting class
        if(!ooClass) return false;

        // insert in model
        if(OOModel::Project* curProject = dynamic_cast<OOModel::Project*>(ooStack_.top()))
            curProject->modules()->append(ooClass);
        else if(OOModel::Module* curModel = dynamic_cast<OOModel::Module*>(ooStack_.top()))
            curModel->modules()->append(ooClass);
        else if(OOModel::Class* curClass = dynamic_cast<OOModel::Class*>(ooStack_.top()))
        {
            //TODO: support inner classes in OOModel::Class*
            log_->writeError(className_,QString("inner class found not yet supported"),QString("CXXRecordDecl"),rd->getNameAsString());
            curClass->getAllAttributes();
        }
        else
            log_->writeError(className_,QString("uknown where to put class"),QString("CXXRecordDecl"),rd->getNameAsString());
        // visit child decls
        ooStack_.push(ooClass);
        clang::DeclContext::decl_iterator it = rd->decls_begin();
        for(;it!=rd->decls_end();++it)
        {
            TraverseDecl(*it);
        }
        ooStack_.pop();
    }
//    else if(rd->isStruct())
//    {
//        std::cout << "Struct found " << rd->getNameAsString() << std::endl;
//        return Base::TraverseCXXRecordDecl(rd);
//    }
    else
    {
        std::cout << "Neither class nor Struct " << rd->getNameAsString() << std::endl;
        return Base::TraverseCXXRecordDecl(rd);
    }
    return true;
}

bool ClangAstVisitor::TraverseCXXMethodDecl(clang::CXXMethodDecl *methodDecl)
{
    // Constructors not yet handled
    if(llvm::isa<clang::CXXConstructorDecl>(methodDecl))
        return true;
    // translation Manager will insert method in correct class
    OOModel::Method* method = trMngr_->insertMethodDecl(methodDecl);
    if(!method)
    {
        log_->writeError(className_,QString("no ooModel::method found"),QString("CXXMethodDecl"),methodDecl->getNameAsString());
        // for now return false to see error (interupts visitor)
        return false;
    }
    // only visit the body if we are at the definition
    if(methodDecl->isThisDeclarationADefinition())
    {
        ooStack_.push(method->items());
        TraverseStmt(methodDecl->getBody());
        ooStack_.pop();
    }
    return true;
}

bool ClangAstVisitor::TraverseIfStmt(clang::IfStmt *ifStmt)
{
    OOModel::IfStatement* ooIfStmt = new OOModel::IfStatement();
    OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
    if(itemList)
    {
        // append the if stmt to current stmt list
        itemList->append(ooIfStmt);
        // condition
        inBody_ = false;
        TraverseStmt(ifStmt->getCond());
        inBody_ = true;
        ooIfStmt->setCondition(ooExprStack_.pop());
        // then branch
        ooStack_.push(ooIfStmt->thenBranch());
        TraverseStmt(ifStmt->getThen());
        ooStack_.pop();
        // else branch
        ooStack_.push(ooIfStmt->elseBranch());
        TraverseStmt(ifStmt->getElse());
        ooStack_.pop();
    }
    return true;
}

bool ClangAstVisitor::TraverseWhileStmt(clang::WhileStmt* wStmt)
{
    OOModel::LoopStatement* ooLoop = new OOModel::LoopStatement();
    OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
    if(itemList)
    {
        // append the loop to current stmt list
        itemList->append(ooLoop);
        // condition
        inBody_ = false;
        TraverseStmt(wStmt->getCond());
        inBody_ = true;
        ooLoop->setCondition(ooExprStack_.pop());
        // body
        ooStack_.push(ooLoop->body());
        TraverseStmt(wStmt->getBody());
        ooStack_.pop();
    }
    return true;
}

bool ClangAstVisitor::TraverseForStmt(clang::ForStmt *fStmt)
{
    OOModel::LoopStatement* ooLoop = new OOModel::LoopStatement();
    OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
    if(itemList)
    {
        itemList->append(ooLoop);
        inBody_ = false;
        // init
        TraverseStmt(fStmt->getInit());
        ooLoop->setInitStep(ooExprStack_.pop());
        // condition
        TraverseStmt(fStmt->getCond());
        ooLoop->setCondition(ooExprStack_.pop());
        // update
        TraverseStmt(fStmt->getInc());
        ooLoop->setUpdateStep(ooExprStack_.pop());
        inBody_ = true;
        // body
        ooStack_.push(ooLoop->body());
        TraverseStmt(fStmt->getBody());
        ooStack_.pop();
    }
    return true;
}

bool ClangAstVisitor::TraverseReturnStmt(clang::ReturnStmt* rStmt)
{
    OOModel::ReturnStatement* ooReturn = new OOModel::ReturnStatement();
    OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
    if(itemList)
    {
        itemList->append(ooReturn);
        // return expression
        inBody_ = false;
        TraverseStmt(rStmt->getRetValue());
        inBody_ = true;
        ooReturn->values()->append(ooExprStack_.pop());
    }
    return true;
}

bool ClangAstVisitor::TraverseStmt(clang::Stmt *S)
{
    return Base::TraverseStmt(S);
}

bool ClangAstVisitor::VisitStmt(clang::Stmt* S)
{
    //    std::cout << "VISITING STMT" << std::endl;
    //    llvm::errs() << "VISITING STMT" << "\n";
        S->dump();
    return Base::VisitStmt(S);
}

bool ClangAstVisitor::TraverseVarDecl(clang::VarDecl* vd)
{
//    std::cout << "Visiting VarDecl " << vd->getNameAsString() <<std::endl;

    OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
    if(itemList)
    {
        OOModel::VariableDeclaration* varDecl = trMngr_->insertVar(vd);
        if(vd->getType().getTypePtr()->isArrayType())
        {
            const clang::ArrayType* arrType =  vd->getType().getTypePtr()->getAsArrayTypeUnsafe();
            if(arrType)
            {
                if(llvm::isa<clang::ConstantArrayType>(arrType))
                {
                    const clang::ConstantArrayType* constArr = llvm::dyn_cast<clang::ConstantArrayType>(arrType);
                    std::cout << "Const Array Size: " << constArr->getSize().getLimitedValue() << std::endl;
                }
            }
        }
        else
        {
            OOModel::Expression* type = CppImportUtilities::convertClangType(vd->getType());
            if(type) varDecl->setVarType(type);
        }

        if(vd->hasInit())
        {
            TraverseStmt(vd->getInit());
            varDecl->setInitialValue(ooExprStack_.pop());
        }
        if(inBody_)
            itemList->append(varDecl);
        else
            ooExprStack_.push(varDecl);
    }
    else
    {
        if(!llvm::isa<clang::ParmVarDecl>(vd))
            log_->writeWarning(className_,QString("this variable is not supported"),QString("VarDecl"),vd->getNameAsString());
    }
    return true;
}

bool ClangAstVisitor::VisitFieldDecl(clang::FieldDecl* fd)
{
    OOModel::Field* field = trMngr_->insertField(fd);
    if(!field)
    {
        log_->writeError(className_,QString("no parent found for this field"),QString("FieldDecl"),fd->getNameAsString());
        //        return false;
    }
    return true;
}

bool ClangAstVisitor::TraverseCXXMemberCallExpr(clang::CXXMemberCallExpr *cExpr)
{
    OOModel::MethodCallExpression* ooMCall = new OOModel::MethodCallExpression(QString::fromStdString(cExpr->getMethodDecl()->getNameAsString()));

    // visit arguments
    bool inBody = inBody_;
    inBody_ = false;
    clang::ExprIterator argIt = cExpr->arg_begin();
    for(;argIt!=cExpr->arg_end();++argIt)
    {
        TraverseStmt(*argIt);
        ooMCall->arguments()->append(ooExprStack_.pop());
    }
    inBody_ = inBody;
    if(inBody_)
    {
        OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
        if(itemList) itemList->append(ooMCall);
    }
    else
        ooExprStack_.push(ooMCall);
    return true;
}

bool ClangAstVisitor::VisitIntegerLiteral(clang::IntegerLiteral* intLit)
{
    OOModel::IntegerLiteral* ooIntLit = new OOModel::IntegerLiteral();
    ooIntLit->setValue(intLit->getValue().getLimitedValue());
    ooExprStack_.push(ooIntLit);
    return true;
}

bool ClangAstVisitor::VisitCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr* bExpr)
{
    OOModel::BooleanLiteral* ooBoolLit = new OOModel::BooleanLiteral();
    ooBoolLit->setValue(bExpr->getValue());
    ooExprStack_.push(ooBoolLit);
    return true;
}

bool ClangAstVisitor::VisitDeclRefExpr(clang::DeclRefExpr* declRef)
{
    OOModel::ReferenceExpression* refExpr = new OOModel::ReferenceExpression();
    refExpr->setName(QString::fromStdString(declRef->getNameInfo().getName().getAsString()));
    ooExprStack_.push(refExpr);
    return true;
}

bool ClangAstVisitor::shouldUseDataRecursionFor(clang::Stmt *S)
{
    // -unused var
    S->getStmtClass();
    return false;
}

bool ClangAstVisitor::TraverseBinaryOp(clang::BinaryOperator* binOp)
{
    OOModel::BinaryOperation::OperatorTypes ooOperatorType = CppImportUtilities::convertClangOpcode(binOp->getOpcode());
    OOModel::BinaryOperation* ooBinOp = new OOModel::BinaryOperation();
    // save inBody_ value for recursive expressions
    bool inBody = inBody_;
    inBody_ = false;
    ooBinOp->setOp(ooOperatorType);
    TraverseStmt(binOp->getLHS());
    ooBinOp->setLeft(ooExprStack_.pop());
    TraverseStmt(binOp->getRHS());
    ooBinOp->setRight(ooExprStack_.pop());

    if(inBody)
    {
        OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
        if(itemList) itemList->append(ooBinOp);
        else std::cout << "ERROR INSERT BINOP" << std::endl;
    }
    else
        ooExprStack_.push(ooBinOp);
    inBody_ = inBody;
    return true;
}

bool ClangAstVisitor::TraverseAssignment(clang::BinaryOperator *binOp)
{
    OOModel::AssignmentExpression::AssignmentTypes ooOperatorType = CppImportUtilities::convertClangAssignOpcode(binOp->getOpcode());
    OOModel::AssignmentExpression* ooBinOp = new OOModel::AssignmentExpression();
    // save inBody_ value for recursive expressions
    bool inBody = inBody_;
    inBody_ = false;
    ooBinOp->setOp(ooOperatorType);
    TraverseStmt(binOp->getLHS());
    ooBinOp->setLeft(ooExprStack_.pop());
    TraverseStmt(binOp->getRHS());
    ooBinOp->setRight(ooExprStack_.pop());

    if(inBody)
    {
        OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
        if(itemList) itemList->append(ooBinOp);
        else std::cout << "ERROR INSERT BINOP" << std::endl;
    }
    else
        ooExprStack_.push(ooBinOp);
    inBody_ = inBody;
    return true;
}

bool ClangAstVisitor::TraverseUnaryOp(clang::UnaryOperator *uOp)
{
    OOModel::UnaryOperation::OperatorTypes ooOperatorType = CppImportUtilities::convertUnaryOpcode(uOp->getOpcode());
    OOModel::UnaryOperation* ooUnaryOp = new OOModel::UnaryOperation();
    // save inBody_ value for recursive expressions
    bool inBody = inBody_;
    inBody_ = false;
    ooUnaryOp->setOp(ooOperatorType);
    TraverseStmt(uOp->getSubExpr());
    ooUnaryOp->setOperand(ooExprStack_.pop());

    if(inBody)
    {
        OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
        if(itemList) itemList->append(ooUnaryOp);
        else std::cout << "ERROR INSERT Unary" << std::endl;
    }
    else
        ooExprStack_.push(ooUnaryOp);
    inBody_ = inBody;
    return true;
}
