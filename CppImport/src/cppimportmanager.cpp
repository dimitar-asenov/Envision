#include "cppimportmanager.h"

CppImportManager::CppImportManager()
{
    this->testModel_ = new Model::Model();
    this->testProject_ = dynamic_cast<OOModel::Project*> (testModel_->createRoot("Project"));
    testModel_->beginModification(testProject_, "Adding a project");
        testProject_->setName("NewProject");
    testModel_->endModification();
}

void CppImportManager::importSrcFile(QString fileName)
{
    fileName.at(1);
}

Model::Model *CppImportManager::getModel()
{
    return this->testModel_;
}

OOModel::Project *CppImportManager::getProject()
{
    return this->testProject_;
}

void CppImportManager::parseFile()
{
    clang::tooling::runToolOnCode(new ClangAstFrontendActions(testModel_,testProject_), "class Test{};");
}
