#ifndef CLANGASTCONSUMER_H
#define CLANGASTCONSUMER_H

//ENVISION - CPPIMPORT
#include "ClangAstVisitor.h"

//ENVISION
#include "OOModel/src/allOOModelNodes.h"

//CLANG
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"

class ClangAstConsumer : public clang::ASTConsumer
{
public:
    ClangAstConsumer(Model::Model* model, OOModel::Project* currentProject = nullptr);
    virtual bool HandleTopLevelDecl(clang::DeclGroupRef D) override;
    virtual void HandleTranslationUnit(clang::ASTContext &Context) override;

protected:
    ClangAstVisitor* astVisitor_;

};

#endif // CLANGASTCONSUMER_H
