/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/

#include "ClangAstConsumer.h"


ClangAstConsumer::ClangAstConsumer(Model::Model *model, OOModel::Project *currentProject) :
    clang::ASTConsumer()
{
    this->astVisitor_ = new ClangAstVisitor(model, currentProject, logger_);
}

ClangAstConsumer::ClangAstConsumer(clang::CompilerInstance *ci, Model::Model *model, OOModel::Project *currentProject) :
    clang::ASTConsumer(), ci_(ci)
{
    if(ci)
        ci->getPreprocessor().enableIncrementalProcessing();
    this->astVisitor_ = new ClangAstVisitor(model, currentProject, logger_);
}

ClangAstConsumer::~ClangAstConsumer()
{
    delete astVisitor_;
    delete logger_;
}

//bool ClangAstConsumer::HandleTopLevelDecl(clang::DeclGroupRef D)
//{
//    clang::DeclGroupRef::iterator iterator = D.begin();

//        for (; iterator != D.end(); ++iterator)
//        {
////            clang::RawCommentList cList = (*iterator)->getASTContext().getRawCommentList();
////           // clang::RawCommentList cList = astContext->getRawCommentList();
////            cList.getComments();
////            astVisitor_->TraverseDecl(*iterator);
//        }
//    return true; // keep going
//}

void ClangAstConsumer::HandleTranslationUnit(clang::ASTContext &Context)
{
    clang::RawCommentList cList = Context.getRawCommentList();
    cList.getComments();
    Context.getTranslationUnitDecl();
    astVisitor_->TraverseDecl(Context.getTranslationUnitDecl());
}

void ClangAstConsumer::Initialize(clang::ASTContext &Context)
{
    Context.getRawCommentList();
    if(ci_)
    {
        ci_->getPreprocessor().addPPCallbacks(new ClangPPCallbacks());
    }
}
