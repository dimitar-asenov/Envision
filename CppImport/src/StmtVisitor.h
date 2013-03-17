#pragma once

#include "cppimport_api.h"
#include "CppImportUtilities.h"

class StmtVisitor : public clang::RecursiveASTVisitor<StmtVisitor>
{
public:
    StmtVisitor();
    OOModel::StatementItemList* getItems() { return items_;}
    bool VisitVarDecl(clang::VarDecl* vd);
    bool VisitBinaryOperator(clang::BinaryOperator* binOp);
    bool VisitIntegerLiteral(clang::IntegerLiteral* intLit);
//    bool VisitDeclStmt(clang::DeclStmt* declStmt);

private:
    OOModel::StatementItemList* items_;
};
