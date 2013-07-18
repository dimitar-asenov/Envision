#include "TemplateArgumentVisitor.h"

namespace CppImport {

TemplateArgumentVisitor::TemplateArgumentVisitor
(ExpressionVisitor *vis, CppImportUtilities *util, CppImportLogger *log)
	: exprVisitor_{vis}, utils_{util}, log_{log}
{}

OOModel::FormalTypeArgument*TemplateArgumentVisitor::getLastTranslated()
{
	Q_ASSERT(!typeArgStack_.empty());
	return typeArgStack_.pop();
}

bool TemplateArgumentVisitor::TraverseDecl(clang::Decl* decl)
{
	if(!llvm::isa<clang::TemplateTypeParmDecl>(decl) && !llvm::isa<clang::NonTypeTemplateParmDecl>(decl))
	{
		log_->writeError(className_, decl, CppImportLogger::Reason::OTHER,
							  "Can not handle this decl with this visitor");
		typeArgStack_.push(new OOModel::FormalTypeArgument("#ERROR"));
		return true;
	}
	// dispatch to the correct function
	return RecursiveASTVisitor<TemplateArgumentVisitor>::TraverseDecl(decl);
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
	{
		exprVisitor_->TraverseStmt(nonTypeTemplateParm->getDefaultArgument());
		ooArg->setSuperTypeOfExpression(exprVisitor_->getLastExpression());
	}
	typeArgStack_.push(ooArg);
	return true;
}

}
