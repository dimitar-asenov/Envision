#include "ClangConsumerFactory.h"

ClangConsumerFactory::ClangConsumerFactory()
{
    this->model_ = new Model::Model();
    this->project_ = dynamic_cast<OOModel::Project*> (model_->createRoot("Project"));
    model_->beginModification(project_, "Adding a project");
    project_->setName("NewProject");
    model_->endModification();
}

clang::ASTConsumer *ClangConsumerFactory::CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile)
{
    std::cout << "PROCESSING FILE " << InFile.str() << std::endl;
    return new ClangAstConsumer(&CI,model_,project_);
}
