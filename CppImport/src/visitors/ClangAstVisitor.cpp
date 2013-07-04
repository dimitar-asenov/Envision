/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/

#include "ClangAstVisitor.h"
#include "ExpressionVisitor.h"
#include "../CppImportUtilities.h"
#include "TemplateArgumentVisitor.h"

namespace CppImport {

ClangAstVisitor::ClangAstVisitor(OOModel::Project* project, CppImportLogger* logger)
:  log_(logger)
{
	exprVisitor_ = new ExpressionVisitor(this, log_);
	utils_ = new CppImportUtilities(log_, exprVisitor_);
	exprVisitor_->setUtilities(utils_);
	trMngr_ = new TranslateManager(utils_);
	templArgVisitor_ = new TemplateArgumentVisitor(exprVisitor_, utils_, log_);
	ooStack_.push(project);
}

ClangAstVisitor::~ClangAstVisitor()
{
	SAFE_DELETE(trMngr_);
	SAFE_DELETE(utils_);
	SAFE_DELETE(exprVisitor_);
}

void ClangAstVisitor::setSourceManager(const clang::SourceManager* sourceManager)
{
	Q_ASSERT(sourceManager);
	sourceManager_ = sourceManager;
	trMngr_->setSourceManager(sourceManager);
}

Model::Node*ClangAstVisitor::ooStackTop()
{
	return ooStack_.top();
}

void ClangAstVisitor::pushOOStack(Model::Node* node)
{
	Q_ASSERT(node);
	ooStack_.push(node);
}

Model::Node*ClangAstVisitor::popOOStack()
{
	return ooStack_.pop();
}

bool ClangAstVisitor::TraverseNamespaceDecl(clang::NamespaceDecl* namespaceDecl)
{
	if(!shouldModel(namespaceDecl->getLocation()))
		return true;

	OOModel::Module* ooModule = nullptr;
	// insert it in model
	if(OOModel::Project* curProject = dynamic_cast<OOModel::Project*>(ooStack_.top()))
	{
		ooModule = trMngr_->insertNamespace(namespaceDecl, ooStack_.size()-1);
		// if the module is not yet in the list add it
		if(!curProject->modules()->contains(ooModule))
			curProject->modules()->append(ooModule);
	}
	else if(OOModel::Module* curModel = dynamic_cast<OOModel::Module*>(ooStack_.top()))
	{
		ooModule = trMngr_->insertNamespace(namespaceDecl, ooStack_.size());
		// check if the namespace is already the one which is active
		if(curModel == ooModule)
			return true;
		// if the module is not yet in the list add it
		if(!curModel->modules()->contains(ooModule))
			curModel->modules()->append(ooModule);
	}
	else
	{
		log_->writeError(className_, "uknown where to put namespace", namespaceDecl);
		// this is a severe error which should not happen therefore stop visiting
		return false;
	}
	// visit the body of the namespace
	ooStack_.push(ooModule);
	clang::DeclContext::decl_iterator it = namespaceDecl->decls_begin();
	for(;it!=namespaceDecl->decls_end();++it)
	{
		TraverseDecl(*it);
	}
	ooStack_.pop();
	return true;
}

bool ClangAstVisitor::TraverseClassTemplateDecl(clang::ClassTemplateDecl* classTemplate)
{
	if(!shouldModel(classTemplate->getLocation()) || !classTemplate->isThisDeclarationADefinition())
		return true;

	clang::CXXRecordDecl* recordDecl = classTemplate->getTemplatedDecl();
	if(auto ooClass = createClass(recordDecl))
	{
		if(!trMngr_->insertClassTemplate(classTemplate, ooClass))
			// this class is already visited
			return true;
		TraverseClass(recordDecl, ooClass);
		// visit type arguments if any
		auto templateParamList = classTemplate->getTemplateParameters();
		for( auto templateParam = templateParamList->begin();
			  templateParam != templateParamList->end(); ++templateParam)
		{
			templArgVisitor_->TraverseDecl(*templateParam);
			ooClass->typeArguments()->append(templArgVisitor_->getLastTranslated());
		}
	}
	return true;
}

bool ClangAstVisitor::TraverseClassTemplateSpecializationDecl
(clang::ClassTemplateSpecializationDecl* specializationDecl)
{
	if(!shouldModel(specializationDecl->getLocation()) || !specializationDecl->isThisDeclarationADefinition())
		return true;

	clang::CXXRecordDecl* recordDecl = specializationDecl->getSpecializedTemplate()->getTemplatedDecl();
	if(auto ooClass = createClass(recordDecl))
	{
		if(!trMngr_->insertClassTemplateSpec(specializationDecl, ooClass))
			// this class is already visited
			return true;
		TraverseClass(recordDecl, ooClass);
		// visit type arguments if any
		for(unsigned i = 0; i < specializationDecl->getTemplateArgs().size(); i++)
			ooClass->typeArguments()->append(new OOModel::FormalTypeArgument
					("#spec", utils_->translateTemplateArgument(specializationDecl->getTemplateArgs().get(i))));
	}
	return true;
}

bool ClangAstVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl* recordDecl)
{
	if(!shouldModel(recordDecl->getLocation()) || !recordDecl->isThisDeclarationADefinition())
		return true;

	if(auto ooClass = createClass(recordDecl))
	{
		if(!trMngr_->insertClass(recordDecl,ooClass))
			// this class is already visited
			return true;
		TraverseClass(recordDecl, ooClass);
		// visit type arguments if any
		if(auto describedTemplate = recordDecl->getDescribedClassTemplate())
		{
			auto templateParamList = describedTemplate->getTemplateParameters();
			for( auto templateParam = templateParamList->begin();
				  templateParam != templateParamList->end(); ++templateParam)
			{
				templArgVisitor_->TraverseDecl(*templateParam);
				ooClass->typeArguments()->append(templArgVisitor_->getLastTranslated());
			}
		}
	}
	else
		log_->writeError(className_, "Unsupported RecordDecl", recordDecl);
	return true;
}

