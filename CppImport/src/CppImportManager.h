#pragma once

#include "cppimport_api.h"
#include "ClangAstConsumer.h"
#include "ClangConsumerFactory.h"

class CppImportManager
{
public:
    CppImportManager(){}
    ~CppImportManager();
    bool setSrcPath(QString& path);
    void visualizeSrcFile();

protected:
    bool setCompilationDbPath(QString& path);
    clang::CompilerInstance compilerInstance_;
    clang::tooling::ClangTool* myTool_;
    clang::tooling::CompilationDatabase* compilationDB_;
    std::vector<std::string> sources_;

};
