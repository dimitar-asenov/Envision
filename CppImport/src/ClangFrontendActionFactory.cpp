#include "ClangFrontendActionFactory.h"

#include "ClangConsumerCreator.h"

#include "manager/TranslateManager.h"

namespace CppImport {

ClangFrontendActionFactory::ClangFrontendActionFactory(OOModel::Project* project, TranslateManager* manager, CppImportLogger* log)
	: logger_{log}
{
	visitor_ = new ClangAstVisitor(project, logger_, manager);
}

ClangFrontendActionFactory::~ClangFrontendActionFactory()
{
	SAFE_DELETE(visitor_);
}

clang::FrontendAction* ClangFrontendActionFactory::create()
{
	return new ClangConsumerCreator(new ClangAstConsumer(logger_, visitor_));
}

}
