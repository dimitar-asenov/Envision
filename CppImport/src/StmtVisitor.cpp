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
