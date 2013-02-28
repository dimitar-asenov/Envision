#include "clangastconsumer.h"

ClangAstConsumer::ClangAstConsumer(Model::Model *model, OOModel::Project *currentProject)
{
    this->astVisitor_ = new ClangAstVisitor(model,currentProject);
}
