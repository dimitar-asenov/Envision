#pragma once

#include "cppimport_api.h"
#include "ClangAstConsumer.h"
#include "ClangAstFrontendActions.h"
#include "ClangConsumerFactory.h"
#include "ClangPPFrontendAction.h"
#include "HelperSingleton.h"

class CppImportManager
{
public:
    CppImportManager();
    void setSrcFile(QString fileName);
    bool setCompilationDbPath(QString path);
    void visualizeSrcFile();
    Model::Model* getModel();
    OOModel::Project* getProject();

protected:
    Model::Model* testModel_;
    OOModel::Project* testProject_;
    clang::CompilerInstance compilerInstance_;
    ClangAstConsumer* astConsumer_;
    clang::ASTContext* astContext_;
    clang::tooling::ClangTool* myTool_;
    clang::tooling::CompilationDatabase* compilationDB_;
    std::vector<std::string> sources_;

};
