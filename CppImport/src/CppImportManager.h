#pragma once

#include "cppimport_api.h"
#include "ClangAstConsumer.h"
#include "ClangConsumerFactory.h"

class CppImportManager
{
public:
    CppImportManager(){}
    ~CppImportManager();
    void setSrcFile(QString fileName);
    bool setCompilationDbPath(QString path);
    void visualizeSrcFile();

protected:
    clang::CompilerInstance compilerInstance_;
    clang::tooling::ClangTool* myTool_;
    clang::tooling::CompilationDatabase* compilationDB_;
    std::vector<std::string> sources_;

};
