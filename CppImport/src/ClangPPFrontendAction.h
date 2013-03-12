#pragma once

#include "cppimport_api.h"
#include "ClangAstConsumer.h"

class ClangPPFrontendAction : public clang::ASTFrontendAction
{
public:
    ClangPPFrontendAction();
    ClangPPFrontendAction(Model::Model *model, OOModel::Project *project);
    virtual clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override;
//    virtual bool BeginSourceFileAction(clang::CompilerInstance &CI, llvm::StringRef Filename) override;
//    virtual void EndSourceFileAction() override;
//    virtual void ExecuteAction() override;
//    virtual bool usesPreprocessorOnly() const override;

private:
    Model::Model* model_;
    OOModel::Project* project_;
};