bool ClangAstVisitor::TraverseFunctionDecl(clang::FunctionDecl* functionDecl)
{
	if(!shouldModel(functionDecl->getLocation()) || llvm::isa<clang::CXXMethodDecl>(functionDecl))
		return true;

	if(auto ooFunction = trMngr_->insertFunctionDecl(functionDecl))
	{
		if(ooFunction->parent())
			// this function has already been inserted to the model
			return true;

		// insert in model
		if(OOModel::Project* curProject = dynamic_cast<OOModel::Project*>(ooStack_.top()))
			curProject->methods()->append(ooFunction);
		else if(OOModel::Module* curModel = dynamic_cast<OOModel::Module*>(ooStack_.top()))
			curModel->methods()->append(ooFunction);
		else if(OOModel::Class* curClass = dynamic_cast<OOModel::Class*>(ooStack_.top()))
		{
			log_->writeError(className_,"friend function with definition is not supported", functionDecl);
			curClass->methods()->append(ooFunction);
		}
		else
			log_->writeError(className_, "uknown where to put function", functionDecl);
		// handle body, typeargs and storage specifier
		TraverseFunction(functionDecl, ooFunction);
	}
	else
		log_->writeError(className_, "could not insert function", functionDecl);
	return true;
}

bool ClangAstVisitor::TraverseIfStmt(clang::IfStmt* ifStmt)
{
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
	{
		OOModel::IfStatement* ooIfStmt = new OOModel::IfStatement();
		// append the if stmt to current stmt list
		itemList->append(ooIfStmt);
		// condition
		bool inBody = inBody_;
		inBody_ = false;
		if(auto varDecl = ifStmt->getConditionVariable())
			TraverseDecl(varDecl);
		else
			TraverseStmt(ifStmt->getCond());
		ooIfStmt->setCondition(ooExprStack_.pop());
		inBody_ = true;
		// then branch
		ooStack_.push(ooIfStmt->thenBranch());
		TraverseStmt(ifStmt->getThen());
		ooStack_.pop();
		// else branch
		ooStack_.push(ooIfStmt->elseBranch());
		TraverseStmt(ifStmt->getElse());
		inBody_ = inBody;
		ooStack_.pop();
	}
	else
		log_->writeError(className_, "Uknown where to put if stmt", ifStmt);
	return true;
}

bool ClangAstVisitor::TraverseWhileStmt(clang::WhileStmt* whileStmt)
{
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
	{
		OOModel::LoopStatement* ooLoop = new OOModel::LoopStatement();
		// append the loop to current stmt list
		itemList->append(ooLoop);
		// condition
		bool inBody = inBody_;
		inBody_ = false;
		if(auto varDecl = whileStmt->getConditionVariable())
			TraverseDecl(varDecl);
		else
			TraverseStmt(whileStmt->getCond());
		inBody_ = true;
		ooLoop->setCondition(ooExprStack_.pop());
		// body
		ooStack_.push(ooLoop->body());
		TraverseStmt(whileStmt->getBody());
		inBody_ = inBody;
		ooStack_.pop();
	}
	else
		log_->writeError(className_, "Uknown where to put while stmt", whileStmt);
	return true;
}

