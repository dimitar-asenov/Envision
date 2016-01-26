/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "DeclarationVisitor.h"
#include "ExpressionVisitor.h"
#include "StatementVisitor.h"
#include "ElementVisitor.h"

#include "OOModel/src/statements/Block.h"
#include "OOModel/src/statements/BreakStatement.h"
#include "OOModel/src/statements/CaseStatement.h"
#include "OOModel/src/statements/ContinueStatement.h"
#include "OOModel/src/statements/DeclarationStatement.h"
#include "OOModel/src/statements/ExpressionStatement.h"
#include "OOModel/src/statements/ForEachStatement.h"
#include "OOModel/src/statements/IfStatement.h"
#include "OOModel/src/statements/LoopStatement.h"
#include "OOModel/src/statements/ReturnStatement.h"
#include "OOModel/src/statements/SwitchStatement.h"
#include "OOModel/src/statements/TryCatchFinallyStatement.h"
#include "OOModel/src/statements/AssertStatement.h"
#include "OOModel/src/statements/SynchronizedStatement.h"
#include "OOModel/src/elements/StatementItem.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "OOModel/src/elements/CommentStatementItem.h"
#include "Comments/src/nodes/CommentNode.h"

using namespace Export;
using namespace OOModel;

namespace CppExport {

SourceFragment* StatementVisitor::visit(StatementItem* statementItem)
{
	if (auto castStatement = DCast<Block>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<BreakStatement>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<CaseStatement>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<ContinueStatement>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<DeclarationStatement>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<ExpressionStatement>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<ForEachStatement>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<IfStatement>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<LoopStatement>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<ReturnStatement>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<SwitchStatement>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<TryCatchFinallyStatement>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<AssertStatement>(statementItem)) return visit(castStatement);
	if (auto castStatement = DCast<CommentStatementItem>(statementItem)) return visit(castStatement);

	// TODO: handle comments
	auto fragment = new CompositeFragment{statementItem};
	*fragment << "UNKNOWN STATEMENT TYPE";
	return fragment;
}

SourceFragment* StatementVisitor::visit(Block* statement)
{
	auto fragment = new CompositeFragment{statement};
	*fragment << list(statement->items(), StatementVisitor(data()),
							statement->items()->size() > 1 ? "body" : "bodyNoBraces");
	return fragment;
}

SourceFragment* StatementVisitor::visit(BreakStatement* statement)
{
	return new TextFragment{statement, "break;"};
}

SourceFragment* StatementVisitor::visit(CaseStatement* statement)
{
	auto fragment = new CompositeFragment{statement};
	if (statement->caseExpression())
		*fragment << "case " << expression(statement->caseExpression()) << ":";
	else *fragment << "default:";

	*fragment << list(statement->body(), StatementVisitor(data()),
							statement->body()->size() > 1 ? "body" : "bodyNoBraces");

	return fragment;
}

SourceFragment* StatementVisitor::visit(ContinueStatement* statement)
{
	return new TextFragment{statement, "continue;"};
}

SourceFragment* StatementVisitor::visit(DeclarationStatement* statement)
{
	return declaration(statement->declaration());
}

SourceFragment* StatementVisitor::visit(ExpressionStatement* statement)
{
	auto fragment = new CompositeFragment{statement};
	if (DCast<EmptyExpression>(statement->expression()))
		*fragment << "\n";
	else
		*fragment << expression(statement->expression()) << ";";
	return fragment;
}

SourceFragment* StatementVisitor::visit(ForEachStatement* statement)
{
	auto fragment = new CompositeFragment{statement};

	required(statement, statement->varType(), "explicit loop variable type");
	*fragment << "for (";
	if (statement->varType()) *fragment << expression(statement->varType()) << " ";
	*fragment << statement->varNameNode() << " : ";
	*fragment << expression(statement->collection()) << ")";
	*fragment << list(statement->body(), StatementVisitor(data()), blockStyle(statement->body()));
	return fragment;
}

SourceFragment* StatementVisitor::visit(IfStatement* statement)
{
	auto fragment = new CompositeFragment{statement};
	*fragment << "if (" << expression(statement->condition()) << ")";
	*fragment << list(statement->thenBranch(), StatementVisitor(data()), blockStyle(statement->thenBranch()));
	if (!statement->elseBranch()->isEmpty())
	{
		*fragment << "else";
		*fragment << list(statement->elseBranch(), StatementVisitor(data()), blockStyle(statement->elseBranch()));
	}
	return fragment;
}

SourceFragment* StatementVisitor::visit(LoopStatement* statement)
{
	auto fragment = new CompositeFragment{statement};

	if (statement->loopKind() == LoopStatement::LoopKind::PreCheck) // for and while loops
	{
		if (!statement->initStep() && !statement->updateStep()) // while loop
		{
			*fragment << "while (";
			if (statement->condition()) *fragment << expression(statement->condition());
			*fragment << ")" << list(statement->body(), StatementVisitor(data()), blockStyle(statement->body()));
		}
		else // for loop
		{
			*fragment << "for (";
			if (statement->initStep()) *fragment << expression(statement->initStep());
			*fragment << " ; ";
			if (statement->condition()) *fragment << expression(statement->condition());
			*fragment << " ; ";
			if (statement->updateStep()) *fragment << expression(statement->updateStep());
			*fragment << ")" << list(statement->body(), StatementVisitor(data()), blockStyle(statement->body()));
		}
	}
	else	// do loop
	{
		*fragment << "do" << list(statement->body(), StatementVisitor(data()), blockStyle(statement->body())) << "while (";
		if (statement->condition()) *fragment << expression(statement->condition());
		*fragment << ");";
	}

	return fragment;
}

QString StatementVisitor::blockStyle(Model::List* block)
{
	if (block->size() == 1)
	{
		if (auto parentIf = DCast<OOModel::IfStatement>(block->parent()))
		{
			if (block == parentIf->thenBranch())
			{
				if (!parentIf->elseBranch()->isEmpty() &&
					 !Model::Node::childrenOfType<OOModel::IfStatement>(block).isEmpty())
					return "body";

				if (auto ifStatement = DCast<OOModel::IfStatement>(block->first()))
					if (!ifStatement->elseBranch()->isEmpty() || !parentIf->elseBranch()->isEmpty())
						return "body";
			}
			else if (block == parentIf->elseBranch())
					if (DCast<OOModel::IfStatement>(block->first()))
							return "sameLineIf";
		}

		return "bodyNoBraces";
	}

	return "body";
}

SourceFragment* StatementVisitor::visit(ReturnStatement* statement)
{
	auto fragment = new CompositeFragment{statement};

	if (statement->values()->size() > 1)
		error(statement->values(), "Cannot have more than one return value in Cpp");

	if (statement->values()->isEmpty()) *fragment << "return;";
	else *fragment << "return " << expression(statement->values()->first()) << ";";

	return fragment;
}

SourceFragment* StatementVisitor::visit(SwitchStatement* statement)
{
	auto fragment = new CompositeFragment{statement};
	*fragment << "switch (" << expression(statement->switchExpression()) << ")";
	*fragment << list(statement->body(), StatementVisitor(data()), "body");
	return fragment;
}

SourceFragment* StatementVisitor::visit(TryCatchFinallyStatement* statement)
{
	auto fragment = new CompositeFragment{statement};
	*fragment << "try";
	*fragment << list(statement->tryBody(), StatementVisitor(data()), "body");
	*fragment << list(statement->catchClauses(), ElementVisitor(data()), "vertical");
	if (!statement->finallyBody()->isEmpty())
	{
		*fragment << "finally";
		*fragment << list(statement->finallyBody(), StatementVisitor(data()), "body");
	}
	return fragment;
}

SourceFragment* StatementVisitor::visit(AssertStatement* statement)
{
	auto fragment = new CompositeFragment{statement};
	*fragment << "assert " << expression(statement->expression()) << ";";
	return fragment;
}

SourceFragment* StatementVisitor::visit(OOModel::SynchronizedStatement* statement)
{
	notAllowed(statement);
	return {};
}

SourceFragment* StatementVisitor::visit(OOModel::CommentStatementItem* statement)
{
	auto fragment = new CompositeFragment{statement, "sections"};
	for (auto line : *statement->commentNode()->lines())
		*fragment << line;
	return fragment;
}

}
