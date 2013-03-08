#include "CppImportUtilities.h"

using namespace OOModel;

CppImportUtilities::CppImportUtilities()
{
}

OOModel::Expression *CppImportUtilities::convertClangType(clang::QualType type)
{
    if(type.getTypePtr()->isIntegerType())
        return new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT);
    else if(type.getTypePtr()->isBooleanType())
        return new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN);
    else if(type.getTypePtr()->isVoidType())
        return new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID);
    else
        return nullptr;
}
