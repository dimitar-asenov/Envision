#ifndef CLANGASTVISITOR_H
#define CLANGASTVISITOR_H

//ENVISION

#include "OOModel/src/allOOModelNodes.h"


//CLANG
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Type.h"


#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
//#include "clang/Rewrite/Rewriter.h"
//#include "clang/Rewrite/Rewriters.h"


//LLVM
#include "llvm/Support/Host.h"

using clang::RecursiveASTVisitor;

class ClangAstVisitor : public RecursiveASTVisitor <ClangAstVisitor>
{
public:
    ClangAstVisitor(Model::Model *model, OOModel::Project *currentProject);
    bool VisitStmt(clang::Stmt *S);
    bool VisitDecl(clang::Decl *D);

protected:
    OOModel::Project* currentProject_;
    Model::Model* currentModel_;

};

#endif // CLANGASTVISITOR_H
