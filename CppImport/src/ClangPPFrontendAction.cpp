#include "ClangPPFrontendAction.h"

ClangPPFrontendAction::ClangPPFrontendAction()
{
    this->model_ = new Model::Model();
    this->project_ = dynamic_cast<OOModel::Project*> (model_->createRoot("Project"));
    model_->beginModification(project_, "Adding a project");
    project_->setName("NewProject");
    model_->endModification();
}

ClangPPFrontendAction::ClangPPFrontendAction(Model::Model* model, OOModel::Project* project) :
    model_(model), project_(project)
{
}

clang::ASTConsumer *ClangPPFrontendAction::CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile)
{
    InFile.str();
//    CI.getASTConsumer();
    CI.hasASTConsumer();
    return new ClangAstConsumer(&CI,model_,project_);
}

//bool ClangPPFrontendAction::BeginSourceFileAction(clang::CompilerInstance &CI, llvm::StringRef Filename)
//{
//    Filename.str();
//    CI.hasASTConsumer();
////    CI.getASTConsumer();
//    std::cout << "BEGIN SOURCE FILE ACTION - HAS PP: " << CI.hasPreprocessor() << std::endl;
//    return true;
//}

//void ClangPPFrontendAction::EndSourceFileAction()
//{
//    std::cout << "END SOURCE FILE ACTION  " << std::endl;
//}

//void ClangPPFrontendAction::ExecuteAction()
//{
//    std::cout << "EXECUTE ACTION " << std::endl;
//}

//bool ClangPPFrontendAction::usesPreprocessorOnly() const
//{
//    return false;
//}
