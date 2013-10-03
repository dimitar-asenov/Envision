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

#include "ExpressionVisitor.h"
#include "ClangAstVisitor.h"
#include "../CppImportUtilities.h"

namespace CppImport {

ExpressionVisitor::ExpressionVisitor(ClangAstVisitor* visitor, CppImportLogger* log)
: baseVisitor_{visitor}, log_{log}
{}

void ExpressionVisitor::setUtilities(CppImportUtilities* utils)
{
	Q_ASSERT(utils);
	utils_ = utils;
}

OOModel::Expression* ExpressionVisitor::translateExpression(clang::Stmt* s)
{
	TraverseStmt(s);
	if(!ooExprStack_.empty()) return ooExprStack_.pop();
	return utils_->createErrorExpression("Could not convert last expression");
}

bool ExpressionVisitor::VisitExpr(clang::Expr* e)
{
	if(e && !llvm::isa<clang::ImplicitCastExpr>(e))
	{
		log_->writeError(className_, e, CppImportLogger::Reason::NOT_SUPPORTED);
		return true;
	}
	return Base::VisitStmt(e);
}

bool ExpressionVisitor::TraverseMemberExpr(clang::MemberExpr* memberExpr)
{
	QString name = QString::fromStdString(memberExpr->getMemberDecl()->getNameAsString());
	clang::Expr* base = nullptr;
	if(!memberExpr->isImplicitAccess())
		base = memberExpr->getBase();
	if(memberExpr->hasExplicitTemplateArgs())
		ooExprStack_.push(createRef(name, memberExpr->getLocStart(), memberExpr->getQualifier(),
											 memberExpr->getExplicitTemplateArgs().getTemplateArgs(),
											 memberExpr->getNumTemplateArgs(), base));
	else
		ooExprStack_.push(createRef(name, memberExpr->getLocStart(), memberExpr->getQualifier(), nullptr, 0, base));
	return true;
}

bool ExpressionVisitor::TraverseUnresolvedMemberExpr(clang::UnresolvedMemberExpr* unresolvedMember)
{
	QString name = QString::fromStdString(unresolvedMember->getMemberName().getAsString());
	clang::Expr* base = nullptr;
	if(!unresolvedMember->isImplicitAccess())
		base = unresolvedMember->getBase();
	if(unresolvedMember->hasExplicitTemplateArgs())
		ooExprStack_.push(createRef(name, unresolvedMember->getLocStart(), unresolvedMember->getQualifier(),
											 unresolvedMember->getExplicitTemplateArgs().getTemplateArgs(),
											 unresolvedMember->getNumTemplateArgs(), base));
	else
		ooExprStack_.push(createRef(name, unresolvedMember->getLocStart(),
											 unresolvedMember->getQualifier(), nullptr, 0, base));
	return true;
}

bool ExpressionVisitor::TraverseCXXDependentScopeMemberExpr(clang::CXXDependentScopeMemberExpr* dependentScopeMember)
{
	QString name = QString::fromStdString(dependentScopeMember->getMember().getAsString());
	clang::Expr* base = nullptr;
	if(!dependentScopeMember->isImplicitAccess())
		base = dependentScopeMember->getBase();
	if(dependentScopeMember->hasExplicitTemplateArgs())
		ooExprStack_.push(createRef(name, dependentScopeMember->getLocStart(), dependentScopeMember->getQualifier(),
											 dependentScopeMember->getExplicitTemplateArgs().getTemplateArgs(),
											 dependentScopeMember->getNumTemplateArgs(), base));
	else
		ooExprStack_.push(createRef(name, dependentScopeMember->getLocStart(),
											 dependentScopeMember->getQualifier(), nullptr, 0, base));
	return true;
}

bool ExpressionVisitor::TraverseDeclRefExpr(clang::DeclRefExpr* declRefExpr)
{
	QString name = QString::fromStdString(declRefExpr->getDecl()->getNameAsString());
	if(declRefExpr->hasExplicitTemplateArgs())
		ooExprStack_.push(createRef(name, declRefExpr->getLocStart(), declRefExpr->getQualifier(),
											 declRefExpr->getExplicitTemplateArgs().getTemplateArgs(),
											 declRefExpr->getNumTemplateArgs()));
	else
		ooExprStack_.push(createRef(name, declRefExpr->getLocStart(), declRefExpr->getQualifier()));
	return true;
}

bool ExpressionVisitor::TraverseDependentScopeDeclRefExpr(clang::DependentScopeDeclRefExpr* dependentScope)
{
	QString name = QString::fromStdString(dependentScope->getDeclName().getAsString());
	if(dependentScope->hasExplicitTemplateArgs())
		ooExprStack_.push(createRef(name, dependentScope->getLocStart(), dependentScope->getQualifier(),
											 dependentScope->getExplicitTemplateArgs().getTemplateArgs(),
											 dependentScope->getNumTemplateArgs()));
	else
		ooExprStack_.push(createRef(name, dependentScope->getLocStart(), dependentScope->getQualifier()));
	return true;
}

bool ExpressionVisitor::TraverseCXXMemberCallExpr(clang::CXXMemberCallExpr* callExpr)
{
	return TraverseCallExpr(callExpr);
}

bool ExpressionVisitor::TraverseCallExpr(clang::CallExpr* callExpr)
{
	OOModel::MethodCallExpression* ooMethodCall = new OOModel::MethodCallExpression();
	TraverseStmt(callExpr->getCallee());
	if(!ooExprStack_.empty())
		ooMethodCall->setCallee(ooExprStack_.pop());
	else
	{
		log_->writeError(className_, callExpr->getCallee(), CppImportLogger::Reason::NOT_SUPPORTED);
		ooMethodCall->setCallee(utils_->createErrorExpression("Could not convert calleee"));
	}

	// visit arguments
	for(auto argIt = callExpr->arg_begin(); argIt!=callExpr->arg_end(); ++argIt)
	{
		if(llvm::isa<clang::CXXDefaultArgExpr>(*argIt))
			// this is a default arg and is not written in the source code
			continue;
		TraverseStmt(*argIt);
		if(!ooExprStack_.empty())
			ooMethodCall->arguments()->append(ooExprStack_.pop());
		else
			log_->writeError(className_, *argIt, CppImportLogger::Reason::NOT_SUPPORTED);
	}

	ooExprStack_.push(ooMethodCall);

	return true;
}

bool ExpressionVisitor::TraverseCXXOperatorCallExpr(clang::CXXOperatorCallExpr* callExpr)
{
	//first traverse the callee then put each argument on the stack
	if(auto callee = llvm::dyn_cast<clang::MemberExpr>(callExpr->getCallee()->IgnoreImplicit()))
		TraverseStmt(callee);
	for(auto argIt = callExpr->arg_begin(); argIt != callExpr->arg_end(); ++argIt)
		TraverseStmt(*argIt);

	unsigned numArguments = callExpr->getNumArgs();
	clang::OverloadedOperatorKind operatorKind = callExpr->getOperator();
	switch(utils_->getOverloadKind(operatorKind, numArguments))
	{
		case CppImportUtilities::OverloadKind::Unsupported:
			ooExprStack_.push(utils_->createErrorExpression("Unsupported Overload Expression"));
			break;
		case CppImportUtilities::OverloadKind::Unary:
		{
			if(2 == numArguments)
				// remove dummy expression
				ooExprStack_.pop();
			OOModel::UnaryOperation* ooUnary = new OOModel::UnaryOperation();
			ooUnary->setOp(utils_->translateUnaryOverloadOp(operatorKind, numArguments));
			if(!ooExprStack_.empty())
				ooUnary->setOperand(ooExprStack_.pop());
			ooExprStack_.push(ooUnary);
			break;
		}
		case CppImportUtilities::OverloadKind::Binary:
		{
			OOModel::BinaryOperation* ooBinary = new OOModel::BinaryOperation();
			ooBinary->setOp(utils_->translateBinaryOverloadOp(operatorKind));
			if(!ooExprStack_.empty())
				ooBinary->setRight(ooExprStack_.pop());
			if(!ooExprStack_.empty())
				ooBinary->setLeft(ooExprStack_.pop());
			ooExprStack_.push(ooBinary);
			break;
		}
		case CppImportUtilities::OverloadKind::Assign:
		{
			OOModel::AssignmentExpression* ooAssign = new OOModel::AssignmentExpression();
			ooAssign->setOp(utils_->translateAssignOverloadOp(operatorKind));
			if(!ooExprStack_.empty())
				ooAssign->setRight(ooExprStack_.pop());
			if(!ooExprStack_.empty())
				ooAssign->setLeft(ooExprStack_.pop());
			ooExprStack_.push(ooAssign);
			break;
		}
		case CppImportUtilities::OverloadKind::MethodCall:
		{
			OOModel::MethodCallExpression* ooCall = new OOModel::MethodCallExpression();
			for(unsigned i = 0; i < numArguments - 1; i++)
			{
				if(!ooExprStack_.empty())
					ooCall->arguments()->prepend(ooExprStack_.pop());
			}
			if(!ooExprStack_.empty())
			{
				ooCall->setCallee(ooExprStack_.pop());
				ooExprStack_.push(ooCall);
				break;
			}
			// this should not happen
			SAFE_DELETE(ooCall);
			log_->writeError(className_, callExpr, CppImportLogger::Reason::OTHER, "No method callee found (overload)");
			ooExprStack_.push(utils_->createErrorExpression("METHOD CALL NO NAME FOUND"));
			break;
		}
		case CppImportUtilities::OverloadKind::ReferenceExpr:
		{
			if(!ooExprStack_.empty())
				if(DCast<OOModel::ReferenceExpression>(ooExprStack_.top()))
					break;
			ooExprStack_.push(utils_->createErrorExpression("Could not resolve Reference/Arrow"));
			break;
		}
		case CppImportUtilities::OverloadKind::Comma:
			ooExprStack_.push(utils_->createErrorExpression("Unsupported COMMA OVERLOAD"));
			break;
		default:
			ooExprStack_.push(utils_->createErrorExpression("Unsupported Overload Expression"));
			break;
	}

	return true;
}

bool ExpressionVisitor::TraverseCXXNewExpr(clang::CXXNewExpr* newExpr)
{
	OOModel::NewExpression* ooNewExpr = new OOModel::NewExpression();
	TraverseStmt(newExpr->getInitializer());
	if(!ooExprStack_.empty())
		ooNewExpr->setNewType(ooExprStack_.pop());
	if(newExpr->isArray())
	{
		TraverseStmt(newExpr->getArraySize());
		if(!ooExprStack_.empty())
			ooNewExpr->setAmount(ooExprStack_.pop());
	}

	ooExprStack_.push(ooNewExpr);
	return true;
}

bool ExpressionVisitor::TraverseCXXDeleteExpr(clang::CXXDeleteExpr* deleteExpr)
{
	OOModel::DeleteExpression* ooDeleteExpr = new OOModel::DeleteExpression(deleteExpr->isArrayForm());
	TraverseStmt(deleteExpr->getArgument());
	if(!ooExprStack_.empty())
		ooDeleteExpr->setExpr(ooExprStack_.pop());
	ooExprStack_.push(ooDeleteExpr);
	return true;
}

bool ExpressionVisitor::TraverseIntegerLiteral(clang::IntegerLiteral* intLit)
{
	ooExprStack_.push(new OOModel::IntegerLiteral(intLit->getValue().getLimitedValue()));
	return true;
}

bool ExpressionVisitor::TraverseCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr* boolLitExpr)
{
	ooExprStack_.push(new OOModel::BooleanLiteral(boolLitExpr->getValue()));
	return true;
}

