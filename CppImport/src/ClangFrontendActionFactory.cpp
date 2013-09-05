#include "ClangFrontendActionFactory.h"

#include "TranslateFrontendAction.h"

#include "manager/TranslateManager.h"

namespace CppImport {

ClangFrontendActionFactory::ClangFrontendActionFactory(ClangAstVisitor* visitor, CppImportLogger* log)
: visitor_{visitor}, log_{log}
{}

clang::FrontendAction* ClangFrontendActionFactory::create()
{
	return new TranslateFrontendAction(visitor_, log_);
}

}
