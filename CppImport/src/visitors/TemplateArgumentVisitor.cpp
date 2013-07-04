#include "TemplateArgumentVisitor.h"

namespace CppImport {

TemplateArgumentVisitor::TemplateArgumentVisitor
(ExpressionVisitor *vis, CppImportUtilities *util, CppImportLogger *log)
	: exprVisitor_{vis}, utils_{util}, log_{log}
{}

OOModel::FormalTypeArgument*TemplateArgumentVisitor::getLastTranslated()
{
	if(!typeArgStack_.empty())
		return typeArgStack_.pop();
	// TODO:
//	log_->writeError(className_, "Could not convert");
	return new OOModel::FormalTypeArgument("#ERROR");
}

bool TemplateArgumentVisitor::TraverseTemplateTypeParmDecl(clang::TemplateTypeParmDecl* templateParm)
{
	OOModel::FormalTypeArgument* ooArg = new OOModel::FormalTypeArgument
			(QString::fromStdString(templateParm->getNameAsString()));
	if(templateParm->hasDefaultArgument())
		ooArg->setSubTypeOfExpression(utils_->translateQualifiedType(templateParm->getDefaultArgument()));
	typeArgStack_.push(ooArg);
	return true;
}

bool TemplateArgumentVisitor::TraverseNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl* nonTypeTemplateParm)
{
	OOModel::FormalTypeArgument* ooArg = new OOModel::FormalTypeArgument
			(QString::fromStdString(nonTypeTemplateParm->getNameAsString()));
	ooArg->setSubTypeOfExpression(utils_->translateQualifiedType(nonTypeTemplateParm->getType()));
	if(nonTypeTemplateParm->hasDefaultArgument())
	{
		exprVisitor_->TraverseStmt(nonTypeTemplateParm->getDefaultArgument());
		ooArg->setSuperTypeOfExpression(exprVisitor_->getLastExpression());
	}
	typeArgStack_.push(ooArg);
	return true;
}

}