bool ExpressionVisitor::TraverseCXXNullPtrLiteralExpr(clang::CXXNullPtrLiteralExpr*)
{
	ooExprStack_.push(new OOModel::NullLiteral());
	return true;
}

bool ExpressionVisitor::TraverseFloatingLiteral(clang::FloatingLiteral* floatLiteral)
{
	ooExprStack_.push(new OOModel::FloatLiteral(floatLiteral->getValueAsApproximateDouble()));
	return true;
}

bool ExpressionVisitor::TraverseCharacterLiteral(clang::CharacterLiteral* charLiteral)
{
	ooExprStack_.push(new OOModel::CharacterLiteral(QChar(charLiteral->getValue())));
	return true;
}

bool ExpressionVisitor::TraverseStringLiteral(clang::StringLiteral* stringLiteral)
{
	ooExprStack_.push(new OOModel::StringLiteral(QString::fromStdString(stringLiteral->getBytes().str())));
	return true;
}

bool ExpressionVisitor::TraverseCXXConstructExpr(clang::CXXConstructExpr* constructExpr)
{
	// if is elidable we can directly visit the children
	if(constructExpr->isElidable())
		return TraverseStmt(*(constructExpr->child_begin()));
	// check for lambda
	if(!constructExpr->getConstructor()->getParent()->isLambda())
	{
		OOModel::MethodCallExpression* ooMethodCall = new OOModel::MethodCallExpression(
					QString::fromStdString(constructExpr->getConstructor()->getNameAsString()));
		for(auto argIt = constructExpr->arg_begin(); argIt != constructExpr->arg_end(); ++argIt)
		{
			if(llvm::isa<clang::CXXDefaultArgExpr>(*argIt))
				// this is a default arg and is not written in the source code
				continue;
			TraverseStmt(*argIt);
			if(!ooExprStack_.empty())
				ooMethodCall->arguments()->append(ooExprStack_.pop());
			else
				log_->writeError(className_, *argIt, CppImportLogger::Reason::NOT_SUPPORTED);
		}
		ooExprStack_.push(ooMethodCall);
		return true;
	}
	// clang models lambda construct expressions weird, the name of the lambda is in the first argument
	if(constructExpr->getNumArgs() != 1)
		log_->writeError(className_, constructExpr, CppImportLogger::Reason::OTHER, "ContructExpr need inspectation");
	for(auto argIt = constructExpr->arg_begin(); argIt != constructExpr->arg_end(); ++argIt)
	{
		if(llvm::isa<clang::CXXDefaultArgExpr>(*argIt))
			// this is a default arg and is not written in the source code
			continue;
		TraverseStmt(*argIt);
	}
	return true;
}

