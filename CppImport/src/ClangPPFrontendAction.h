#pragma once

#include "ClangAstConsumer.h"
#include "OOModel/src/allOOModelNodes.h"

#include "clang/Rewrite/Frontend/FrontendActions.h"

class ClangPPFrontendAction : public clang::PreprocessorFrontendAction
{
public:
    ClangPPFrontendAction();
    ClangPPFrontendAction(Model::Model *model, OOModel::Project *project);
    virtual clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override;
    virtual bool BeginSourceFileAction(clang::CompilerInstance &CI, llvm::StringRef Filename) override;
    virtual void ExecuteAction() override;

private:
    Model::Model* model_;
    OOModel::Project* project_;
};

