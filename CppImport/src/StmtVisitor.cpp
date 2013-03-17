#include "StmtVisitor.h"

StmtVisitor::StmtVisitor()
{
    items_ = new OOModel::StatementItemList();
}

bool StmtVisitor::VisitVarDecl(clang::VarDecl *vd)
{
    OOModel::VariableDeclaration* varDecl = new OOModel::VariableDeclaration();
    varDecl->setName(QString::fromStdString(vd->getName().str()));

    OOModel::Expression* type = CppImportUtilities::convertClangType(vd->getType());
    if(type) varDecl->setVarType(type);

//    trMngr_->insertVar(vd,varDecl);

    items_->append(varDecl);
    return true;
}

bool StmtVisitor::VisitBinaryOperator(clang::BinaryOperator *binOp)
{
    OOModel::BinaryOperation::OperatorTypes type = CppImportUtilities::convertClangOpcode(binOp->getOpcode());
    std::cout << "TYPE          BIN OP : " << type << std::endl;
    return true;
}

bool StmtVisitor::VisitIntegerLiteral(clang::IntegerLiteral *intLit)
{
    intLit->getValue();
    return true;
}