bool ClangAstVisitor::TraverseDoStmt(clang::DoStmt* doStmt)
{
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
	{
		OOModel::LoopStatement* ooLoop = new OOModel::LoopStatement();
		ooLoop->setLoopKind(OOModel::LoopStatement::LoopKind::PostCheck);
		// append the loop to current stmt list
		itemList->append(ooLoop);
		// condition
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(doStmt->getCond());
		ooLoop->setCondition(ooExprStack_.pop());
		// body
		inBody_ = true;
		ooStack_.push(ooLoop->body());
		TraverseStmt(doStmt->getBody());
		inBody_ = inBody;
		ooStack_.pop();
	}
	else
		log_->writeError(className_, "Uknown where to put do while stmt", doStmt);
	return true;
}

bool ClangAstVisitor::TraverseForStmt(clang::ForStmt* forStmt)
{
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
	{
		OOModel::LoopStatement* ooLoop = new OOModel::LoopStatement();
		itemList->append(ooLoop);
		bool inBody = inBody_;
		inBody_ = false;
		// init
		TraverseStmt(forStmt->getInit());
		if(!ooExprStack_.empty())
		ooLoop->setInitStep(ooExprStack_.pop());
		// condition
		TraverseStmt(forStmt->getCond());
		if(!ooExprStack_.empty())
		ooLoop->setCondition(ooExprStack_.pop());
		// update
		TraverseStmt(forStmt->getInc());
		if(!ooExprStack_.empty())
		ooLoop->setUpdateStep(ooExprStack_.pop());
		inBody_ = true;
		// body
		ooStack_.push(ooLoop->body());
		TraverseStmt(forStmt->getBody());
		inBody_ = inBody;
		ooStack_.pop();
	}
	else
		log_->writeError(className_, "Uknown where to put for stmt", forStmt);
	return true;
}

bool ClangAstVisitor::TraverseCXXForRangeStmt(clang::CXXForRangeStmt* forRangeStmt)
{
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
	{
		OOModel::ForEachStatement* ooLoop = new OOModel::ForEachStatement();
		const clang::VarDecl* loopVar = forRangeStmt->getLoopVariable();
		itemList->append(ooLoop);
		ooLoop->setVarName(QString::fromStdString(loopVar->getNameAsString()));
		ooLoop->setVarType(utils_->translateQualifiedType(loopVar->getType()));
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(forRangeStmt->getRangeInit());
		if(!ooExprStack_.empty())
			ooLoop->setCollection(ooExprStack_.pop());
		inBody_ = true;
		//body
		ooStack_.push(ooLoop->body());
		TraverseStmt(forRangeStmt->getBody());
		ooStack_.pop();
		inBody_ = inBody;
	}
	else
		log_->writeError(className_, "Uknown where to put forRange stmt", forRangeStmt);
	return true;
}

bool ClangAstVisitor::TraverseSwitchStmt(clang::SwitchStmt* switchStmt)
{
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
	{
		OOModel::SwitchStatement* ooSwitchStmt = new OOModel::SwitchStatement();
		itemList->append(ooSwitchStmt);
		// save inbody var
		bool inBody = inBody_;
		inBody_ = false;
		// traverse condition
		if(auto varDecl = switchStmt->getConditionVariable())
			TraverseDecl(varDecl);
		else
			TraverseStmt(switchStmt->getCond());
		if(!ooExprStack_.empty())
			ooSwitchStmt->setSwitchVar(ooExprStack_.pop());
		// traverse the cases
		clang::SwitchCase* switchCase = switchStmt->getSwitchCaseList();
		TraverseStmt(switchCase);
		if(!ooSwitchCaseStack_.empty())
			ooSwitchStmt->cases()->append(ooSwitchCaseStack_.pop());
		while(auto nextCase = switchCase->getNextSwitchCase())
		{
			TraverseStmt(nextCase);
			if(!ooSwitchCaseStack_.empty())
				// need to prepend because clang visits in reverse order
				ooSwitchStmt->cases()->prepend(ooSwitchCaseStack_.pop());
			switchCase = nextCase;
		}
		// restore inbody var
		inBody_ = inBody;
	}
	else
		log_->writeError(className_, "Uknown where to put switch stmt", switchStmt);
	return true;
}

bool ClangAstVisitor::TraverseReturnStmt(clang::ReturnStmt* returnStmt)
{
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
	{
		OOModel::ReturnStatement* ooReturn = new OOModel::ReturnStatement();
		itemList->append(ooReturn);
		// return expression
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(returnStmt->getRetValue());
		if(!ooExprStack_.empty())
			ooReturn->values()->append(ooExprStack_.pop());
		else
			log_->writeError(className_, "Return expr not supported", returnStmt->getRetValue());
		inBody_ = inBody;
	}
	else
		log_->writeError(className_, "Uknown where to put return stmt", returnStmt);
	return true;
}

