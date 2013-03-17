#pragma once

#include "cppimport_api.h"

class CppImportUtilities
{
public:
    CppImportUtilities() {}
    static OOModel::Expression* convertClangType(clang::QualType type);
    static OOModel::BinaryOperation::OperatorTypes convertClangOpcode(clang::BinaryOperatorKind kind);
};
