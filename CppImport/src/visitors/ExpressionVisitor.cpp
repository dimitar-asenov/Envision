/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

ExpressionVisitor::ExpressionVisitor(ClangAstVisitor* visitor, ClangHelpers& clang, CppImportLogger* log)
 : baseVisitor_{visitor}, clang_{clang}, log_{log}
{}

void ExpressionVisitor::setUtilities(CppImportUtilities* utils)
{
	Q_ASSERT(utils);
	utils_ = utils;
}

OOModel::Expression* ExpressionVisitor::translateExpression(clang::Stmt* s)
{
	TraverseStmt(s);
	if (!ooExprStack_.empty()) return ooExprStack_.pop();
	return utils_->createErrorExpression("Could not convert last expression", s->getSourceRange());
}

bool ExpressionVisitor::VisitExpr(clang::Expr* e)
{
	if (e && !llvm::isa<clang::ImplicitCastExpr>(e) && !llvm::isa<clang::CXXBindTemporaryExpr>(e)
		 && !llvm::isa<clang::MaterializeTemporaryExpr>(e) && !llvm::isa<clang::ExprWithCleanups>(e))
	{
		log_->writeError(className_, e, CppImportLogger::Reason::NOT_SUPPORTED);
		return true;
	}
	return Base::VisitStmt(e);
}

bool ExpressionVisitor::TraverseMemberExpr(clang::MemberExpr* memberExpr)
{
	clang::Expr* base = nullptr;
	if (!memberExpr->isImplicitAccess())
		base = memberExpr->getBase();

	OOModel::ReferenceExpression* ooReference = nullptr;
	if (memberExpr->hasExplicitTemplateArgs())
		ooReference = createQualifiedReferenceWithTemplateArguments(memberExpr->getMemberNameInfo().getSourceRange(),
																						memberExpr->getQualifierLoc(),
																						memberExpr->getTemplateArgs(),
																						memberExpr->getNumTemplateArgs(), base);
	else
		ooReference = createQualifiedReferenceWithTemplateArguments(memberExpr->getMemberNameInfo().getSourceRange(),
																						memberExpr->getQualifierLoc(), nullptr, 0, base);

	ooExprStack_.push(ooReference);

	return true;
}

bool ExpressionVisitor::TraverseUnresolvedMemberExpr(clang::UnresolvedMemberExpr* unresolvedMember)
{
	clang::Expr* base = nullptr;
	if (!unresolvedMember->isImplicitAccess())
		base = unresolvedMember->getBase();

	OOModel::ReferenceExpression* ooReference = nullptr;
	if (unresolvedMember->hasExplicitTemplateArgs())
		ooReference = createQualifiedReferenceWithTemplateArguments(unresolvedMember->getMemberNameInfo()
																							.getSourceRange(),
																						unresolvedMember->getQualifierLoc(),
																						unresolvedMember->getTemplateArgs(),
																						unresolvedMember->getNumTemplateArgs(), base);
	else
		ooReference = createQualifiedReferenceWithTemplateArguments(unresolvedMember->getMemberNameInfo()
																							.getSourceRange(),
																						unresolvedMember->getQualifierLoc(), nullptr, 0,
																						base);

	ooExprStack_.push(ooReference);
	return true;
}

bool ExpressionVisitor::TraverseCXXDependentScopeMemberExpr(clang::CXXDependentScopeMemberExpr* dependentScopeMember)
{
	clang::Expr* base = nullptr;
	if (!dependentScopeMember->isImplicitAccess())
		base = dependentScopeMember->getBase();

	OOModel::ReferenceExpression* ooReference = nullptr;
	if (dependentScopeMember->hasExplicitTemplateArgs())
		ooReference = createQualifiedReferenceWithTemplateArguments(dependentScopeMember->getMemberNameInfo()
																							.getSourceRange(),
																						dependentScopeMember->getQualifierLoc(),
																						dependentScopeMember->getTemplateArgs(),
																						dependentScopeMember->getNumTemplateArgs(), base);
	else
		ooReference = createQualifiedReferenceWithTemplateArguments(dependentScopeMember->getMemberNameInfo()
																						.getSourceRange(),
																						dependentScopeMember->getQualifierLoc(), nullptr, 0,
																						base);

	ooExprStack_.push(ooReference);
	return true;
}

bool ExpressionVisitor::TraverseDeclRefExpr(clang::DeclRefExpr* declRefExpr)
{
	OOModel::ReferenceExpression* ooReference = nullptr;
	if (declRefExpr->hasExplicitTemplateArgs())
		ooReference = createQualifiedReferenceWithTemplateArguments(declRefExpr->getNameInfo().getSourceRange(),
																						declRefExpr->getQualifierLoc(),
																						declRefExpr->getTemplateArgs(),
																						declRefExpr->getNumTemplateArgs());
	else
		ooReference = createQualifiedReferenceWithTemplateArguments(declRefExpr->getNameInfo().getSourceRange(),
																						declRefExpr->getQualifierLoc());

	ooExprStack_.push(ooReference);
	return true;
}

