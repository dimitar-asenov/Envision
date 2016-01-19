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

#pragma once

#include "../cppexport_api.h"

#include "Export/src/Visitor.h"

namespace OOModel {
	class Block;
	class BreakStatement;
	class CaseStatement;
	class ContinueStatement;
	class DeclarationStatement;
	class ExpressionStatement;
	class ForEachStatement;
	class IfStatement;
	class LoopStatement;
	class ReturnStatement;
	class SwitchStatement;
	class TryCatchFinallyStatement;
	class AssertStatement;
	class SynchronizedStatement;
	class StatementItem;
	class CommentStatementItem;
}

namespace CppExport {

class DeclarationVisitor;
class ExpressionVisitor;
class ElementVisitor;

class StatementVisitor
:public Export::Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>
{
	public:
		using Visitor::Visitor;

		Export::SourceFragment* visit(OOModel::Block* statement);
		Export::SourceFragment* visit(OOModel::BreakStatement* statement);
		Export::SourceFragment* visit(OOModel::CaseStatement* statement);
		Export::SourceFragment* visit(OOModel::ContinueStatement* statement);
		Export::SourceFragment* visit(OOModel::DeclarationStatement* statement);
		Export::SourceFragment* visit(OOModel::ExpressionStatement* statement);
		Export::SourceFragment* visit(OOModel::ForEachStatement* statement);
		Export::SourceFragment* visit(OOModel::IfStatement* statement);
		Export::SourceFragment* visit(OOModel::LoopStatement* statement);
		Export::SourceFragment* visit(OOModel::ReturnStatement* statement);
		Export::SourceFragment* visit(OOModel::SwitchStatement* statement);
		Export::SourceFragment* visit(OOModel::TryCatchFinallyStatement* statement);
		Export::SourceFragment* visit(OOModel::AssertStatement* statement);
		Export::SourceFragment* visit(OOModel::SynchronizedStatement* statement);
		Export::SourceFragment* visit(OOModel::CommentStatementItem* statement);

		Export::SourceFragment* visit(OOModel::StatementItem* statementItem);
	private:
		QString blockStyle(Model::List* block);
};

}
