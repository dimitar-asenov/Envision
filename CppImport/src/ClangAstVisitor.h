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
//    bool TraverseBinMul(clang::BinaryOperator* binOp);
//    bool TraverseBinDiv(clang::BinaryOperator* binOp);
//    bool TraverseBinRem(clang::BinaryOperator* binOp);
    bool TraverseBinAdd(clang::BinaryOperator* binOp);
//    bool TraverseBinSub(clang::BinaryOperator* binOp);
    bool TraverseBinLT(clang::BinaryOperator* binOp);
    bool TraverseBinGT(clang::BinaryOperator* binOp);
    bool TraverseBinLE(clang::BinaryOperator* binOp);
    bool TraverseBinGE(clang::BinaryOperator* binOp);
    bool TraverseBinEQ(clang::BinaryOperator* binOp);
    bool TraverseBinNE(clang::BinaryOperator* binOp);
//    bool TraverseBinAnd(clang::BinaryOperator* binOp);
//    bool TraverseBinXor(clang::BinaryOperator* binOp);
//    bool TraverseBinOr(clang::BinaryOperator* binOp);
//     binary assigns
    bool TraverseBinAssign(clang::BinaryOperator* binOp);
//    bool TraverseBinMulAssign(clang::CompoundAssignOperator* binOp);
//    bool TraverseBinDivAssign(clang::CompoundAssignOperator* binOp);
//    bool TraverseBinRemAssign(clang::CompoundAssignOperator* binOp);
//    bool TraverseBinAddAssign(clang::CompoundAssignOperator* binOp);
//    bool TraverseBinSubAssign(clang::CompoundAssignOperator* binOp);
//    bool TraverseBinShlAssign(clang::CompoundAssignOperator* binOp);
//    bool TraverseBinShrAssign(clang::CompoundAssignOperator* binOp);
//    bool TraverseBinAndAssign(clang::CompoundAssignOperator* binOp);
//    bool TraverseBinXorAssign(clang::CompoundAssignOperator* binOp);
//    bool TraverseBinOrAssign(clang::CompoundAssignOperator* binOp);


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




// binary ops
//    inline bool ClangAstVisitor::TraverseBinMul(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
//    inline bool ClangAstVisitor::TraverseBinDiv(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
//    inline bool ClangAstVisitor::TraverseBinRem(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinAdd(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
//    inline bool ClangAstVisitor::TraverseBinSub(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinLT(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinGT(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinLE(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinGE(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinEQ(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
inline bool ClangAstVisitor::TraverseBinNE(clang::BinaryOperator* binOp)
{return TraverseBinaryOp(binOp);}
//    inline bool ClangAstVisitor::TraverseBinAnd(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
//    inline bool ClangAstVisitor::TraverseBinXor(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
//    inline bool ClangAstVisitor::TraverseBinOr(clang::BinaryOperator* binOp)
//    {return TraverseBinaryOp(binOp);}
// binary assigns
inline bool ClangAstVisitor::TraverseBinAssign(clang::BinaryOperator* binOp)
{return TraverseAssignment(binOp);}
//    inline bool ClangAstVisitor::TraverseBinMulAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    inline bool ClangAstVisitor::TraverseBinDivAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    inline bool ClangAstVisitor::TraverseBinRemAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    inline bool ClangAstVisitor::TraverseBinAddAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    inline bool ClangAstVisitor::TraverseBinSubAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    inline bool ClangAstVisitor::TraverseBinShlAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    inline bool ClangAstVisitor::TraverseBinShrAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    inline bool ClangAstVisitor::TraverseBinAndAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    inline bool ClangAstVisitor::TraverseBinXorAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
//    inline bool ClangAstVisitor::TraverseBinOrAssign(clang::CompoundAssignOperator* binOp)
//    {return TraverseAssignment(binOp);}
