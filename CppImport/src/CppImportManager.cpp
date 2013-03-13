#include "CppImportManager.h"

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
    myTool_->run(clang::tooling::newFrontendActionFactory<ClangConsumerFactory>());
}
