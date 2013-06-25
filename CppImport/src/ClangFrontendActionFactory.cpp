#include "ClangFrontendActionFactory.h"

#include "ClangConsumerFactory.h"

namespace CppImport {

ClangFrontendActionFactory::ClangFrontendActionFactory(OOModel::Project* project)
{
	logger_ = new CppImportLogger();
	visitor_ = new ClangAstVisitor(project, logger_);
}

ClangFrontendActionFactory::~ClangFrontendActionFactory()
{
	SAFE_DELETE(logger_);
	SAFE_DELETE(visitor_);
}

clang::FrontendAction* ClangFrontendActionFactory::create()
{
	return new ClangConsumerFactory(new ClangAstConsumer(logger_, visitor_));
}

void ClangFrontendActionFactory::outputStatistics()
{
	logger_->outputStatistics();
}

}