bool ExpressionVisitor::TraverseDependentScopeDeclRefExpr(clang::DependentScopeDeclRefExpr* dependentScope)
{
	OOModel::ReferenceExpression* ooReference = nullptr;
	if (dependentScope->hasExplicitTemplateArgs())
		ooReference = createQualifiedReferenceWithTemplateArguments(dependentScope->getNameInfo().getSourceRange(),
																						dependentScope->getQualifierLoc(),
																						dependentScope->getTemplateArgs(),
																						dependentScope->getNumTemplateArgs());
	else
		ooReference = createQualifiedReferenceWithTemplateArguments(dependentScope->getNameInfo().getSourceRange(),
																						dependentScope->getQualifierLoc());

	if (dependentScope->hasTemplateKeyword())
		ooReference->setMemberKind(OOModel::ReferenceExpression::MemberKind::Template);
	ooExprStack_.push(ooReference);
	return true;
}

bool ExpressionVisitor::TraverseCXXMemberCallExpr(clang::CXXMemberCallExpr* callExpr)
{
	// skip nameless member calls (operator calls etc.)
	if (auto memberExpr = llvm::dyn_cast<clang::MemberExpr>(callExpr->getCallee()))
			if (!memberExpr->getMemberNameInfo().getLoc().getPtrEncoding())
				return Base::TraverseStmt(callExpr->getImplicitObjectArgument());

	return TraverseCallExpr(callExpr);
}

bool ExpressionVisitor::TraverseCallExpr(clang::CallExpr* callExpr)
{
	auto ooMethodCall = clang_.createNode<OOModel::MethodCallExpression>(callExpr->getSourceRange());
	TraverseStmt(callExpr->getCallee());
	if (!ooExprStack_.empty())
		ooMethodCall->setCallee(ooExprStack_.pop());
	else
	{
		log_->writeError(className_, callExpr->getCallee(), CppImportLogger::Reason::NOT_SUPPORTED);
		ooMethodCall->setCallee(utils_->createErrorExpression("Could not convert calleee", callExpr->getSourceRange()));
	}
	for (auto argument : translateArguments(callExpr->getArgs(), callExpr->getNumArgs()))
		ooMethodCall->arguments()->append(argument);

	ooExprStack_.push(ooMethodCall);
	return true;
}

bool ExpressionVisitor::TraverseStmt(clang::Stmt* S)
{
	// WORKAROUND
	// we check for binary operation directly because as of clang 3.8 tree traversal does not work for <<
	clang::BinaryOperator* binOp = S ? llvm::dyn_cast<clang::BinaryOperator>(S) : nullptr;
	if (binOp && binOp->isShiftOp())
		return TraverseBinaryOp(binOp);
	else
		return Base::TraverseStmt(S);
}

