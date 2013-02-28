#include "clangastvisitor.h"



using namespace clang;
using namespace llvm;

bool ClangAstVisitor::VisitStmt(clang::Stmt *S)
{
    if(isa<IfStmt>(S))
    {

    }
    return true;
}



bool ClangAstVisitor::VisitDecl(clang::Decl *D)
{
    if(isa<RecordDecl>(D))
    {
//        RecordDecl *recDecl = cast<RecordDecl>(D);

    }

    return true;
}
