#include "ClangConsumerFactory.h"

ClangConsumerFactory::ClangConsumerFactory(Model::Model *model, OOModel::Project *project)
    : model_(model) , project_(project)
{
}

clang::ASTConsumer* ClangConsumerFactory::newASTConsumer()
{
    return new ClangAstConsumer(model_,project_);
}
