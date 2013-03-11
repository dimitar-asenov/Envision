#include "ClangPPCallbacks.h"

ClangPPCallbacks::ClangPPCallbacks()
{
}

void ClangPPCallbacks::FileChanged(clang::SourceLocation Loc, clang::PPCallbacks::FileChangeReason Reason, clang::SrcMgr::CharacteristicKind FileType, clang::FileID PrevFID)
{
    Loc.isFileID();
    PrevFID.isInvalid();
    std::cout << "FILE CHANGED REASON " << Reason << "  FILETYPE " << FileType << std::endl;
}

void ClangPPCallbacks::InclusionDirective(clang::SourceLocation HashLoc, const clang::Token &IncludeTok, llvm::StringRef FileName, bool IsAngled, clang::CharSourceRange FilenameRange, const clang::FileEntry *File, llvm::StringRef SearchPath, llvm::StringRef RelativePath, const clang::Module *Imported)
{
    HashLoc.isFileID();
    IncludeTok.getFlags();
    File->getDir();
    Imported->getFullModuleName();
    FilenameRange.getBegin();
    SearchPath.str();

    std::cout << "INCLUDE DIRECTIVE " << RelativePath.str() << "   " << FileName.str() << " IS ANGLED " << IsAngled <<   std::endl;
}