bool ExpressionVisitor::TraverseCXXUnresolvedConstructExpr(clang::CXXUnresolvedConstructExpr* unresolvedConstruct)
{
	OOModel::MethodCallExpression* ooMethodCall = new OOModel::MethodCallExpression();
	ooMethodCall->setCallee(utils_->translateQualifiedType(unresolvedConstruct->getTypeAsWritten(),
																			 unresolvedConstruct->getLocStart()));
	// visit arguments
	for(auto argIt = unresolvedConstruct->arg_begin(); argIt!=unresolvedConstruct->arg_end(); ++argIt)
	{
		if(llvm::isa<clang::CXXDefaultArgExpr>(*argIt))
			// this is a default arg and is not written in the source code
			continue;
		TraverseStmt(*argIt);
		if(!ooExprStack_.empty())
			ooMethodCall->arguments()->append(ooExprStack_.pop());
		else
			log_->writeError(className_, *argIt, CppImportLogger::Reason::NOT_SUPPORTED);
	}
	ooExprStack_.push(ooMethodCall);
	return true;
}

bool ExpressionVisitor::TraverseParenExpr(clang::ParenExpr* parenthesizedExpr)
{
	OOModel::UnaryOperation* ooParenExpr = new OOModel::UnaryOperation();
	ooParenExpr->setOp(OOModel::UnaryOperation::PARENTHESIS);
	TraverseStmt(parenthesizedExpr->getSubExpr());
	if(!ooExprStack_.empty())
		ooParenExpr->setOperand(ooExprStack_.pop());

	ooExprStack_.push(ooParenExpr);
	return true;
}