bool ClangAstVisitor::TraverseDeclStmt(clang::DeclStmt* declStmt)
{
	if(inBody_)
	{
		for(auto declIt = declStmt->decl_begin(); declIt != declStmt->decl_end(); ++declIt)
			TraverseDecl(*declIt);
		return true;
	}
	if(!declStmt->isSingleDecl())
	{
		log_->writeWarning(className_, "Decl Stmt with multiple decl needs inspection", declStmt);
		OOModel::CommaExpression* ooComma = new OOModel::CommaExpression();
		// TODO: fix this section
		bool inBody = inBody_;
		inBody_ = false;
		auto declIt = declStmt->decl_begin();
		TraverseDecl(*declIt);
		if(!ooExprStack_.empty())
			ooComma->setLeft(ooExprStack_.pop());
		if(++declIt != declStmt->decl_end())
		{
			TraverseDecl(*declIt);
			if(!ooExprStack_.empty())
				ooComma->setRight(ooExprStack_.pop());
		}

		if(!(inBody_ = inBody))
			ooExprStack_.push(ooComma);
		else
			log_->writeError(className_,"uknown where to put ", declStmt);
		return true;
	}
	return TraverseDecl(declStmt->getSingleDecl());
}

bool ClangAstVisitor::TraverseCXXTryStmt(clang::CXXTryStmt* tryStmt)
{
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
	{
		OOModel::TryCatchFinallyStatement* ooTry = new OOModel::TryCatchFinallyStatement();
		itemList->append(ooTry);
		bool inBody = inBody_;
		inBody_ = true;
		// visit the body
		ooStack_.push(ooTry->tryBody());
		TraverseStmt(tryStmt->getTryBlock());
		ooStack_.pop();
		// visit catch blocks
		unsigned end = tryStmt->getNumHandlers();
		for(unsigned i = 0; i < end; i++)
		{
			TraverseStmt(tryStmt->getHandler(i));
			ooTry->catchClauses()->append(ooStack_.pop());
		}
		inBody_ = inBody;
	}
	else
		log_->writeError(className_, "Uknown where to put try stmt", tryStmt);
	return true;
}

bool ClangAstVisitor::TraverseCXXCatchStmt(clang::CXXCatchStmt* catchStmt)
{
	OOModel::CatchClause* ooCatch = new OOModel::CatchClause();
	// save inBody var
	bool inBody = inBody_;
	inBody_ = false;
	// visit exception to catch
	if(catchStmt->getExceptionDecl())
	{
		TraverseDecl(catchStmt->getExceptionDecl());
		if(!ooExprStack_.empty())
			ooCatch->setExceptionToCatch(ooExprStack_.pop());
	}
	// visit catch body
	inBody_ = true;
	ooStack_.push(ooCatch->body());
	TraverseStmt(catchStmt->getHandlerBlock());
	ooStack_.pop();
	// finish up
	inBody_ = inBody;
	ooStack_.push(ooCatch);
	return true;
}

bool ClangAstVisitor::TraverseStmt(clang::Stmt* S)
{
	if(S && llvm::isa<clang::Expr>(S))
	{
		// always ignore implicit stuff
		auto casted = llvm::dyn_cast<clang::Expr>(S);
		bool ret = exprVisitor_->TraverseStmt(casted->IgnoreImplicit());
		if(auto expr = exprVisitor_->getLastExpression())
		{
			if(!inBody_)
				ooExprStack_.push(expr);
			else if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
				itemList->append(new OOModel::ExpressionStatement(expr));
		}
		else
		{
			ooExprStack_.push(utils_->createErrorExpression("Could not convert expr"));
			log_->writeError(className_, "exprvisitor couldn't convert", S);
		}
		return ret;
	}

	return Base::TraverseStmt(S);
}

