#include "ClangFrontendActionFactory.h"

#include "ClangConsumerFactory.h"

namespace CppImport {

ClangFrontendActionFactory::ClangFrontendActionFactory(Model::Model* model)
: model_(model)
{
	project_ = dynamic_cast<OOModel::Project*> (model_->createRoot("Project"));
	model_->beginModification(project_, "Adding a project");
	project_->setName("NewProject");


	logger_ = new CppImportLogger();
	visitor_ = new ClangAstVisitor(model_, project_, logger_);
	consumer_ = new ClangAstConsumer(logger_, visitor_);
	consumerFactory_ = new ClangConsumerFactory(consumer_);
}

ClangFrontendActionFactory::~ClangFrontendActionFactory()
{
	SAFE_DELETE(consumer_);
}

clang::FrontendAction* ClangFrontendActionFactory::create()
{
	return new ClangConsumerFactory(new ClangAstConsumer(logger_, visitor_));;
}

}
