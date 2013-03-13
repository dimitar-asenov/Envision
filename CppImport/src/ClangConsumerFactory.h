#pragma once

#include "cppimport_api.h"
#include "ClangAstConsumer.h"

class ClangConsumerFactory : public clang::ASTFrontendAction
{
public:
    ClangConsumerFactory();
//    static Model::Model* getModel() { return model_;}
    static Model::Model* model_;
    virtual clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override;

private:
    OOModel::Project* project_;
};