bool ClangAstVisitor::TraverseVarDecl(clang::VarDecl* varDecl)
{
	if(!shouldModel(varDecl->getLocation()))
		return true;

	if(llvm::isa<clang::ParmVarDecl>(varDecl))
		return true;
	OOModel::VariableDeclaration* ooVarDecl = nullptr;
	OOModel::VariableDeclarationExpression* ooVarDeclExpr = nullptr;
	QString varName = QString::fromStdString(varDecl->getNameAsString());

	if(!inBody_)
	{
		ooVarDecl = new OOModel::VariableDeclaration(varName);
		ooVarDeclExpr = new OOModel::VariableDeclarationExpression(ooVarDecl);
		ooExprStack_.push(ooVarDeclExpr);
	}
	else if(auto project = dynamic_cast<OOModel::Project*>(ooStack_.top()))
	{
		ooVarDecl = new OOModel::Field(varName);
		project->fields()->append(ooVarDecl);
	}
	else if(auto module = dynamic_cast<OOModel::Module*>(ooStack_.top()))
	{
		ooVarDecl = new OOModel::Field(varName);
		module->fields()->append(ooVarDecl);
	}
	else if(auto ooClass = dynamic_cast<OOModel::Class*>(ooStack_.top()))
	{
		ooVarDecl = new OOModel::Field(varName);
		ooClass->fields()->append(ooVarDecl);
	}
	else if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
	{
		ooVarDecl = new OOModel::VariableDeclaration(varName);
		// TODO: remove variabledeclaration expression?
		itemList->append(new OOModel::ExpressionStatement(new OOModel::VariableDeclarationExpression(ooVarDecl)));
	}
	else
	{
		log_->writeWarning(className_, "this variable is not supported", varDecl);
		return true;
	}

	// set the type
	ooVarDecl->setTypeExpression(utils_->translateQualifiedType(varDecl->getType()));

	if(varDecl->hasInit())
	{
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(varDecl->getInit()->IgnoreImplicit());
		if(!ooExprStack_.empty())
		{
			// make sure we have not ourself as init (if init couldn't be converted)
			if(ooVarDeclExpr != ooExprStack_.top())
				ooVarDecl->setInitialValue(ooExprStack_.pop());
		}
		else
			log_->writeError(className_, "Var Init Expr not supported", varDecl->getInit());
		inBody_ = inBody;
	}
	// modifiers
	ooVarDecl->modifiers()->set(utils_->translateStorageSpecifier(varDecl->getStorageClass()));

	return true;
}

