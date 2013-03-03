#include "clangastconsumer.h"


using namespace clang;

ClangAstConsumer::ClangAstConsumer(Model::Model *model, OOModel::Project *currentProject)
{
    this->astVisitor_ = new ClangAstVisitor(model, currentProject);
}

bool ClangAstConsumer::HandleTopLevelDecl(DeclGroupRef D)
{
    DeclGroupRef::iterator iterator = D.begin();

    for (; iterator != D.end(); ++iterator)
    {
//        astVisitor_->TraverseDecl(*iterator);
    }
    return true; // keep going
}

void ClangAstConsumer::HandleTranslationUnit(ASTContext &Context)
{
    astVisitor_->TraverseDecl(Context.getTranslationUnitDecl());
}