bool ExpressionVisitor::TraverseCXXOperatorCallExpr(clang::CXXOperatorCallExpr* callExpr)
{
	//first Traverse the callee then put each argument on the stack
	if (auto callee = llvm::dyn_cast<clang::MemberExpr>(callExpr->getCallee()->IgnoreImplicit()))
		TraverseStmt(callee);
	for (auto argIt = callExpr->arg_begin(); argIt != callExpr->arg_end(); ++argIt)
		TraverseStmt(*argIt);

	unsigned numArguments = callExpr->getNumArgs();
	clang::OverloadedOperatorKind operatorKind = callExpr->getOperator();
	switch (utils_->getOverloadKind(operatorKind, numArguments))
	{
		case CppImportUtilities::OverloadKind::Unsupported:
			ooExprStack_.push(utils_->createErrorExpression("Unsupported Overload Expression",
																			callExpr->getSourceRange()));
			break;
		case CppImportUtilities::OverloadKind::Unary:
		{
			auto ooUnary = clang_.createNode<OOModel::UnaryOperation>(callExpr->getSourceRange(),
																			utils_->translateUnaryOverloadOp(operatorKind, numArguments));
			if (!ooExprStack_.empty())
				ooUnary->setOperand(ooExprStack_.pop());
			ooExprStack_.push(ooUnary);
			break;
		}
		case CppImportUtilities::OverloadKind::Binary:
		{
			auto ooBinary = clang_.createNode<OOModel::BinaryOperation>(callExpr->getSourceRange(),
																							utils_->translateBinaryOverloadOp(operatorKind));
			if (!ooExprStack_.empty())
				ooBinary->setRight(ooExprStack_.pop());
			if (!ooExprStack_.empty())
				ooBinary->setLeft(ooExprStack_.pop());
			ooExprStack_.push(ooBinary);
			break;
		}
		case CppImportUtilities::OverloadKind::Assign:
		{
			auto ooAssign = clang_.createNode<OOModel::AssignmentExpression>(callExpr->getSourceRange(),
																							utils_->translateAssignOverloadOp(operatorKind));
			if (!ooExprStack_.empty())
				ooAssign->setRight(ooExprStack_.pop());
			if (!ooExprStack_.empty())
				ooAssign->setLeft(ooExprStack_.pop());
			ooExprStack_.push(ooAssign);
			break;
		}
		case CppImportUtilities::OverloadKind::MethodCall:
		{
			auto ooCall = clang_.createNode<OOModel::MethodCallExpression>(callExpr->getSourceRange());
			for (unsigned i = 0; i < numArguments - 1; i++)
			{
				if (!ooExprStack_.empty())
					ooCall->arguments()->prepend(ooExprStack_.pop());
			}
			if (!ooExprStack_.empty())
			{
				ooCall->setCallee(ooExprStack_.pop());
				ooExprStack_.push(ooCall);
				break;
			}
			// this should not happen
			clang_.deleteNode(ooCall);
			ooExprStack_.push(utils_->createErrorExpression("METHOD CALL NO NAME FOUND", callExpr->getSourceRange()));
			break;
		}
		case CppImportUtilities::OverloadKind::ReferenceExpr:
		{
			if (!ooExprStack_.empty())
					break;
			ooExprStack_.push(utils_->createErrorExpression("Could not resolve Reference/Arrow",
																			callExpr->getSourceRange()));
			break;
		}
		case CppImportUtilities::OverloadKind::Comma:
			ooExprStack_.push(utils_->createErrorExpression("Unsupported COMMA OVERLOAD", callExpr->getSourceRange()));
			break;
		default:
			ooExprStack_.push(utils_->createErrorExpression("Unsupported Overload Expression",
																			callExpr->getSourceRange()));
			break;
	}

	return true;
}

bool ExpressionVisitor::TraverseCXXNewExpr(clang::CXXNewExpr* newExpr)
{
	auto ooNewExpr = clang_.createNode<OOModel::NewExpression>(newExpr->getSourceRange());
	if (newExpr->getInitializer() && !newExpr->isArray())
	{
		auto allocatedTypeLoc = newExpr->getAllocatedTypeSourceInfo()->getTypeLoc();

		auto methodCallExpr = clang_.createNode<OOModel::MethodCallExpression>(
					clang::SourceRange{allocatedTypeLoc.getSourceRange().getBegin(), newExpr->getSourceRange().getEnd()});
		methodCallExpr->setCallee(utils_->translateQualifiedType(allocatedTypeLoc));

		if (auto parenListExpr = llvm::dyn_cast<clang::ParenListExpr>(newExpr->getInitializer()))
		{
			methodCallExpr->setMethodCallKind(OOModel::MethodCallExpression::MethodCallKind::CallConstruction);
			for (unsigned i = 0; i < parenListExpr->getNumExprs(); i++)
				methodCallExpr->arguments()->append(translateExpression(parenListExpr->getExpr(i)));
		}
		else if (auto initListExpr = llvm::dyn_cast<clang::InitListExpr>(newExpr->getInitializer()))
		{
			methodCallExpr->setMethodCallKind(OOModel::MethodCallExpression::MethodCallKind::ListConstruction);
			if (initListExpr->getSyntacticForm()) initListExpr = initListExpr->getSyntacticForm();
			for (auto initExpr : initListExpr->inits())
				methodCallExpr->arguments()->append(translateExpression(initExpr));
		}
		else if (auto constructExpr = llvm::dyn_cast<clang::CXXConstructExpr>(newExpr->getInitializer()))
		{
			if (constructExpr->getParenOrBraceRange().getBegin().getPtrEncoding() &&
				 clang_.spelling(constructExpr->getParenOrBraceRange().getBegin()) == "{")
				methodCallExpr->setMethodCallKind(OOModel::MethodCallExpression::MethodCallKind::ListConstruction);
			else
				methodCallExpr->setMethodCallKind(OOModel::MethodCallExpression::MethodCallKind::CallConstruction);

			for (auto argument : translateArguments(constructExpr->getArgs(), constructExpr->getNumArgs()))
				methodCallExpr->arguments()->append(argument);
		}

		ooNewExpr->setNewType(methodCallExpr);
	}
	else
		ooNewExpr->setNewType(utils_->translateQualifiedType(newExpr->getAllocatedTypeSourceInfo()->getTypeLoc()));

	if (newExpr->isArray())
		ooNewExpr->dimensions()->append(translateExpression(newExpr->getArraySize()));

	ooExprStack_.push(ooNewExpr);
	return true;
}

