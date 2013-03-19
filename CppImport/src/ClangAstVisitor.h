#pragma once

#include "cppimport_api.h"
#include "CppImportUtilities.h"
#include "TranslateManager.h"

class ClangAstVisitor : public clang::RecursiveASTVisitor <ClangAstVisitor>
{
    typedef clang::RecursiveASTVisitor<ClangAstVisitor> Base;
public:
    ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject);
    ~ClangAstVisitor();

    bool TraverseCXXRecordDecl(clang::CXXRecordDecl* rd);
    bool TraverseCXXMethodDecl(clang::CXXMethodDecl* methodDecl);
    bool TraverseIfStmt(clang::IfStmt* ifStmt);
    bool TraverseWhileStmt(clang::WhileStmt* wStmt);
    bool TraverseReturnStmt(clang::ReturnStmt* rStmt);

    bool TraverseStmt(clang::Stmt *S);
    bool TraverseVarDecl(clang::VarDecl* vd);


    // binary ops
//    bool TraverseBinMul(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
//    bool TraverseBinDiv(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
//    bool TraverseBinRem(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
    bool TraverseBinAdd(clang::BinaryOperator* binOp)
    {return TraverseBinaryOp(binOp);}
//    bool TraverseBinSub(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
    bool TraverseBinAdd(clang::BinaryOperator* binOp)
    {return TraverseBinaryOp(binOp);}
//    bool TraverseBinSub(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
    bool TraverseBinLT(clang::BinaryOperator* binOp)
    {return TraverseBinaryOp(binOp);}
    bool TraverseBinGT(clang::BinaryOperator* binOp)
    {return TraverseBinaryOp(binOp);}
    bool TraverseBinLE(clang::BinaryOperator* binOp)
    {return TraverseBinaryOp(binOp);}
    bool TraverseBinGE(clang::BinaryOperator* binOp)
    {return TraverseBinaryOp(binOp);}
    bool TraverseBinEQ(clang::BinaryOperator* binOp)
    {return TraverseBinaryOp(binOp);}
    bool TraverseBinNE(clang::BinaryOperator* binOp)
    {return TraverseBinaryOp(binOp);}
//    bool TraverseBinAnd(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
//    bool TraverseBinXor(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
//    bool TraverseBinOr(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
    // binary assigns
    bool TraverseBinAssign(clang::BinaryOperator* binOp)
    {return TraverseAssignment(binOp);}
//    bool TraverseBinMulAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    bool TraverseBinDivAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    bool TraverseBinRemAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    bool TraverseBinAddAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    bool TraverseBinSubAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    bool TraverseBinShlAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    bool TraverseBinShrAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    bool TraverseBinAndAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    bool TraverseBinXorAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    bool TraverseBinOrAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}


    bool VisitStmt(clang::Stmt* S);
    bool VisitFieldDecl(clang::FieldDecl* fd);

    bool VisitIntegerLiteral(clang::IntegerLiteral* intLit);
    bool VisitDeclRefExpr(clang::DeclRefExpr* declRef);

    bool shouldUseDataRecursionFor(clang::Stmt* S);
private:
    bool TraverseBinaryOp(clang::BinaryOperator* binOp);
    bool TraverseAssignment(clang::BinaryOperator* binOp);

    QStack<Model::Node*> ooStack{};
    QStack<OOModel::Expression*> ooExprStack{};

    Model::Model* currentModel_{};
    OOModel::Project* currentProject_{};
    clang::ASTContext* astContext_{};
    TranslateManager* trMngr_{};
    bool inBody_{false};
};
