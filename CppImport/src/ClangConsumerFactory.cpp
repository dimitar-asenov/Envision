#include "ClangConsumerFactory.h"

ClangConsumerFactory::ClangConsumerFactory()
{
    model_  = HelperSingleton::getInstance().getModel();
    project_ = HelperSingleton::getInstance().getProject();
}

ClangConsumerFactory::ClangConsumerFactory(Model::Model *model, OOModel::Project *project)
    : model_(model) , project_(project)
{
}

//clang::ASTConsumer* ClangConsumerFactory::newASTConsumer()
//{
//    return new ClangAstConsumer(model_,project_);
//}

clang::ASTConsumer *ClangConsumerFactory::CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile)
{
    std::cout << "PROCESSING FILE " << InFile.str() << std::endl;
    return new ClangAstConsumer(&CI,model_,project_);
}
