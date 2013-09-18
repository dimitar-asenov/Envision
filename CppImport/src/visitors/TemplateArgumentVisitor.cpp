#include "TemplateArgumentVisitor.h"

namespace CppImport {

TemplateArgumentVisitor::TemplateArgumentVisitor
(ExpressionVisitor* vis, CppImportUtilities* util, CppImportLogger* log)
: exprVisitor_{vis}, utils_{util}, log_{log}
{}

OOModel::FormalTypeArgument* TemplateArgumentVisitor::translateTemplateArg(clang::Decl* d)
{
	// remove reference lastTranslatedArg
	lastTranslatedTypeArg_ = nullptr;
	TraverseDecl(d);
	Q_ASSERT(lastTranslatedTypeArg_);
	return lastTranslatedTypeArg_;
}

bool TemplateArgumentVisitor::VisitDecl(clang::Decl* decl)
{
	if(decl)
	{
		log_->writeError(className_, decl, CppImportLogger::Reason::OTHER,
							  "Can not handle this decl with this visitor");
		lastTranslatedTypeArg_ = new OOModel::FormalTypeArgument("#ERROR");
		return true;
	}
	return RecursiveASTVisitor<TemplateArgumentVisitor>::VisitDecl(decl);
}

bool TemplateArgumentVisitor::TraverseTemplateTypeParmDecl(clang::TemplateTypeParmDecl* templateParm)
{
	lastTranslatedTypeArg_ = new OOModel::FormalTypeArgument(QString::fromStdString(templateParm->getNameAsString()));
	if(templateParm->hasDefaultArgument())
		lastTranslatedTypeArg_->setSubTypeOfExpression(utils_->translateQualifiedType(templateParm->getDefaultArgument(),
																						 templateParm->getLocStart()));
	return true;
}

bool TemplateArgumentVisitor::TraverseNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl* nonTypeTemplateParm)
{
	lastTranslatedTypeArg_ = new OOModel::FormalTypeArgument
(QString::fromStdString(nonTypeTemplateParm->getNameAsString()));
	lastTranslatedTypeArg_->setSubTypeOfExpression(utils_->translateQualifiedType(nonTypeTemplateParm->getType(),
																					 nonTypeTemplateParm->getLocStart()));
	if(nonTypeTemplateParm->hasDefaultArgument())
		lastTranslatedTypeArg_->setSuperTypeOfExpression(exprVisitor_->translateExpression
																		 (nonTypeTemplateParm->getDefaultArgument()));
	return true;
}

}
