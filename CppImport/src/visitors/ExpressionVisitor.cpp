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

namespace CppImport {

ExpressionVisitor::ExpressionVisitor(ClangAstVisitor* visitor, CppImportLogger* log, CppImportUtilities* utils)
: baseVisitor_(visitor), log_(log) , utils_(utils)
{}


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
	TraverseStmt(throwExpr->getSubExpr());
	if(!ooExprStack_.empty())
		ooThrow->setExpr(ooExprStack_.pop());

	ooExprStack_.push(ooThrow);
	return true;
}

bool ExpressionVisitor::VisitDependentScopeDeclRefExpr(clang::DependentScopeDeclRefExpr* depenentScope)
{
	// TODO: this should be extended to support also the dependent type
	QString ref = QString::fromStdString(depenentScope->getDeclName().getAsString());
	OOModel::ReferenceExpression* ooRef = new OOModel::ReferenceExpression(ref);
	ooExprStack_.push(ooRef);
	return true;
}

bool ExpressionVisitor::TraverseCXXMemberCallExpr(clang::CXXMemberCallExpr* callExpr)
{
//	OOModel::MethodCallExpression* ooMCall = new OOModel::MethodCallExpression
//			(QString::fromStdString(callExpr->getMethodDecl()->getNameAsString()));

//	// visit arguments
//	clang::ExprIterator argIt = callExpr->arg_begin();
//	for(;argIt!=callExpr->arg_end();++argIt)
//	{
//		TraverseStmt(*argIt);
//		ooMCall->arguments()->append(ooExprStack_.pop());
//	}

//	// set target
//	// TODO multiple levels possible
//	if(auto callee = llvm::dyn_cast<clang::MemberExpr>(callExpr->getCallee()))
//	{
//		TraverseStmt(callee->getBase());
//		if(!ooExprStack_.empty())
//			ooMCall->ref()->setPrefix(ooExprStack_.pop());
//	}

//	// type arguments

//	ooExprStack_.push(ooMCall);
//	return true;
	return TraverseCallExpr(callExpr);
}

bool ExpressionVisitor::TraverseCallExpr(clang::CallExpr* callExpr)
{
	if(auto callee = callExpr->getDirectCallee())
	{
		OOModel::MethodCallExpression* ooMCall = new OOModel::MethodCallExpression
				(QString::fromStdString(callee->getNameAsString()));

		// visit arguments
		clang::ExprIterator argIt = callExpr->arg_begin();
		for(;argIt!=callExpr->arg_end();++argIt)
		{
			TraverseStmt(*argIt);
			if(!ooExprStack_.empty())
				ooMCall->arguments()->append(ooExprStack_.pop());
			else
				log_->writeError(className_,QString("not supported"),*argIt);
		}

		// set target
		// TODO multiple levels possible
		if(auto callee = llvm::dyn_cast<clang::MemberExpr>(callExpr->getCallee()))
		{
			TraverseStmt(callee->getBase());
			if(!ooExprStack_.empty())
				ooMCall->ref()->setPrefix(ooExprStack_.pop());
		}

		// type arguments
		// TODO this might need some inspection
		if(auto specArgs = callee->getTemplateSpecializationArgs())
		{
			for(unsigned i=0; i < specArgs->size(); i++)
			{
				// TODO what if its not a type
				if(specArgs->get(i).getKind() == clang::TemplateArgument::ArgKind::Type)
					if(auto type = utils_->convertClangType(specArgs->get(i).getAsType()))
						ooMCall->ref()->typeArguments()->append(type);
			}
		}
		ooExprStack_.push(ooMCall);
	}
	else
	{
		//TODO
	}
	return true;
}

bool ExpressionVisitor::TraverseCXXNewExpr(clang::CXXNewExpr* newExpr)
{
	OOModel::NewExpression* ooNewExpr = new OOModel::NewExpression();
	ooNewExpr->setNewType(utils_->convertClangType(newExpr->getAllocatedType()));
	if(newExpr->isArray())
	{
		TraverseStmt(newExpr->getArraySize());
		if(!ooExprStack_.empty())
			ooNewExpr->setAmount(ooExprStack_.pop());
	}

	ooExprStack_.push(ooNewExpr);
	return true;
}

bool ExpressionVisitor::VisitIntegerLiteral(clang::IntegerLiteral* intLit)
{
	OOModel::IntegerLiteral* ooIntLit = new OOModel::IntegerLiteral();
	ooIntLit->setValue(intLit->getValue().getLimitedValue());
	ooExprStack_.push(ooIntLit);
	return true;
}

