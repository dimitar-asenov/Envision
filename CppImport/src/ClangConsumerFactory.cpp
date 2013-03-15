#include "ClangConsumerFactory.h"

ClangConsumerFactory::ClangConsumerFactory()
{
    model_ = new Model::Model();
    project_ = dynamic_cast<OOModel::Project*> (model_->createRoot("Project"));
    model_->beginModification(project_, "Adding a project");
    project_->setName("NewProject");
}

clang::ASTConsumer *ClangConsumerFactory::CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile)
{
    std::cout << "PROCESSING FILE " << InFile.str() << std::endl;
    return new ClangAstConsumer(&CI,model_,project_);
}

Model::Model* ClangConsumerFactory::model_;
