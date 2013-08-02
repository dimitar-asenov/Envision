#include "TemplateArgumentVisitor.h"

namespace CppImport {

TemplateArgumentVisitor::TemplateArgumentVisitor
(ExpressionVisitor *vis, CppImportUtilities *util, CppImportLogger *log)
	: exprVisitor_{vis}, utils_{util}, log_{log}
{}

OOModel::FormalTypeArgument* TemplateArgumentVisitor::translateTemplateArg(clang::Decl* d)
{
	TraverseDecl(d);
	Q_ASSERT(!typeArgStack_.empty());
	return typeArgStack_.pop();
}

bool TemplateArgumentVisitor::VisitDecl(clang::Decl* decl)
{
	if(decl)
	{
		log_->writeError(className_, decl, CppImportLogger::Reason::OTHER,
							  "Can not handle this decl with this visitor");
		typeArgStack_.push(new OOModel::FormalTypeArgument("#ERROR"));
		return true;
	}
	return RecursiveASTVisitor<TemplateArgumentVisitor>::VisitDecl(decl);
}

bool TemplateArgumentVisitor::TraverseTemplateTypeParmDecl(clang::TemplateTypeParmDecl* templateParm)
{
	OOModel::FormalTypeArgument* ooArg = new OOModel::FormalTypeArgument
			(QString::fromStdString(templateParm->getNameAsString()));
	if(templateParm->hasDefaultArgument())
		ooArg->setSubTypeOfExpression(utils_->translateQualifiedType(templateParm->getDefaultArgument(),
																						 templateParm->getLocStart()));
	typeArgStack_.push(ooArg);
	return true;
}

bool TemplateArgumentVisitor::TraverseNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl* nonTypeTemplateParm)
{
	OOModel::FormalTypeArgument* ooArg = new OOModel::FormalTypeArgument
			(QString::fromStdString(nonTypeTemplateParm->getNameAsString()));
	ooArg->setSubTypeOfExpression(utils_->translateQualifiedType(nonTypeTemplateParm->getType(),
																					 nonTypeTemplateParm->getLocStart()));
	if(nonTypeTemplateParm->hasDefaultArgument())
		ooArg->setSuperTypeOfExpression(exprVisitor_->translateExpression(nonTypeTemplateParm->getDefaultArgument()));

	typeArgStack_.push(ooArg);
	return true;
}

}