bool ExpressionVisitor::TraverseCXXDeleteExpr(clang::CXXDeleteExpr* deleteExpr)
{
	auto ooDeleteExpr = clang_.createNode<OOModel::DeleteExpression>(deleteExpr->getSourceRange(),
																						  deleteExpr->isArrayForm());
	TraverseStmt(deleteExpr->getArgument());
	if (!ooExprStack_.empty())
		ooDeleteExpr->setExpr(ooExprStack_.pop());
	ooExprStack_.push(ooDeleteExpr);
	return true;
}

bool ExpressionVisitor::TraverseIntegerLiteral(clang::IntegerLiteral* intLit)
{
	if (intLit->getLocation().getPtrEncoding())
		ooExprStack_.push(clang_.createNode<OOModel::IntegerLiteral>(intLit->getSourceRange(),
																						 clang_.spelling(intLit->getLocation())));
	return true;
}

bool ExpressionVisitor::TraverseCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr* boolLitExpr)
{
	ooExprStack_.push(clang_.createNode<OOModel::BooleanLiteral>(boolLitExpr->getSourceRange(),
																					 boolLitExpr->getValue()));
	return true;
}

bool ExpressionVisitor::TraverseCXXNullPtrLiteralExpr(clang::CXXNullPtrLiteralExpr* nullLitExpr)
{
	ooExprStack_.push(clang_.createNode<OOModel::NullLiteral>(nullLitExpr->getSourceRange()));
	return true;
}

bool ExpressionVisitor::TraverseFloatingLiteral(clang::FloatingLiteral* floatLiteral)
{
	if (floatLiteral->getLocation().getPtrEncoding())
		ooExprStack_.push(clang_.createNode<OOModel::FloatLiteral>(floatLiteral->getSourceRange(),
																				  clang_.spelling(floatLiteral->getLocation())));
	return true;
}

bool ExpressionVisitor::TraverseCharacterLiteral(clang::CharacterLiteral* charLiteral)
{
	if (charLiteral->getLocation().getPtrEncoding())
	{
		auto value = clang_.spelling(charLiteral->getLocation());
		ooExprStack_.push(clang_.createNode<OOModel::CharacterLiteral>(charLiteral->getSourceRange(),
																							value.mid(1, value.length() - 2)));
	}
	return true;
}

bool ExpressionVisitor::TraverseStringLiteral(clang::StringLiteral* stringLiteral)
{
	OOModel::Expression* result = nullptr;
	for (auto it = stringLiteral->tokloc_begin(); it != stringLiteral->tokloc_end(); it++)
	{
		OOModel::Expression* part = nullptr;
		if (clang_.unexpandedSpelling(*it).startsWith("#"))
			part = clang_.createReference(*it);
		else
		{
			auto partSpelling = clang_.spelling(*it);
			part = clang_.createNode<OOModel::StringLiteral>(*it, partSpelling.mid(1, partSpelling.length() - 2));
		}

		if (!result)
			result = part;
		else
			result = clang_.createNode<OOModel::BinaryOperation>(clang::SourceRange(stringLiteral->getLocStart(), *it),
																				  OOModel::BinaryOperation::PLUS, result, part);
	}

	Q_ASSERT(result);
	ooExprStack_.push(result);
	return true;
}

QList<OOModel::Expression*> ExpressionVisitor::translateArguments(clang::Expr** arguments, int numArguments)
{
	QList<OOModel::Expression*> result;
	for (auto i = 0; i < numArguments; i++)
	{
		if (llvm::isa<clang::CXXDefaultArgExpr>(arguments[i]))
			// this is a default arg and is not written in the source code
			continue;
		TraverseStmt(arguments[i]);
		if (!ooExprStack_.empty())
			result.append(ooExprStack_.pop());
		else
			log_->writeError(className_, arguments[i], CppImportLogger::Reason::NOT_SUPPORTED);
	}
	return result;
}

