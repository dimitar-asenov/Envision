#include "cppimportmanager.h"

CppImportManager::CppImportManager()
{
    this->testModel_ = new Model::Model();
    this->testProject_ = dynamic_cast<OOModel::Project*> (testModel_->createRoot("Project"));
    testModel_->beginModification(testProject_, "Adding a project");
    testProject_->setName("NewProject");
    testModel_->endModification();
}

void CppImportManager::setSrcFile(QString fileName)
{
    sources_.push_back(fileName.toStdString());
}

bool CppImportManager::setCompilationDbPath(QString path)
{
    std::string Error = "DATABASE NOT OK";
    compilationDB_ = nullptr;
    compilationDB_ = clang::tooling::CompilationDatabase::loadFromDirectory(path.toAscii().data(),Error);

    if(!compilationDB_)
    {
        std::cout << "ERROR NO COMPDB" << std::endl;
        return false;
    }
    return true;
}

void CppImportManager::visualizeSrcFile()
{
    myTool_ = new clang::tooling::ClangTool(*compilationDB_,sources_);
    ClangConsumerFactory* myFac = new ClangConsumerFactory(testModel_,testProject_);
    myTool_->run(clang::tooling::newFrontendActionFactory<ClangConsumerFactory>(myFac));
}

Model::Model* CppImportManager::getModel()
{
    return this->testModel_;
}

OOModel::Project* CppImportManager::getProject()
{
    return this->testProject_;
}