bool ExpressionVisitor::TraverseArraySubscriptExpr(clang::ArraySubscriptExpr* arraySubsrciptExpr)
{
		OOModel::BinaryOperation* ooArrayAccess = new OOModel::BinaryOperation();
		ooArrayAccess->setOp(OOModel::BinaryOperation::ARRAY_INDEX);
		// visit the base the base is A in the expr A[10]
		TraverseStmt(arraySubsrciptExpr->getBase());
		if(!ooExprStack_.empty())
			ooArrayAccess->setLeft(ooExprStack_.pop());
		// visit the idx the idx is 10 in the expr A[10]
		TraverseStmt(arraySubsrciptExpr->getIdx());
		if(!ooExprStack_.empty())
			ooArrayAccess->setRight(ooExprStack_.pop());

		ooExprStack_.push(ooArrayAccess);
		return true;
}

bool ExpressionVisitor::TraverseCXXThisExpr(clang::CXXThisExpr* thisExpr)
{
	if(!thisExpr->isImplicit())
			ooExprStack_.push(new OOModel::ThisExpression());
	return true;
}

bool ExpressionVisitor::TraverseCXXTypeidExpr(clang::CXXTypeidExpr* typeIdExpr)
{
	OOModel::TypeTraitExpression* ooTypeTrait = new OOModel::TypeTraitExpression
			(OOModel::TypeTraitExpression::TypeTraitKind::TypeId);
	if(typeIdExpr->isTypeOperand())
		ooTypeTrait->setOperand(utils_->translateQualifiedType(
				typeIdExpr->getTypeOperandSourceInfo()->getType(), typeIdExpr->getLocStart()));
	else
	{
		TraverseStmt(typeIdExpr->getExprOperand());
		if(!ooExprStack_.empty())
			ooTypeTrait->setOperand(ooExprStack_.pop());
		else
			log_->writeError(className_, typeIdExpr, CppImportLogger::Reason::NOT_SUPPORTED);
	}
	ooExprStack_.push(ooTypeTrait);
	return true;
}