bool ExpressionVisitor::TraverseCXXConstructExpr(clang::CXXConstructExpr* constructExpr)
{
	if (!constructExpr)
		return true;

	if (!constructExpr->getParenOrBraceRange().getBegin().getPtrEncoding())
	{
		if (constructExpr->getNumArgs() > 0)
			return TraverseStmt(*(constructExpr->child_begin()));
		else
			return true;
	}

	// check for lambda
	if (!constructExpr->getConstructor()->getParent()->isLambda())
	{
		if ((!constructExpr->isListInitialization() || !clang_.spelling(constructExpr->getLocation()).startsWith("{"))
			  && !clang_.spelling(constructExpr->getLocation()).startsWith("="))
		{
			auto ooMethodCall = clang_.createNode<OOModel::MethodCallExpression>(constructExpr->getSourceRange());
			if (auto temporaryObjectExpression = llvm::dyn_cast<clang::CXXTemporaryObjectExpr>(constructExpr))
				ooMethodCall->setCallee(utils_->translateQualifiedType(
													temporaryObjectExpression->getTypeSourceInfo()->getTypeLoc()));
			else
				ooMethodCall->setCallee(clang_.createReference(constructExpr->getLocation()));

			if (constructExpr->getParenOrBraceRange().getBegin().getPtrEncoding() &&
				 clang_.spelling(constructExpr->getParenOrBraceRange().getBegin()) == "{")
				ooMethodCall->setMethodCallKind(OOModel::MethodCallExpression::MethodCallKind::ListConstruction);
			else
				ooMethodCall->setMethodCallKind(OOModel::MethodCallExpression::MethodCallKind::CallConstruction);


			for (auto argument : translateArguments(constructExpr->getArgs(), constructExpr->getNumArgs()))
				ooMethodCall->arguments()->append(argument);
			ooExprStack_.push(ooMethodCall);
		}
		else
		{
			auto arrayInit = clang_.createNode<OOModel::ArrayInitializer>(constructExpr->getSourceRange());
			for (auto argument : translateArguments(constructExpr->getArgs(), constructExpr->getNumArgs()))
				arrayInit->values()->append(argument);

			ooExprStack_.push(arrayInit);
		}
		return true;
	}
	// clang implements lambda construct expressions weirdly, the name of the lambda is in the first argument
	if (constructExpr->getNumArgs() != 1)
		log_->writeError(className_, constructExpr, CppImportLogger::Reason::OTHER, "ContructExpr need inspectation");
	for (auto argIt = constructExpr->arg_begin(); argIt != constructExpr->arg_end(); ++argIt)
	{
		if (llvm::isa<clang::CXXDefaultArgExpr>(*argIt))
			// this is a default arg and is not written in the source code
			continue;
		TraverseStmt(*argIt);
	}
	return true;
}

bool ExpressionVisitor::TraverseCXXUnresolvedConstructExpr(clang::CXXUnresolvedConstructExpr* unresolvedConstruct)
{
	auto ooMethodCall = clang_.createNode<OOModel::MethodCallExpression>(unresolvedConstruct->getSourceRange());
	ooMethodCall->setCallee(utils_->translateQualifiedType(unresolvedConstruct->getTypeSourceInfo()->getTypeLoc()));

	if (unresolvedConstruct->getLParenLoc().getPtrEncoding() &&
		 clang_.spelling(unresolvedConstruct->getLParenLoc()) == "{")
		ooMethodCall->setMethodCallKind(OOModel::MethodCallExpression::MethodCallKind::ListConstruction);
	else
		ooMethodCall->setMethodCallKind(OOModel::MethodCallExpression::MethodCallKind::CallConstruction);

	if (unresolvedConstruct->arg_size() == 1 &&
		 llvm::dyn_cast<clang::InitListExpr>(unresolvedConstruct->getArg(0)))
	{
		auto initListExpr = llvm::dyn_cast<clang::InitListExpr>(unresolvedConstruct->getArg(0));
		Q_ASSERT(initListExpr);
		if (initListExpr->getSyntacticForm()) initListExpr = initListExpr->getSyntacticForm();
		for (auto argument : *initListExpr)
			ooMethodCall->arguments()->append(translateExpression(argument));
	}
	else
		for (auto argument : translateArguments(unresolvedConstruct->arg_begin(), unresolvedConstruct->arg_size()))
			ooMethodCall->arguments()->append(argument);

	ooExprStack_.push(ooMethodCall);
	return true;
}

bool ExpressionVisitor::TraverseParenExpr(clang::ParenExpr* parenthesizedExpr)
{
	auto ooParenExpr = clang_.createNode<OOModel::UnaryOperation>(parenthesizedExpr->getSourceRange(),
																					  OOModel::UnaryOperation::PARENTHESIS);

	ooParenExpr->setOperand(translateExpression(parenthesizedExpr->getSubExpr()));

	ooExprStack_.push(ooParenExpr);
	return true;
}

bool ExpressionVisitor::TraverseArraySubscriptExpr(clang::ArraySubscriptExpr* arraySubsrciptExpr)
{
		auto ooArrayAccess = clang_.createNode<OOModel::BinaryOperation>(arraySubsrciptExpr->getSourceRange(),
																							  OOModel::BinaryOperation::ARRAY_INDEX);
		// visit the base the base is A in the expr A[10]
		TraverseStmt(arraySubsrciptExpr->getLHS());
		if (!ooExprStack_.empty())
			ooArrayAccess->setLeft(ooExprStack_.pop());
		// visit the idx the idx is 10 in the expr A[10]
		TraverseStmt(arraySubsrciptExpr->getRHS());
		if (!ooExprStack_.empty())
			ooArrayAccess->setRight(ooExprStack_.pop());

		ooExprStack_.push(ooArrayAccess);
		return true;
}

