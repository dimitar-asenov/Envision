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
    case clang::BO_Mul:
        return OOModel::BinaryOperation::TIMES;
    case clang::BO_Div:
        return OOModel::BinaryOperation::DIVIDE;
    case clang::BO_Rem:
        return OOModel::BinaryOperation::REMAINDER;
    case clang::BO_Add:
        return OOModel::BinaryOperation::PLUS;
    case clang::BO_Sub:
        return OOModel::BinaryOperation::MINUS;
    case clang::BO_LT:
        return OOModel::BinaryOperation::LESS;
    case clang::BO_GT:
        return OOModel::BinaryOperation::GREATER;
    case clang::BO_LE:
        return OOModel::BinaryOperation::LESS_EQUALS;
    case clang::BO_GE:
        return OOModel::BinaryOperation::GREATER_EQUALS;
    case clang::BO_EQ:
        return OOModel::BinaryOperation::EQUALS;
    case clang::BO_NE:
        return OOModel::BinaryOperation::NOT_EQUALS;
    case clang::BO_And:
        return OOModel::BinaryOperation::AND;
    case clang::BO_Xor:
        return OOModel::BinaryOperation::XOR;
    case clang::BO_Or:
        return OOModel::BinaryOperation::OR;
    default:
        return OOModel::BinaryOperation::GREATER;
    }
}

OOModel::AssignmentExpression::AssignmentTypes CppImportUtilities::convertClangAssignOpcode(clang::BinaryOperatorKind kind)
{
    switch (kind) {
    case clang::BO_Assign:
        return OOModel::AssignmentExpression::ASSIGN;
    case clang::BO_MulAssign:
        return OOModel::AssignmentExpression::TIMES_ASSIGN;
    case clang::BO_DivAssign:
        return OOModel::AssignmentExpression::DIVIDE_ASSIGN;
    case clang::BO_RemAssign:
        return OOModel::AssignmentExpression::REMAINDER_ASSIGN;
    case clang::BO_AddAssign:
        return OOModel::AssignmentExpression::PLUS_ASSIGN;
    case clang::BO_SubAssign:
        return OOModel::AssignmentExpression::MINUS_ASSIGN;
    case clang::BO_ShlAssign:
        return OOModel::AssignmentExpression::LEFT_SHIFT_ASSIGN;
    case clang::BO_ShrAssign:
        return OOModel::AssignmentExpression::RIGHT_SHIFT_SIGNED_ASSIGN;
    case clang::BO_AndAssign:
        return OOModel::AssignmentExpression::BIT_AND_ASSIGN;
    case clang::BO_XorAssign:
        return OOModel::AssignmentExpression::BIT_XOR_ASSIGN;
    case clang::BO_OrAssign:
        return OOModel::AssignmentExpression::BIT_OR_ASSIGN;
    default:
        return OOModel::AssignmentExpression::ASSIGN;
    }
}