bool ClangAstVisitor::TraverseEnumDecl(clang::EnumDecl* enumDecl)
{
	if(!shouldModel(enumDecl->getLocation()))
		return true;

	OOModel::Class* ooEnumClass = new OOModel::Class
			(QString::fromStdString(enumDecl->getNameAsString()), OOModel::Class::ConstructKind::Enum);
	// insert in model
	if(OOModel::Project* curProject = dynamic_cast<OOModel::Project*>(ooStack_.top()))
		curProject->classes()->append(ooEnumClass);
	else if(OOModel::Module* curModel = dynamic_cast<OOModel::Module*>(ooStack_.top()))
		curModel->classes()->append(ooEnumClass);
	else if(OOModel::Class* curClass = dynamic_cast<OOModel::Class*>(ooStack_.top()))
		curClass->classes()->append(ooEnumClass);
	else if(OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
		itemList->append(new OOModel::DeclarationStatement(ooEnumClass));
	else
	{
		log_->writeWarning(className_, "Unknown where to put Enum", enumDecl);
		// no need to further process this enum
		return true;
	}
	clang::EnumDecl::enumerator_iterator it = enumDecl->enumerator_begin();
	bool inBody = inBody_;
	inBody_ = false;
	for(;it!=enumDecl->enumerator_end();++it)
	{
		// check if there is an initializing expression if so visit it first and then add it to the enum
		if(auto e = it->getInitExpr())
		{
			TraverseStmt(e);
			ooEnumClass->enumerators()->append(new OOModel::Enumerator
														  (QString::fromStdString(it->getNameAsString()),ooExprStack_.pop()));
		}
		else
			ooEnumClass->enumerators()->append(new OOModel::Enumerator(QString::fromStdString(it->getNameAsString())));
	}
	inBody_ = inBody;
	return true;
}

bool ClangAstVisitor::TraverseFieldDecl(clang::FieldDecl* fieldDecl)
{
	if(!shouldModel(fieldDecl->getLocation()))
		return true;
	// TODO: field implementation is missing a lot of things
	OOModel::Field* field = trMngr_->insertField(fieldDecl);
	if(!field)
	{
		log_->writeError(className_, "no parent found for this field", fieldDecl);
		// TODO
		// return false;
		return true;
	}
	if(fieldDecl->hasInClassInitializer())
	{
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(fieldDecl->getInClassInitializer());
		if(!ooExprStack_.empty())
			field->setInitialValue(ooExprStack_.pop());
		else
			log_->writeError(className_, "Could not translate init", fieldDecl->getInClassInitializer());
		inBody_ = inBody;
	}

	field->setTypeExpression(utils_->translateQualifiedType(fieldDecl->getType()));
	// modifiers
	field->modifiers()->set(utils_->translateAccessSpecifier(fieldDecl->getAccess()));
	return true;
}

bool ClangAstVisitor::TraverseCaseStmt(clang::CaseStmt* caseStmt)
{
	OOModel::SwitchCase* ooSwitchCase = new OOModel::SwitchCase();
	// traverse condition
	TraverseStmt(caseStmt->getLHS());
	if(!ooExprStack_.empty())
		ooSwitchCase->setExpr(ooExprStack_.pop());
	// no need to store inBody because stored by TraverseSwitchStmt()
	inBody_ = true;
	// traverse statements/body
	ooStack_.push(ooSwitchCase->statement());
	TraverseStmt(caseStmt->getSubStmt());
	ooStack_.pop();
	inBody_ = false;
	ooSwitchCaseStack_.push(ooSwitchCase);
	return true;
}

bool ClangAstVisitor::TraverseDefaultStmt(clang::DefaultStmt* defaultStmt)
{
	OOModel::SwitchCase* ooDefaultCase = new OOModel::SwitchCase();
	// no need to store inBody because stored by TraverseSwitchStmt()
	inBody_ = true;
	// traverse statements/body
	ooStack_.push(ooDefaultCase->statement());
	TraverseStmt(defaultStmt->getSubStmt());
	ooStack_.pop();
	inBody_ = false;
	ooSwitchCaseStack_.push(ooDefaultCase);
	return true;
}

bool ClangAstVisitor::VisitBreakStmt(clang::BreakStmt* breakStmt)
{
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
		itemList->append(new OOModel::BreakStatement());
	else
		log_->writeError(className_, "Uknown where to put break stmt", breakStmt);
	return true;
}

bool ClangAstVisitor::VisitContinueStmt(clang::ContinueStmt* continueStmt)
{
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
		itemList->append(new OOModel::ContinueStatement());
	else
		log_->writeError(className_, "Uknown where to put continue stmt", continueStmt);
	return true;
}

bool ClangAstVisitor::VisitTypedefNameDecl(clang::TypedefNameDecl* typedefDecl)
{
	if(!shouldModel(typedefDecl->getLocation()))
		return true;
	OOModel::TypeAlias* ooTypeAlias = new OOModel::TypeAlias();
	ooTypeAlias->setTypeExpression(utils_->translateQualifiedType(typedefDecl->getUnderlyingType()));
	ooTypeAlias->setName(QString::fromStdString(typedefDecl->getNameAsString()));
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
		itemList->append(new OOModel::DeclarationStatement(ooTypeAlias));
	else if(auto declaration = dynamic_cast<OOModel::Declaration*>(ooStack_.top()))
		declaration->subDeclarations()->append(ooTypeAlias);
	else
		log_->writeError(className_, "Uknown where to put typedef", typedefDecl);
	return true;
}

bool ClangAstVisitor::VisitNamespaceAliasDecl(clang::NamespaceAliasDecl* namespaceAlias)
{
	if(!shouldModel(namespaceAlias->getLocation()))
		return true;
	OOModel::TypeAlias* ooTypeAlias = new OOModel::TypeAlias();
	ooTypeAlias->setName(QString::fromStdString(namespaceAlias->getNameAsString()));
	OOModel::ReferenceExpression* nameRef = new OOModel::ReferenceExpression
			(QString::fromStdString(namespaceAlias->getAliasedNamespace()->getNameAsString()));
	if(auto prefix = namespaceAlias->getQualifier())
		nameRef->setPrefix(utils_->translateNestedNameSpecifier(prefix));
	ooTypeAlias->setTypeExpression(nameRef);
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
		itemList->append(new OOModel::DeclarationStatement(ooTypeAlias));
	else if(auto declaration = dynamic_cast<OOModel::Declaration*>(ooStack_.top()))
		declaration->subDeclarations()->append(ooTypeAlias);
	else
	{
		SAFE_DELETE(ooTypeAlias);
		log_->writeError(className_, "Uknown where to put namespace alias", namespaceAlias);
	}
	return true;
}

bool ClangAstVisitor::VisitUsingDecl(clang::UsingDecl* usingDecl)
{
	if(!shouldModel(usingDecl->getLocation()))
		return true;
	OOModel::ReferenceExpression* nameRef = new OOModel::ReferenceExpression
			(QString::fromStdString(usingDecl->getNameAsString()));
	if(auto prefix = usingDecl->getQualifier())
		nameRef->setPrefix(utils_->translateNestedNameSpecifier(prefix));
	OOModel::NameImport* ooNameImport = new OOModel::NameImport(nameRef);
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
		itemList->append(new OOModel::DeclarationStatement(ooNameImport));
	else if(auto declaration = dynamic_cast<OOModel::Declaration*>(ooStack_.top()))
		declaration->subDeclarations()->append(ooNameImport);
	else
	{
		SAFE_DELETE(ooNameImport);
		log_->writeError(className_, "Uknown where to put using decl", usingDecl);
	}
	return true;
}

bool ClangAstVisitor::VisitUsingDirectiveDecl(clang::UsingDirectiveDecl* usingDirectiveDecl)
{
	if(!shouldModel(usingDirectiveDecl->getLocation()))
		return true;
	OOModel::ReferenceExpression* nameRef = new OOModel::ReferenceExpression
			(QString::fromStdString(usingDirectiveDecl->getNominatedNamespaceAsWritten()->getNameAsString()));
	if(auto prefix = usingDirectiveDecl->getQualifier())
		nameRef->setPrefix(utils_->translateNestedNameSpecifier(prefix));
	OOModel::NameImport* ooNameImport = new OOModel::NameImport(nameRef);
	if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
		itemList->append(new OOModel::DeclarationStatement(ooNameImport));
	else if(auto declaration = dynamic_cast<OOModel::Declaration*>(ooStack_.top()))
		declaration->subDeclarations()->append(ooNameImport);
	else
	{
		SAFE_DELETE(ooNameImport);
		log_->writeError(className_, "Uknown where to put using directive decl", usingDirectiveDecl);
	}
	return true;
}

bool ClangAstVisitor::shouldUseDataRecursionFor(clang::Stmt*)
{
	return false;
}

bool ClangAstVisitor::TraverseMethodDecl(clang::CXXMethodDecl* methodDecl, OOModel::Method::MethodKind kind)
{
	OOModel::Method* ooMethod = trMngr_->insertMethodDecl(methodDecl, kind);
	if(!ooMethod)
	{
		// TODO is this correct?
		// only consider a method where the parent has been visited
		if(trMngr_->containsClass(methodDecl->getParent()))
			log_->writeError(className_, "no ooModel::method found", methodDecl);
		return true;
	}
	// handle body, typeargs and storage specifier
	TraverseFunction(methodDecl, ooMethod);
	// modifiers
	ooMethod->modifiers()->set(utils_->translateAccessSpecifier(methodDecl->getAccess()));
	// member initializers
	if(auto constructor = llvm::dyn_cast<clang::CXXConstructorDecl>(methodDecl))
	{
		if(!constructor->getNumCtorInitializers())
			return true;
		for(auto initIt = constructor->init_begin(); initIt != constructor->init_end(); ++initIt)
		{
			if(!(*initIt)->isWritten())
				continue;
			if(auto ooMemberInit = utils_->translateMemberInit((*initIt)))
				ooMethod->memberInitializers()->append(ooMemberInit);
		}
	}
	return true;
}

void ClangAstVisitor::TraverseClass(clang::CXXRecordDecl* recordDecl, OOModel::Class* ooClass)
{
	Q_ASSERT(ooClass);
	// insert in model
	if(auto curProject = dynamic_cast<OOModel::Project*>(ooStack_.top()))
		curProject->classes()->append(ooClass);
	else if(auto curModel = dynamic_cast<OOModel::Module*>(ooStack_.top()))
		curModel->classes()->append(ooClass);
	else if(auto curClass = dynamic_cast<OOModel::Class*>(ooStack_.top()))
		curClass->classes()->append(ooClass);
	else if(auto itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top()))
		itemList->append(new OOModel::DeclarationStatement(ooClass));
	else
		log_->writeError(className_, "uknown where to put class", recordDecl);

	// visit child decls
	if(recordDecl->isThisDeclarationADefinition())
	{
		ooStack_.push(ooClass);
		for(auto declIt = recordDecl->decls_begin(); declIt!=recordDecl->decls_end(); ++declIt)
		{
			if(auto fDecl = llvm::dyn_cast<clang::FriendDecl>(*declIt))
			{
				// Class type
				if(auto type = fDecl->getFriendType())
					insertFriendClass(type, ooClass);
				// Functions
				if (auto friendDecl = fDecl->getFriendDecl())
				{
					if(!llvm::isa<clang::FunctionDecl>(friendDecl))
						log_->writeError(className_,"Friend which ish not a function", friendDecl);
					else
						insertFriendFunction(llvm::dyn_cast<clang::FunctionDecl>(friendDecl), ooClass);
				}
			}
			else
				TraverseDecl(*declIt);
		}
		ooStack_.pop();

		// visit base classes
		for(auto base_itr = recordDecl->bases_begin(); base_itr!=recordDecl->bases_end(); ++base_itr)
			ooClass->baseClasses()->append(utils_->translateQualifiedType(base_itr->getType()));
	}

	// set modifiers
	ooClass->modifiers()->set(utils_->translateAccessSpecifier(recordDecl->getAccess()));
}

