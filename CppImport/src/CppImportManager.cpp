#include "CppImportManager.h"

CppImportManager::~CppImportManager()
{
    delete compilationDB_;
    delete myTool_;
}

bool CppImportManager::setSrcPath(QString& path)
{
    QDir dir(path);
    // set a filter to only get files which are c++ sources
    QStringList cppFilter;
    cppFilter << "*.cpp" << "*.cc" << "*.cxx";
    dir.setNameFilters(cppFilter);
    dir.setFilter(QDir::Files);
    QStringList files = dir.entryList();
    // add all files found to sources vector
    foreach (QString file, files)
    {
        sources_.push_back(dir.absoluteFilePath(file).toStdString());
    }
    return setCompilationDbPath(path);
}

void CppImportManager::visualizeSrcFile()
{
    myTool_ = new clang::tooling::ClangTool(*compilationDB_,sources_);
    // run overtakes pointer so no need to free it later
    myTool_->run(clang::tooling::newFrontendActionFactory<ClangConsumerFactory>());
    ClangConsumerFactory::model_->endModification();
}

bool CppImportManager::setCompilationDbPath(QString& path)
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
