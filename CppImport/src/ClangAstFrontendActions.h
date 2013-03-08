#pragma once


#include "cppimport_api.h"
#include "ClangAstConsumer.h"

class ClangAstFrontendActions : public clang::ASTFrontendAction
{
public:
    ClangAstFrontendActions(Model::Model* model, OOModel::Project* project);
    virtual clang::ASTConsumer *CreateASTConsumer(
      clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        InFile.str();
        Compiler.getASTConsumer();
      return new ClangAstConsumer(model_,project_);
    }

private:
    Model::Model* model_;
    OOModel::Project* project_;
};
