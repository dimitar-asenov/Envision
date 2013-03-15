#include "CppImportUtilities.h"

OOModel::Expression *CppImportUtilities::convertClangType(clang::QualType type)
{
    if(type.getTypePtr()->isBooleanType())
        return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN);
    else if(type.getTypePtr()->isIntegerType())
        return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::INT);
    else if(type.getTypePtr()->isVoidType())
        return new OOModel::PrimitiveTypeExpression(OOModel::PrimitiveTypeExpression::PrimitiveTypes::VOID);
    else
        return nullptr;
}