bool ExpressionVisitor::VisitCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr* boolLitExpr)
{
	OOModel::BooleanLiteral* ooBoolLit = new OOModel::BooleanLiteral();
	ooBoolLit->setValue(boolLitExpr->getValue());
	ooExprStack_.push(ooBoolLit);
	return true;
}

bool ExpressionVisitor::VisitCXXNullPtrLiteralExpr(clang::CXXNullPtrLiteralExpr*)
{
	ooExprStack_.push(new OOModel::NullLiteral());
	return true;
}

bool ExpressionVisitor::VisitFloatingLiteral(clang::FloatingLiteral* floatLiteral)
{
	OOModel::FloatLiteral* ooFloatLit = new OOModel::FloatLiteral();
	ooFloatLit->setValue(floatLiteral->getValueAsApproximateDouble());
	ooExprStack_.push(ooFloatLit);
	return true;
}

bool ExpressionVisitor::VisitCharacterLiteral(clang::CharacterLiteral* charLiteral)
{
	OOModel::CharacterLiteral* ooCharLit = new OOModel::CharacterLiteral();
	ooCharLit->setValue(QChar(charLiteral->getValue()));
	ooExprStack_.push(ooCharLit);
	return true;
}

bool ExpressionVisitor::VisitStringLiteral(clang::StringLiteral* stringLiteral)
{
	OOModel::StringLiteral* ooStringLit = new OOModel::StringLiteral();
	ooStringLit->setValue(QString::fromStdString(stringLiteral->getBytes().str()));
	ooExprStack_.push(ooStringLit);
	return true;
}

bool ExpressionVisitor::VisitDeclRefExpr(clang::DeclRefExpr* declRefExpr)
{
	// TODO: namespace resolution
	OOModel::ReferenceExpression* refExpr = new OOModel::ReferenceExpression();
	refExpr->setName(QString::fromStdString(declRefExpr->getDecl()->getNameAsString()));
	ooExprStack_.push(refExpr);
	return true;
}

bool ExpressionVisitor::VisitCXXUnresolvedConstructorExpr(clang::CXXUnresolvedConstructExpr*)
{
	ooExprStack_.push(new OOModel::Expression());
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

bool ExpressionVisitor::VisitCXXThisExpr(clang::CXXThisExpr* thisExpr)
{
	if(!thisExpr->isImplicit())
	{
			ooExprStack_.push(new OOModel::ThisExpression());
	}
	return true;
}

bool ExpressionVisitor::VisitMemberExpr(clang::MemberExpr* memberExpr)
{
	if(memberExpr->isImplicitAccess())
	{
		OOModel::ReferenceExpression* ooRef = new OOModel::ReferenceExpression();
		ooRef->ref()->setName(QString::fromStdString(memberExpr->getMemberDecl()->getNameAsString()));
		ooExprStack_.push(ooRef);
	}
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
			TraverseStmt(*argIt);
			if(!ooExprStack_.empty())
				ooMethodCall->arguments()->append(ooExprStack_.pop());
			else
				log_->writeError(className_,"Could not convert", *argIt);
		}
		ooExprStack_.push(ooMethodCall);
		return true;
	}

	log_->writeError(className_, QString("Not handled yet"), constructExpr);
	return true;
}

bool ExpressionVisitor::TraverseLambdaExpr(clang::LambdaExpr* lambdaExpr)
{
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
		OOModel::Expression* type = utils_->convertClangType((*it)->getType());
		if(type) arg->setTypeExpression(type);
		ooLambda->arguments()->append(arg);
	}

	ooExprStack_.push(ooLambda);
	return true;
}

OOModel::Expression* ExpressionVisitor::getLastExpression()
{
	if(!ooExprStack_.empty())
		return ooExprStack_.pop();
	return nullptr;
}

bool ExpressionVisitor::TraverseInitListExpr(clang::InitListExpr* initListExpr)
{
	OOModel::ArrayInitializer* ooArrayInit = new OOModel::ArrayInitializer();
	for(auto iter = initListExpr->begin(); iter!=initListExpr->end(); ++iter)
	{
		TraverseStmt(*iter);
		if(!ooExprStack_.empty())
			ooArrayInit->values()->append(ooExprStack_.pop());
	}
	ooExprStack_.push(ooArrayInit);
	return true;
}

