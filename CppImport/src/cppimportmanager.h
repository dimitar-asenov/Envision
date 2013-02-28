#ifndef CPPIMPORTMANAGER_H
#define CPPIMPORTMANAGER_H

//ENVISION - CPPIMPORT
#include "clangastconsumer.h"

//ENVISION
#include "OOModel/src/allOOModelNodes.h"

//CLANG
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/LangOptions.h"

#include "clang/Lex/HeaderSearch.h"

#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInvocation.h"

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/Builtins.h"

//LLVM
//#include "llvm/ADT/IntrusiveRefCntPtr.h"

class CppImportManager
{
public:
    CppImportManager();
    void importSrcFile(QString fileName);
    Model::Model *getModel();
    OOModel::Project *getProject();
    void parseFile();


protected:
    Model::Model *testModel_;
    OOModel::Project *testProject_;
    clang::CompilerInstance compilerInstance_;
    ClangAstConsumer *astConsumer_;
    clang::ASTContext *astContext_;

};

#endif // CPPIMPORTMANAGER_H