bool ExpressionVisitor::TraverseCXXThisExpr(clang::CXXThisExpr* thisExpr)
{
	if (!thisExpr->isImplicit())
			ooExprStack_.push(clang_.createNode<OOModel::ThisExpression>(thisExpr->getSourceRange()));
	return true;
}

bool ExpressionVisitor::TraverseCXXTypeidExpr(clang::CXXTypeidExpr* typeIdExpr)
{
	auto ooTypeTrait = clang_.createNode<OOModel::TypeTraitExpression>(typeIdExpr->getSourceRange(),
																						OOModel::TypeTraitExpression::TypeTraitKind::TypeId);
	if (typeIdExpr->isTypeOperand())
		ooTypeTrait->setOperand(utils_->translateQualifiedType(typeIdExpr->getTypeOperandSourceInfo()->getTypeLoc()));
	else
	{
		TraverseStmt(typeIdExpr->getExprOperand());
		if (!ooExprStack_.empty())
			ooTypeTrait->setOperand(ooExprStack_.pop());
		else
			log_->writeError(className_, typeIdExpr, CppImportLogger::Reason::NOT_SUPPORTED);
	}
	ooExprStack_.push(ooTypeTrait);
	return true;
}

bool ExpressionVisitor::WalkUpFromOverloadExpr(clang::OverloadExpr* overloadExpr)
{
	OOModel::ReferenceExpression* ooReference = nullptr;
	if (overloadExpr->hasExplicitTemplateArgs())
		ooReference = createQualifiedReferenceWithTemplateArguments(overloadExpr->getNameInfo().getSourceRange(),
																						overloadExpr->getQualifierLoc(),
																						overloadExpr->getTemplateArgs(),
																						overloadExpr->getNumTemplateArgs());
	else
		ooReference = createQualifiedReferenceWithTemplateArguments(overloadExpr->getNameInfo().getSourceRange(),
																						overloadExpr->getQualifierLoc());

	ooExprStack_.push(ooReference);
	return true;
}

bool ExpressionVisitor::TraverseLambdaExpr(clang::LambdaExpr* lambdaExpr)
{
	// TODO: handle captures
	auto ooLambda = clang_.createNode<OOModel::LambdaExpression>(lambdaExpr->getSourceRange());
	// visit result
	if (lambdaExpr->hasExplicitResultType())
	{
		auto functionTypeLoc = lambdaExpr->getCallOperator()->getTypeSourceInfo()->getTypeLoc()
											.castAs<clang::FunctionTypeLoc>();
		ooLambda->results()->append(clang_.createNode<OOModel::FormalResult>(
																	functionTypeLoc.getReturnLoc().getSourceRange(), QString{},
																	utils_->translateQualifiedType(functionTypeLoc.getReturnLoc())));
	}

	switch (lambdaExpr->getCaptureDefault())
	{
		case clang::LambdaCaptureDefault::LCD_None:
			ooLambda->setDefaultCaptureType(OOModel::LambdaExpression::DefaultCaptureType::None);
			break;
		case clang::LambdaCaptureDefault::LCD_ByCopy:
			ooLambda->setDefaultCaptureType(OOModel::LambdaExpression::DefaultCaptureType::Value);
			break;
		case clang::LambdaCaptureDefault::LCD_ByRef:
			ooLambda->setDefaultCaptureType(OOModel::LambdaExpression::DefaultCaptureType::Reference);
			break;
	}

	for (auto it = lambdaExpr->explicit_capture_begin(); it != lambdaExpr->explicit_capture_end(); it++)
	{
		switch (it->getCaptureKind())
		{
			case clang::LambdaCaptureKind::LCK_This:
				ooLambda->captures()->append(clang_.createNode<OOModel::ThisExpression>(it->getLocation()));
				break;
			case clang::LambdaCaptureKind::LCK_ByCopy:
				ooLambda->captures()->append(clang_.createReference(it->getLocation()));
				break;
			case clang::LambdaCaptureKind::LCK_ByRef:
				{
					auto reference = clang_.createReference(it->getLocation());
					if (lambdaExpr->getCaptureDefault() != clang::LambdaCaptureDefault::LCD_ByRef)
						ooLambda->captures()->append(new OOModel::UnaryOperation{
																  OOModel::UnaryOperation::OperatorTypes::ADDRESSOF, reference});
					else
						ooLambda->captures()->append(reference);
				}
				break;
			default:
				Q_ASSERT(false);
		}
	}

	// visit body
	baseVisitor_->pushOOStack(ooLambda->body());
	baseVisitor_->TraverseStmt(lambdaExpr->getBody());
	baseVisitor_->popOOStack();
	// visit arguments
	auto callOperator = lambdaExpr->getCallOperator();
	for (auto it = callOperator->param_begin(); it != callOperator->param_end(); ++it)
	{
		auto arg = clang_.createNamedNode<OOModel::FormalArgument>(*it);
		if (auto type = utils_->translateQualifiedType((*it)->getTypeSourceInfo()->getTypeLoc()))
			arg->setTypeExpression(type);
		ooLambda->arguments()->append(arg);
	}

	ooExprStack_.push(ooLambda);
	return true;
}