bool ExpressionVisitor::TraverseBinaryOp(clang::BinaryOperator* binaryOperator)
{
	OOModel::Expression* ooLeft = nullptr;
	OOModel::Expression* ooRight = nullptr;
	OOModel::BinaryOperation* ooBinOp = nullptr;
	OOModel::CommaExpression* ooComma  = nullptr;
	clang::BinaryOperatorKind opcode = binaryOperator->getOpcode();
	if(opcode == clang::BO_Comma)
	{
		ooComma = new OOModel::CommaExpression();
	}
	else
	{
		ooBinOp = new OOModel::BinaryOperation();
		OOModel::BinaryOperation::OperatorTypes ooOperatorType =
				utils_->convertClangOpcode(opcode);
		ooBinOp->setOp(ooOperatorType);
	}

	TraverseStmt(binaryOperator->getLHS());
	if(!ooExprStack_.empty())
		ooLeft = ooExprStack_.pop();
	else
		log_->writeError(className_, QString("BOP: LHSExpr not supported"), binaryOperator->getLHS());
	TraverseStmt(binaryOperator->getRHS());
	if(!ooExprStack_.empty())
		ooRight = ooExprStack_.pop();
	else
		log_->writeError(className_, QString("BOP: RHSExpr not supported"), binaryOperator->getRHS());
	if(ooBinOp)
	{
		ooBinOp->setLeft(ooLeft);
		ooBinOp->setRight(ooRight);
	}
	else
	{
		ooComma->setLeft(ooLeft);
		ooComma->setRight(ooRight);
	}

	if(ooBinOp) ooExprStack_.push(ooBinOp);
	else ooExprStack_.push(ooComma);

	return true;
}

bool ExpressionVisitor::TraverseAssignment(clang::BinaryOperator* binaryOperator)
{
	OOModel::AssignmentExpression::AssignmentTypes ooOperatorType =
			utils_->convertClangAssignOpcode(binaryOperator->getOpcode());
	OOModel::AssignmentExpression* ooBinOp = new OOModel::AssignmentExpression();

	ooBinOp->setOp(ooOperatorType);
	TraverseStmt(binaryOperator->getLHS());
	if(!ooExprStack_.empty())
		ooBinOp->setLeft(ooExprStack_.pop());
	else
		log_->writeError(className_, QString("BOP: LHSExpr not supported"), binaryOperator->getLHS());
	TraverseStmt(binaryOperator->getRHS());
	if(!ooExprStack_.empty())
		ooBinOp->setRight(ooExprStack_.pop());
	else
		log_->writeError(className_, QString("BOP: RHSExpr not supported"), binaryOperator->getRHS());

	ooExprStack_.push(ooBinOp);
	return true;
}

bool ExpressionVisitor::TraverseUnaryOp(clang::UnaryOperator* unaryOperator)
{
	clang::UnaryOperatorKind opcode = unaryOperator->getOpcode();
	if(opcode == clang::UO_Extension || opcode == clang::UO_Real || opcode == clang::UO_Imag)
	{
		log_->writeError(className_, QString("UNARY OP NOT SUPPORTED"), unaryOperator);
		unaryOperator->dump();
		// just convert the subexpression
		log_->unaryOpNotSupported(opcode);
		return TraverseStmt(unaryOperator->getSubExpr());
	}
	OOModel::UnaryOperation::OperatorTypes ooOperatorType =
			utils_->convertUnaryOpcode(opcode);
	OOModel::UnaryOperation* ooUnaryOp = new OOModel::UnaryOperation();
	ooUnaryOp->setOp(ooOperatorType);
	TraverseStmt(unaryOperator->getSubExpr());
	if(!ooExprStack_.empty())
		ooUnaryOp->setOperand(ooExprStack_.pop());
	else
		log_->writeError(className_, QString("UOP: SubExpr not supported"), unaryOperator->getSubExpr());

	ooExprStack_.push(ooUnaryOp);
	return true;
}

bool ExpressionVisitor::TraverseExplCastExpr(clang::ExplicitCastExpr* castExpr, OOModel::CastExpression::CastKind kind)
{
	OOModel::CastExpression* ooCast = new OOModel::CastExpression(kind);
	// setType to cast to
	ooCast->setType(utils_->convertClangType(castExpr->getType()));
	// visit subexpr
	TraverseStmt(castExpr->getSubExprAsWritten());
	if(!ooExprStack_.empty())
		ooCast->setExpr(ooExprStack_.pop());

	ooExprStack_.push(ooCast);
	return true;
}

}