void ClangAstVisitor::TraverseFunction(clang::FunctionDecl* functionDecl, OOModel::Method* ooFunction)
{
	Q_ASSERT(ooFunction);
	// only visit the body if we are at the definition
	if(functionDecl->isThisDeclarationADefinition())
	{
		if(ooFunction->items()->size())
		{
			log_->writeWarning(className_, "This function is double defined", functionDecl);
			return;
		}
		ooStack_.push(ooFunction->items());
		bool inBody = inBody_;
		inBody_ = true;
		if(auto body = functionDecl->getBody())
			TraverseStmt(body);
		inBody_ = inBody;
		ooStack_.pop();
	}
	// visit type arguments if any & if not yet visited
	if(!ooFunction->typeArguments()->size())
	{
		if(auto functionTemplate = functionDecl->getDescribedFunctionTemplate())
		{
			auto templateParamList = functionTemplate->getTemplateParameters();
			for( auto templateParam = templateParamList->begin();
				  templateParam != templateParamList->end(); ++templateParam)
			{
				templArgVisitor_->TraverseDecl(*templateParam);
				ooFunction->typeArguments()->append(templArgVisitor_->getLastTranslated());
			}
		}
		if(auto specArgs = functionDecl->getTemplateSpecializationArgsAsWritten())
		{
		unsigned templateArgs = specArgs->NumTemplateArgs;
		auto astTemplateArgsList = specArgs->getTemplateArgs();
		for(unsigned i = 0; i < templateArgs; i++)
			ooFunction->typeArguments()->append(new OOModel::FormalTypeArgument
						("#spec", utils_->translateTemplateArgument(astTemplateArgsList[i].getArgument())));
		}
	}
	// modifiers
	ooFunction->modifiers()->set(utils_->translateStorageSpecifier(functionDecl->getStorageClass()));
	if(functionDecl->isInlineSpecified())
		ooFunction->modifiers()->set(OOModel::Modifier::Inline);
	if(functionDecl->isVirtualAsWritten())
		ooFunction->modifiers()->set(OOModel::Modifier::Virtual);
}

