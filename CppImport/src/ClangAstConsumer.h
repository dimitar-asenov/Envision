#pragma once


#include "cppimport_api.h"
#include "ClangAstVisitor.h"
#include "ClangPPCallbacks.h"

class ClangAstConsumer : public clang::ASTConsumer
{
public:
    ClangAstConsumer(Model::Model* model, OOModel::Project* currentProject = nullptr);
    ClangAstConsumer(clang::CompilerInstance* ci, Model::Model* model, OOModel::Project* currentProject = nullptr);
    virtual bool HandleTopLevelDecl(clang::DeclGroupRef D) override;
    virtual void HandleTranslationUnit(clang::ASTContext &Context) override;
    virtual void Initialize(clang::ASTContext &Context) override;

protected:
    ClangAstVisitor* astVisitor_;
    clang::CompilerInstance* ci_;

};