bool ExpressionVisitor::WalkUpFromOverloadExpr(clang::OverloadExpr* overloadExpr)
{
	OOModel::ReferenceExpression* ooRef = new OOModel::ReferenceExpression
			(QString::fromStdString(overloadExpr->getName().getAsString()));
	// template args
	if(overloadExpr->hasExplicitTemplateArgs())
	{
		unsigned templateArgs = overloadExpr->getNumTemplateArgs();
		auto astTemplateArgsList = overloadExpr->getExplicitTemplateArgs().getTemplateArgs();
		for(unsigned i = 0; i < templateArgs; i++)
			ooRef->typeArguments()->append(utils_->translateTemplateArgument(astTemplateArgsList[i].getArgument(),
																								  astTemplateArgsList[i].getLocation()));
	}
	ooExprStack_.push(ooRef);
	return true;
}

bool ExpressionVisitor::TraverseLambdaExpr(clang::LambdaExpr* lambdaExpr)
{
	// TODO: handle captions
	OOModel::LambdaExpression* ooLambda = new OOModel::LambdaExpression();
	// visit body
	baseVisitor_->pushOOStack(ooLambda->body());
	baseVisitor_->TraverseStmt(lambdaExpr->getBody());
	baseVisitor_->popOOStack();
	// visit arguments
	clang::CXXMethodDecl* callOperator = lambdaExpr->getCallOperator();
	for(auto it = callOperator->param_begin(); it != callOperator->param_end(); ++it)
	{
		OOModel::FormalArgument* arg = new OOModel::FormalArgument();
		arg->setName(QString::fromStdString((*it)->getNameAsString()));
		OOModel::Expression* type = utils_->translateQualifiedType((*it)->getType(), (*it)->getLocStart());
		if(type) arg->setTypeExpression(type);
		ooLambda->arguments()->append(arg);
	}

	ooExprStack_.push(ooLambda);
	return true;
}

