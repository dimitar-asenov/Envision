#include "cppimportmanager.h"

CppImportManager::CppImportManager()
{
    this->testModel_ = new Model::Model();
    this->testProject_ = dynamic_cast<OOModel::Project*> (testModel_->createRoot("Project"));
    testModel_->beginModification(testProject_, "Adding a project");
    testProject_->setName("NewProject");
    testModel_->endModification();

    //CLANG SETUP
    std::vector<std::string> Sources;
    Sources.push_back("/home/luke/BachelorThesis/Envision/CppImport/test.cpp");
    std::string Error = "DATABASE NOT OK";
    clang::tooling::CompilationDatabase* compDB = NULL;
    compDB = clang::tooling::CompilationDatabase::loadFromDirectory("/home/luke/BachelorThesis/Envision/CppImport",Error);

    if(!compDB)
        std::cout << "ERROR NO COMPDB" << std::endl;
    myTool_ = new clang::tooling::ClangTool(*compDB,Sources);
}

void CppImportManager::importSrcFile(QString fileName)
{
    fileName.at(1);
}

Model::Model* CppImportManager::getModel()
{
    return this->testModel_;
}

OOModel::Project* CppImportManager::getProject()
{
    return this->testProject_;
}

void CppImportManager::parseFile()
{
    //    clang::tooling::runToolOnCode(new ClangAstFrontendActions(testModel_,testProject_), "class Test{};");
    //    ClangAstFrontendActions *cafa = new ClangAstFrontendActions(testModel_,testProject_);
    //    clang::tooling::FrontendActionFactory* frontendActFact = clang::tooling::newFrontendActionFactory<ClangAstFrontendActions>();
    ////    frontendActFact->
    ClangConsumerFactory* myFac = new ClangConsumerFactory(testModel_,testProject_);
    myTool_->run(clang::tooling::newFrontendActionFactory<ClangConsumerFactory>(myFac));
}
