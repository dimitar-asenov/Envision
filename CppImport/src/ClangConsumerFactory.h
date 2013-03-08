#pragma once

#include "cppimport_api.h"
#include "ClangAstConsumer.h"

class ClangConsumerFactory
{
public:
    ClangConsumerFactory(Model::Model* model, OOModel::Project* project);
    clang::ASTConsumer* newASTConsumer();

private:
    Model::Model* model_;
    OOModel::Project* project_;
};