bool ExpressionVisitor::TraverseConditionalOperator(clang::ConditionalOperator* conditionalOperator)
{
	auto ooConditionalExpr = clang_.createNode<OOModel::ConditionalExpression>(conditionalOperator->getSourceRange());
	// Traverse condition
	TraverseStmt(conditionalOperator->getCond());
	if (!ooExprStack_.empty())
		ooConditionalExpr->setCondition(ooExprStack_.pop());
	// Traverse true part
	TraverseStmt(conditionalOperator->getTrueExpr());
	if (!ooExprStack_.empty())
		ooConditionalExpr->setTrueExpression(ooExprStack_.pop());
	// Traverse false part
	TraverseStmt(conditionalOperator->getFalseExpr());
	if (!ooExprStack_.empty())
		ooConditionalExpr->setFalseExpression(ooExprStack_.pop());

	ooExprStack_.push(ooConditionalExpr);
	return true;
}

bool ExpressionVisitor::TraverseCXXThrowExpr(clang::CXXThrowExpr* throwExpr)
{
	auto ooThrow = clang_.createNode<OOModel::ThrowExpression>(throwExpr->getSourceRange());
	// visit throw expression
	if (auto subExpr = throwExpr->getSubExpr())
	{
		TraverseStmt(subExpr);
		if (!ooExprStack_.empty())
			ooThrow->setExpr(ooExprStack_.pop());
	}
	else
		ooThrow->setExpr(new OOModel::EmptyExpression{});
	ooExprStack_.push(ooThrow);
	return true;
}

bool ExpressionVisitor::TraverseCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr* tempObjectExpr)
{
	return TraverseCXXConstructExpr(tempObjectExpr);
}

bool ExpressionVisitor::TraverseInitListExpr(clang::InitListExpr* initListExpr)
{
	if (initListExpr->getSyntacticForm()) initListExpr = initListExpr->getSyntacticForm();
	auto ooArrayInit = clang_.createNode<OOModel::ArrayInitializer>(initListExpr->getSourceRange());
	for (auto initExpr : *initListExpr)
	{
		TraverseStmt(initExpr);
		if (!ooExprStack_.empty()) ooArrayInit->values()->append(ooExprStack_.pop());
	}
	ooExprStack_.push(ooArrayInit);
	return true;
}

bool ExpressionVisitor::TraverseUnaryExprOrTypeTraitExpr(clang::UnaryExprOrTypeTraitExpr* typeTrait)
{
	OOModel::TypeTraitExpression::TypeTraitKind typeTraitKind;
	if (clang::UETT_SizeOf == typeTrait->getKind())
		typeTraitKind = OOModel::TypeTraitExpression::TypeTraitKind::SizeOf;
	else if (clang::UETT_AlignOf == typeTrait->getKind())
		typeTraitKind = OOModel::TypeTraitExpression::TypeTraitKind::AlignOf;
	else
	{
		log_->writeError(className_, typeTrait, CppImportLogger::Reason::NOT_SUPPORTED);
		return true;
	}

	OOModel::Expression* argument = nullptr;
	if (typeTrait->isArgumentType())
		argument = utils_->translateQualifiedType(typeTrait->getArgumentTypeInfo()->getTypeLoc());
	else
	{
		TraverseStmt(typeTrait->getArgumentExpr());
		if (!ooExprStack_.empty())
			argument = ooExprStack_.pop();
		else
			argument = utils_->createErrorExpression("Unsupported argument", typeTrait->getSourceRange());
	}
	Q_ASSERT(argument);
	ooExprStack_.push(clang_.createNode<OOModel::TypeTraitExpression>(typeTrait->getSourceRange(), typeTraitKind,
																							argument));
	return true;
}


OOModel::ReferenceExpression* ExpressionVisitor::createQualifiedReferenceWithTemplateArguments
(clang::SourceRange sourceRange, const clang::NestedNameSpecifierLoc qualifier,
 const clang::TemplateArgumentLoc* templateArgs, unsigned numTArgs, clang::Expr* base)
{
	auto ooRef = utils_->setReferencePrefix(clang_.createReference(sourceRange), qualifier, base);
	if (templateArgs)
		for (unsigned i = 0; i < numTArgs; i++)
			ooRef->typeArguments()->append(utils_->translateTemplateArgument(templateArgs[i]));
	return ooRef;
}

