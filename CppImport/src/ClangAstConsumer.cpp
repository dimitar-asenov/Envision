#include "ClangAstConsumer.h"


ClangAstConsumer::ClangAstConsumer(Model::Model *model, OOModel::Project *currentProject)
{
    this->astVisitor_ = new ClangAstVisitor(model, currentProject);
}

bool ClangAstConsumer::HandleTopLevelDecl(clang::DeclGroupRef D)
{
    clang::DeclGroupRef::iterator iterator = D.begin();

    for (; iterator != D.end(); ++iterator)
    {
        clang::RawCommentList cList = (*iterator)->getASTContext().getRawCommentList();
       // clang::RawCommentList cList = astContext->getRawCommentList();
        cList.getComments();
        astVisitor_->TraverseDecl(*iterator);
    }
    return true; // keep going
}

void ClangAstConsumer::HandleTranslationUnit(clang::ASTContext &Context)
{
    clang::RawCommentList cList = Context.getRawCommentList();
    cList.getComments();
    Context.getTranslationUnitDecl();
    // astVisitor_->TraverseDecl(Context.getTranslationUnitDecl());
}
