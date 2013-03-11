#ifndef CLANGPPCALLBACKS_H
#define CLANGPPCALLBACKS_H

#include "cppimport_api.h"

class ClangPPCallbacks : public clang::PPCallbacks
{
public:
    ClangPPCallbacks();
    virtual void FileChanged(clang::SourceLocation Loc, clang::PPCallbacks::FileChangeReason Reason, clang::SrcMgr::CharacteristicKind FileType, clang::FileID PrevFID) override;
    virtual void InclusionDirective (clang::SourceLocation HashLoc, const clang::Token &IncludeTok, llvm::StringRef FileName, bool IsAngled, clang::CharSourceRange FilenameRange, const clang::FileEntry *File, llvm::StringRef SearchPath, llvm::StringRef RelativePath, const clang::Module *Imported) override;
};

#endif // CLANGPPCALLBACKS_H
