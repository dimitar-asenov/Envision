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

ClangAstVisitor::ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject = nullptr) :
    currentModel_(model) , currentProject_(currentProject)
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
        std::cout << "ERROR UNKNOWN WHERE TO PUT NAMESPACE" << std::endl;

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
    if(rd->isClass())
    {
        OOModel::Class* ooClass = trMngr_->insertClass(rd);
        // check if there was an error inserting class
        if(!ooClass) return false;

        // insert in model
        if(OOModel::Project* curProject = dynamic_cast<OOModel::Project*>(ooStack_.top()))
            curProject->modules()->append(ooClass);
        else if(OOModel::Module* curModel = dynamic_cast<OOModel::Module*>(ooStack_.top()))
            curModel->modules()->append(ooClass);
        //TODO: support inner classes in OOModel::Class*
//        else if(OOModel::Class* curClass = dynamic_cast<OOModel::Class*>(ooStack_.top()))
//            curClass->
        else
            std::cout << "ERROR UNKNOWN WHERE TO PUT CLASS" << std::endl;
        // visit child decls
        ooStack_.push(ooClass);
        clang::DeclContext::decl_iterator it = rd->decls_begin();
        for(;it!=rd->decls_end();++it)
        {
            TraverseDecl(*it);
        }
        ooStack_.pop();
    }
    else if(rd->isStruct())
    {
        std::cout << "Struct found " << rd->getNameAsString() << std::endl;
        return Base::TraverseCXXRecordDecl(rd);
    }
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
        std::cout << "___________ERROR NO OOMODEL::METHOD FOR THIS DECL_______" << std::endl;
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
        itemList->append(ooIfStmt);
        inBody_ = false;
        TraverseStmt(ifStmt->getCond());
        inBody_ = true;
        ooIfStmt->setCondition(ooExprStack_.pop());
        ooStack_.push(ooIfStmt->thenBranch());
        TraverseStmt(ifStmt->getThen());
        ooStack_.pop();
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
        itemList->append(ooLoop);
        inBody_ = false;
        TraverseStmt(wStmt->getCond());
        inBody_ = true;
        ooLoop->setCondition(ooExprStack_.pop());
        ooStack_.push(ooLoop->body());
        TraverseStmt(wStmt->getBody());
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
    //    S->dump();
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
        itemList->append(varDecl);
    }
    else
    {
        if(!llvm::isa<clang::ParmVarDecl>(vd))
            std::cout << "--->WARNING : THIS VARIABLE IS NOT SUPPORTED : " << vd->getNameAsString() << std::endl;
    }
    return true;
}

bool ClangAstVisitor::VisitFieldDecl(clang::FieldDecl* fd)
{
    OOModel::Field* field = trMngr_->insertField(fd);
    if(!field)
    {
        std::cout << "ERROR COULDN'T INSERT FIELD NO CURRENT OOCLASS" << std::endl;
        //        return false;
    }
    return true;
}

bool ClangAstVisitor::VisitIntegerLiteral(clang::IntegerLiteral* intLit)
{
    OOModel::IntegerLiteral* ooIntLit = new OOModel::IntegerLiteral();
    ooIntLit->setValue(intLit->getValue().getLimitedValue());
    ooExprStack_.push(ooIntLit);

    return true;
}

bool ClangAstVisitor::VisitDeclRefExpr(clang::DeclRefExpr* declRef)
{
    OOModel::ReferenceExpression* refExpr = new OOModel::ReferenceExpression();
    //    if(llvm::isa<clang::VarDecl>(declRef->getDecl()))
    //    {
    //        OOModel::VariableDeclaration* ooVar = trMngr_->getVar(llvm::dyn_cast<clang::VarDecl>(declRef->getDecl()));
    //        if(ooVar) refExpr->setPrefix(ooVar);
    //        else refExpr->setName(QString::fromStdString(declRef->getNameInfo().getName().getAsString()));
    //    }
    //    else
    //    {
    refExpr->setName(QString::fromStdString(declRef->getNameInfo().getName().getAsString()));
    //    }
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