bool ExpressionVisitor::TraverseConditionalOperator(clang::ConditionalOperator* conditionalOperator)
{
	OOModel::ConditionalExpression* ooConditionalExpr = new OOModel::ConditionalExpression();
	// traverse condition
	TraverseStmt(conditionalOperator->getCond());
	if(!ooExprStack_.empty())
		ooConditionalExpr->setCondition(ooExprStack_.pop());
	// traverse true part
	TraverseStmt(conditionalOperator->getTrueExpr());
	if(!ooExprStack_.empty())
		ooConditionalExpr->setTrueExpression(ooExprStack_.pop());
	// traverse false part
	TraverseStmt(conditionalOperator->getFalseExpr());
	if(!ooExprStack_.empty())
		ooConditionalExpr->setFalseExpression(ooExprStack_.pop());

	ooExprStack_.push(ooConditionalExpr);
	return true;
}

bool ExpressionVisitor::TraverseCXXThrowExpr(clang::CXXThrowExpr* throwExpr)
{
	OOModel::ThrowExpression* ooThrow = new OOModel::ThrowExpression();
	// visit throw expression
	if(auto subExpr = throwExpr->getSubExpr())
	{
		TraverseStmt(subExpr);
		if(!ooExprStack_.empty())
			ooThrow->setExpr(ooExprStack_.pop());
	}
	else
		ooThrow->setExpr(new OOModel::EmptyExpression());
	ooExprStack_.push(ooThrow);
	return true;
}

bool ExpressionVisitor::TraverseCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr* tempObjectExpr)
{
	return TraverseCXXConstructExpr(tempObjectExpr);
}

bool ExpressionVisitor::TraverseInitListExpr(clang::InitListExpr* initListExpr)
{
	OOModel::ArrayInitializer* ooArrayInit = new OOModel::ArrayInitializer();
	for(auto iter = initListExpr->begin(); iter!=initListExpr->end(); ++iter)
	{
		TraverseStmt(*iter);
		if(!ooExprStack_.empty()) ooArrayInit->values()->append(ooExprStack_.pop());
	}
	ooExprStack_.push(ooArrayInit);
	return true;
}

bool ExpressionVisitor::TraverseUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr* typeTrait)
{
	OOModel::Expression* argument = nullptr;
	if(typeTrait->isArgumentType())
		argument = utils_->translateQualifiedType(typeTrait->getArgumentType(), typeTrait->getLocStart());
	else
	{
		TraverseStmt(typeTrait->getArgumentExpr());
		if(!ooExprStack_.empty()) argument = ooExprStack_.pop();
		else
		{
			log_->writeError(className_, typeTrait->getArgumentExpr(), CppImportLogger::Reason::NOT_SUPPORTED);
			argument = utils_->createErrorExpression("Unsupported argument");
		}
	}
	auto kind = typeTrait->getKind();
	if(clang::UETT_SizeOf == kind)
		ooExprStack_.push(new OOModel::TypeTraitExpression
								(OOModel::TypeTraitExpression::TypeTraitKind::SizeOf, argument));
	else if(clang::UETT_AlignOf == kind)
		ooExprStack_.push(new OOModel::TypeTraitExpression
								(OOModel::TypeTraitExpression::TypeTraitKind::AlignOf, argument));
	else
	{
		log_->writeError(className_, typeTrait, CppImportLogger::Reason::NOT_SUPPORTED);
		SAFE_DELETE(argument);
	}
	return true;
}


OOModel::ReferenceExpression* ExpressionVisitor::createRef
(const QString& name, const clang::SourceLocation& location, const clang::NestedNameSpecifier* qualifier,
 const clang::TemplateArgumentLoc* templateArgs, unsigned numTArgs, clang::Expr* base)
{
	OOModel::ReferenceExpression* ooRef = new OOModel::ReferenceExpression(name);
	if(templateArgs)
		for(unsigned i = 0; i < numTArgs; i++)
			ooRef->typeArguments()->append(utils_->translateTemplateArgument(templateArgs[i].getArgument(),
																								  templateArgs[i].getLocation()));
	OOModel::Expression* ooBase = nullptr;
	if(base)
	{
		TraverseStmt(base);
		if(!ooExprStack_.empty()) ooBase = ooExprStack_.pop();
	}
	if(qualifier)
		ooRef->setPrefix(utils_->translateNestedNameSpecifier(qualifier, location, ooBase));
	else if(ooBase)
		ooRef->setPrefix(ooBase);
	return ooRef;
}