OOModel::Class*ClangAstVisitor::createClass(clang::CXXRecordDecl* recordDecl)
{
	QString recordDeclName = QString::fromStdString(recordDecl->getNameAsString());
	if(recordDecl->isClass())
		return new OOModel::Class(recordDeclName,OOModel::Class::ConstructKind::Class);
	else if(recordDecl->isStruct())
		return new OOModel::Class(recordDeclName,OOModel::Class::ConstructKind::Struct);
	else if(recordDecl->isUnion())
		return new OOModel::Class(recordDeclName,OOModel::Class::ConstructKind::Union);
	log_->writeError(className_, "Unsupported RecordDecl", recordDecl);
	return nullptr;
}

void ClangAstVisitor::insertFriendClass(clang::TypeSourceInfo* typeInfo, OOModel::Class* ooClass)
{
	if(clang::CXXRecordDecl* recordDecl = typeInfo->getType().getTypePtr()->getAsCXXRecordDecl())
		ooClass->friends()->append(new OOModel::ReferenceExpression
											(QString::fromStdString(recordDecl->getNameAsString())));
}

void ClangAstVisitor::insertFriendFunction(clang::FunctionDecl* friendFunction, OOModel::Class* ooClass)
{
	if(friendFunction->isThisDeclarationADefinition())
	{
		// TODO this is at the moment the only solution to handle this
		ooStack_.push(ooClass);
		TraverseDecl(friendFunction);
		ooStack_.pop();
	}
	// this should happen anyway that it is clearly visible that there is a friend
	// TODO: this is not really a method call but rather a reference
	OOModel::MethodCallExpression* ooMCall = new OOModel::MethodCallExpression(
				QString::fromStdString(friendFunction->getNameAsString()));
	// TODO: handle return type & arguments & type arguments
	ooClass->friends()->append(ooMCall);
}

bool ClangAstVisitor::shouldModel(const clang::SourceLocation& location)
{
	QString fileName;
	if(auto file = sourceManager_->getPresumedLoc(location).getFilename())
		fileName = QString(file);
	if(sourceManager_->isInSystemHeader(location) || fileName.isEmpty() || fileName.contains("qt"))
		return modelSysHeader_;
	return true;
}

} // namespace cppimport
