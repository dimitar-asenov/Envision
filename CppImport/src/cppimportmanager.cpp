#include "cppimportmanager.h"

CppImportManager::CppImportManager()
{
    using clang::CompilerInstance;
    using clang::TargetOptions;
    using clang::TargetInfo;

    //setup model and project
    this->testModel_ = new Model::Model();
    this->testProject_ = dynamic_cast<OOModel::Project*> (testModel_->createRoot("Project"));
    testModel_->beginModification(testProject_, "Adding a project");
        testProject_->setName("NewProject");
    testModel_->endModification();


    //set up compileInstance
    compilerInstance_.createDiagnostics(0,NULL);
    clang::LangOptions langOpts = compilerInstance_.getLangOpts();
    langOpts.CPlusPlus = 1;

    compilerInstance_.getInvocation().setLangDefaults(langOpts,clang::IK_CXX,clang::LangStandard::lang_cxx03);
    const char *arg = "ccc clang cxx";
    clang::CompilerInvocation::CreateFromArgs(compilerInstance_.getInvocation(), &arg, &arg+1,
                                              compilerInstance_.getDiagnostics());



    llvm::IntrusiveRefCntPtr<TargetOptions> pto( new TargetOptions());
    pto->Triple = llvm::sys::getDefaultTargetTriple();
    llvm::IntrusiveRefCntPtr<TargetInfo> pti(TargetInfo::CreateTargetInfo(compilerInstance_.getDiagnostics(), *pto));
    compilerInstance_.setTarget(pti.getPtr());



    compilerInstance_.createFileManager();
    compilerInstance_.createSourceManager(compilerInstance_.getFileManager());



    //setup preprocessor



//    clang::CompilerInvocation *compilerInvocation = new clang::CompilerInvocation();

//    compilerInstance_.setInvocation(compilerInvocation);
//    astUnit_ = clang::ASTUnit::LoadFromCompilerInvocation(compilerInvocation,&compilerInstance_.getDiagnostics());


    compilerInstance_.createPreprocessor();

    //setup astConsumer
    this->astConsumer_ = new ClangAstConsumer(this->testModel_,this->testProject_);
    compilerInstance_.setASTConsumer(astConsumer_);

    compilerInstance_.createASTContext();
//    clang::IdentifierTable idTable(langOpts);
//    clang::SelectorTable sTable;
//    clang::Builtin::Context context;
//    astContext_ = new clang::ASTContext(langOpts,compilerInstance_.getSourceManager(),
//                                 &compilerInstance_.getTarget(),
//                                 idTable,
//                                 sTable,
//                                 context,0,true);
//    compilerInstance_.setASTContext(astContext_);

}

void CppImportManager::importSrcFile(QString fileName)
{
    using clang::FileEntry;
    const FileEntry *pFile = compilerInstance_.getFileManager().getFile(fileName.toStdString());
    compilerInstance_.getSourceManager().createMainFileID(pFile);
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
    compilerInstance_.getPreprocessor().EnterMainSourceFile();
    compilerInstance_.getDiagnosticClient().BeginSourceFile(compilerInstance_.getLangOpts(),
                                             &compilerInstance_.getPreprocessor());
    clang::ParseAST(compilerInstance_.getPreprocessor(), astConsumer_, compilerInstance_.getASTContext());
    compilerInstance_.getDiagnosticClient().EndSourceFile();
}
