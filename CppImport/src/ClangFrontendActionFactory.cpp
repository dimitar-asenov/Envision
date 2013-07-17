#include "ClangFrontendActionFactory.h"

#include "ClangConsumerCreator.h"

#include "manager/TranslateManager.h"

namespace CppImport {

ClangFrontendActionFactory::ClangFrontendActionFactory(OOModel::Project* project, TranslateManager* manager)
{
	logger_ = new CppImportLogger();
	visitor_ = new ClangAstVisitor(project, logger_, manager);
}

ClangFrontendActionFactory::~ClangFrontendActionFactory()
{
	SAFE_DELETE(logger_);
	SAFE_DELETE(visitor_);
}

clang::FrontendAction* ClangFrontendActionFactory::create()
{
	return new ClangConsumerCreator(new ClangAstConsumer(logger_, visitor_));
}

void ClangFrontendActionFactory::outputStatistics()
{
	logger_->outputStatistics();
}

}