bool ExpressionVisitor::TraverseBinaryOp(clang::BinaryOperator* binaryOperator)
{
	auto ooLeft = translateExpression(binaryOperator->getLHS());
	auto ooRight = translateExpression(binaryOperator->getRHS());

	clang::BinaryOperatorKind opcode = binaryOperator->getOpcode();
	OOModel::Expression* ooBinaryOp = nullptr;
	if (opcode == clang::BO_Comma)
		ooBinaryOp = clang_.createNode<OOModel::CommaExpression>(binaryOperator->getSourceRange(), ooLeft, ooRight);
	else
		ooBinaryOp = clang_.createNode<OOModel::BinaryOperation>(binaryOperator->getSourceRange(),
																					utils_->translateBinaryOp(opcode), ooLeft, ooRight);
	ooExprStack_.push(ooBinaryOp);
	return true;
}

bool ExpressionVisitor::TraverseAssignment(clang::BinaryOperator* binaryOperator)
{
	auto ooBinOp = clang_.createNode<OOModel::AssignmentExpression>(binaryOperator->getSourceRange(),
																					utils_->translateAssignOp(binaryOperator->getOpcode()));
	// left
	TraverseStmt(binaryOperator->getLHS());
	if (!ooExprStack_.empty()) ooBinOp->setLeft(ooExprStack_.pop());
	else log_->writeError(className_, binaryOperator->getLHS(), CppImportLogger::Reason::NOT_SUPPORTED);
	// right
	TraverseStmt(binaryOperator->getRHS());
	if (!ooExprStack_.empty()) ooBinOp->setRight(ooExprStack_.pop());
	else log_->writeError(className_, binaryOperator->getRHS(), CppImportLogger::Reason::NOT_SUPPORTED);

	ooExprStack_.push(ooBinOp);
	return true;
}

bool ExpressionVisitor::TraverseUnaryOp(clang::UnaryOperator* unaryOperator)
{
	clang::UnaryOperatorKind opcode = unaryOperator->getOpcode();
	if (opcode == clang::UO_Extension || opcode == clang::UO_Real || opcode == clang::UO_Imag)
	{
		log_->writeError(className_, unaryOperator, CppImportLogger::Reason::NOT_SUPPORTED);
		unaryOperator->dump();
		// just convert the subexpression
		log_->unaryOpNotSupported(opcode);
		return TraverseStmt(unaryOperator->getSubExpr());
	}
	auto ooUnaryOp = clang_.createNode<OOModel::UnaryOperation>(unaryOperator->getSourceRange(),
																					utils_->translateUnaryOp(opcode));
	// subexpr
	TraverseStmt(unaryOperator->getSubExpr());
	if (!ooExprStack_.empty()) ooUnaryOp->setOperand(ooExprStack_.pop());
	else log_->writeError(className_, unaryOperator->getSubExpr(), CppImportLogger::Reason::NOT_SUPPORTED);

	ooExprStack_.push(ooUnaryOp);
	return true;
}

bool ExpressionVisitor::TraverseExplCastExpr(clang::ExplicitCastExpr* castExpr, OOModel::CastExpression::CastKind kind)
{
	if (kind == OOModel::CastExpression::CastKind::FunctionalCast)
	{
		auto ooMethodCall = clang_.createNode<OOModel::MethodCallExpression>(castExpr->getSourceRange());
		ooMethodCall->setCallee(utils_->translateQualifiedType(castExpr->getTypeInfoAsWritten()->getTypeLoc()));

		auto functionalCastExpr = llvm::dyn_cast<clang::CXXFunctionalCastExpr>(castExpr);
		Q_ASSERT(functionalCastExpr);
		if (functionalCastExpr->getLParenLoc().getPtrEncoding() &&
			 clang_.spelling(functionalCastExpr->getLParenLoc()) == "{")
			ooMethodCall->setMethodCallKind(OOModel::MethodCallExpression::MethodCallKind::ListConstruction);
		else
			ooMethodCall->setMethodCallKind(OOModel::MethodCallExpression::MethodCallKind::CallConstruction);

		if (auto initListExpr = llvm::dyn_cast<clang::InitListExpr>(castExpr->getSubExprAsWritten()))
		{
			if (initListExpr->getSyntacticForm()) initListExpr = initListExpr->getSyntacticForm();
			for (auto argument : *initListExpr)
				ooMethodCall->arguments()->append(translateExpression(argument));
		}
		else
			ooMethodCall->arguments()->append(translateExpression(castExpr->getSubExprAsWritten()));

		ooExprStack_.push(ooMethodCall);
		return true;
	}

	auto ooCast = clang_.createNode<OOModel::CastExpression>(castExpr->getSourceRange(), kind);

	// setType to cast to
	ooCast->setType(utils_->translateQualifiedType(castExpr->getTypeInfoAsWritten()->getTypeLoc()));
	// visit subexpr
	TraverseStmt(castExpr->getSubExprAsWritten());
	if (!ooExprStack_.empty()) ooCast->setExpr(ooExprStack_.pop());

	ooExprStack_.push(ooCast);
	return true;
}

}
