#pragma once

#include "ClangAstConsumer.h"

#include "OOModel/src/allOOModelNodes.h"
#include "clang/AST/ASTConsumer.h"

class ClangConsumerFactory
{
public:
    ClangConsumerFactory(Model::Model* model, OOModel::Project* project);
    clang::ASTConsumer* newASTConsumer();

private:
    Model::Model* model_;
    OOModel::Project* project_;
};

