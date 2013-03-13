#pragma once

#include "cppimport_api.h"
#include "ClangAstConsumer.h"

class ClangConsumerFactory : public clang::ASTFrontendAction
{
public:
    ClangConsumerFactory();
    static Model::Model* getModel() { return model_;}
    virtual clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override;

private:
    static Model::Model* model_;
    static OOModel::Project* project_;
};