bool ExpressionVisitor::TraverseBinaryOp(clang::BinaryOperator* binaryOperator)
{
	OOModel::Expression* ooLeft = nullptr;
	OOModel::Expression* ooRight = nullptr;
	// left
	TraverseStmt(binaryOperator->getLHS());
	if(!ooExprStack_.empty()) ooLeft = ooExprStack_.pop();
	else
		log_->writeError(className_, binaryOperator->getLHS(), CppImportLogger::Reason::NOT_SUPPORTED);
	// right
	TraverseStmt(binaryOperator->getRHS());
	if(!ooExprStack_.empty()) ooRight = ooExprStack_.pop();
	else
		log_->writeError(className_, binaryOperator->getRHS(), CppImportLogger::Reason::NOT_SUPPORTED);

	clang::BinaryOperatorKind opcode = binaryOperator->getOpcode();
	if(opcode == clang::BO_Comma)
		ooExprStack_.push(new OOModel::CommaExpression(ooLeft, ooRight));
	else
		ooExprStack_.push(new OOModel::BinaryOperation
								(utils_->translateBinaryOp(opcode), ooLeft, ooRight));
	return true;
}

bool ExpressionVisitor::TraverseAssignment(clang::BinaryOperator* binaryOperator)
{
	OOModel::AssignmentExpression* ooBinOp = new OOModel::AssignmentExpression
			(utils_->translateAssignOp(binaryOperator->getOpcode()));
	// left
	TraverseStmt(binaryOperator->getLHS());
	if(!ooExprStack_.empty()) ooBinOp->setLeft(ooExprStack_.pop());
	else log_->writeError(className_, binaryOperator->getLHS(), CppImportLogger::Reason::NOT_SUPPORTED);
	// right
	TraverseStmt(binaryOperator->getRHS());
	if(!ooExprStack_.empty()) ooBinOp->setRight(ooExprStack_.pop());
	else log_->writeError(className_, binaryOperator->getRHS(), CppImportLogger::Reason::NOT_SUPPORTED);

	ooExprStack_.push(ooBinOp);
	return true;
}

bool ExpressionVisitor::TraverseUnaryOp(clang::UnaryOperator* unaryOperator)
{
	clang::UnaryOperatorKind opcode = unaryOperator->getOpcode();
	if(opcode == clang::UO_Extension || opcode == clang::UO_Real || opcode == clang::UO_Imag)
	{
		log_->writeError(className_, unaryOperator, CppImportLogger::Reason::NOT_SUPPORTED);
		unaryOperator->dump();
		// just convert the subexpression
		log_->unaryOpNotSupported(opcode);
		return TraverseStmt(unaryOperator->getSubExpr());
	}
	OOModel::UnaryOperation* ooUnaryOp = new OOModel::UnaryOperation(utils_->translateUnaryOp(opcode));
	// subexpr
	TraverseStmt(unaryOperator->getSubExpr());
	if(!ooExprStack_.empty()) ooUnaryOp->setOperand(ooExprStack_.pop());
	else log_->writeError(className_, unaryOperator->getSubExpr(), CppImportLogger::Reason::NOT_SUPPORTED);

	ooExprStack_.push(ooUnaryOp);
	return true;
}

bool ExpressionVisitor::TraverseExplCastExpr(clang::ExplicitCastExpr* castExpr, OOModel::CastExpression::CastKind kind)
{
	OOModel::CastExpression* ooCast = new OOModel::CastExpression(kind);
	// setType to cast to
	ooCast->setType(utils_->translateQualifiedType(castExpr->getType(), castExpr->getLocStart()));
	// visit subexpr
	TraverseStmt(castExpr->getSubExprAsWritten());
	if(!ooExprStack_.empty()) ooCast->setExpr(ooExprStack_.pop());

	ooExprStack_.push(ooCast);
	return true;
}

}
