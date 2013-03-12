#pragma once

#include "cppimport_api.h"
#include "ClangAstConsumer.h"
#include "HelperSingleton.h"

class ClangConsumerFactory : public clang::ASTFrontendAction
{
public:
    ClangConsumerFactory();
    ClangConsumerFactory(Model::Model* model, OOModel::Project* project);
//    clang::ASTConsumer* newASTConsumer();
    virtual clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override;

private:
    Model::Model* model_;
    OOModel::Project* project_;
};

