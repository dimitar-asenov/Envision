#pragma once

#include "OOModel/src/allOOModelNodes.h"
#include "clang/AST/Type.h"

class CppImportUtilities
{
public:
    CppImportUtilities();
    static OOModel::Expression* convertClangType(clang::QualType type);
};
