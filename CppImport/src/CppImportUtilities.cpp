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

OOModel::BinaryOperation::OperatorTypes CppImportUtilities::convertClangOpcode(clang::BinaryOperatorKind kind)
{
    switch (kind) {
    case clang::BO_EQ:
        return OOModel::BinaryOperation::EQUALS;
        break;
    case clang::BO_Add:
        return OOModel::BinaryOperation::PLUS;
    default:
        return OOModel::BinaryOperation::GREATER;
        break;
    }
}
