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

#include "ClangAstConsumer.h"

#include "OOModel/src/elements/StatementItemList.h"
#include "OOModel/src/elements/CommentStatementItem.h"

namespace CppImport {

CppImport::Comment::Comment(clang::RawComment* rawComment, const clang::SourceManager& sourceManager)
	: rawComment_{rawComment}
{
	text_ = QString::fromStdString(rawComment->getRawText(sourceManager).str());
	auto presumedLocationStart = sourceManager.getPresumedLoc(rawComment->getLocStart());
	fileName_ = presumedLocationStart.getFilename();
	lineStart_ = presumedLocationStart.getLine();
	lineEnd_ = sourceManager.getPresumedLineNumber(rawComment->getLocEnd());
}

void CppImport::Comment::setNode(Model::Node* node)
{
	// assert that the client can only associate the comment with a node once.
	Q_ASSERT(node && !node_);
	node_ = node;
}

void Comment::insertIntoItemList(OOModel::StatementItemList* itemList, int index)
{
	setNode(itemList);
	commentStatementItem_ = new OOModel::CommentStatementItem{};
	commentStatementItem_->setCommentNode(new Comments::CommentNode{text()});

	if (index >= 0)
		itemList->insert(index, commentStatementItem_);
	else
		itemList->append(commentStatementItem_);
}

void Comment::removeFromItemList()
{
	Q_ASSERT(commentStatementItem_);
	auto itemList = DCast<OOModel::StatementItemList>(node());
	Q_ASSERT(itemList);

	itemList->remove(commentStatementItem_);

	// in this special case the client can afterwards reassociate the comment.
	node_ = nullptr;

	SAFE_DELETE(commentStatementItem_);
	commentStatementItem_ = nullptr;
}

QString Comment::text()
{
	auto lines = text_.split('\n');
	if (!lines.first().startsWith("/*"))
	{
		static QRegularExpression singleLineRegex{"^\\s*//\\s?(.*)$"};
		for (auto i = 0; i < lines.size(); i++)
		{
			auto match = singleLineRegex.match(lines[i]);
			if (match.hasMatch()) lines[i] = match.captured(1);
		}
	}
	else
	{
		static QRegularExpression firstLineRegex{"^/\\*+<?\\s?(.*)$"};
		auto match = firstLineRegex.match(lines.first());
		if (match.hasMatch()) lines[0] = match.captured(1);

		static QRegularExpression middleLineRegex{"^\\s*\\*\\s(.*)$"};
		for (auto i = 1; i < lines.size(); i++)
		{
			match = middleLineRegex.match(lines[i]);
			if (match.hasMatch()) lines[i] = match.captured(1);
		}

		static QRegularExpression lastLineRegex{"(.*(^|[^\\*]))\\*+/\\s*$"};
		match = lastLineRegex.match(lines.last());
		if (match.hasMatch())
		{
			auto captured = match.captured(1);
			while (captured.endsWith('*')) captured = captured.left(captured.size() - 1);
			lines[lines.size() - 1] = captured;
		}

		for (auto i = 1; i < lines.size() - 1; i++) if (lines[i].trimmed() == "*") lines[i] = "";
		if (lines.first().trimmed().isEmpty()) lines.removeFirst();
		if (lines.last().trimmed().isEmpty()) lines.removeLast();
	}
	return lines.join('\n');
}

}
