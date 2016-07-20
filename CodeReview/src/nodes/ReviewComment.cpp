/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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
#include "ReviewComment.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"
#include "ModelBase/src/nodes/TypedList.hpp"

template class Model::TypedList<CodeReview::ReviewComment>;

namespace CodeReview
{

DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS(ReviewComment)

DEFINE_ATTRIBUTE(ReviewComment, commentNode, CommentNode, false, false, true)
DEFINE_ATTRIBUTE(ReviewComment, username, Text, false, false, true)
DEFINE_ATTRIBUTE(ReviewComment, dateString, Text, false, false, true)

ReviewComment::ReviewComment(Comments::CommentNode* commentNode, qint64 date, Model::Node* parent) :
	Super{parent, ReviewComment::getMetaData()}
{
	setUsername(systemUsername());
	setDate(date);
	setComment(commentNode);
}

QString ReviewComment::systemUsername()
{
	QString username = qgetenv("USER");
	if (username.isEmpty())
		username = qgetenv("USERNAME");
	return username;
}

bool ReviewComment::parseComment(FocusInformation& focusInformation)
{
	auto firstLine = commentNode()->lines()->first();
	if (!firstLine) return false;
	auto lineText = firstLine->get();
	if (lineText.startsWith("[focus"))
	{
		auto endOfCommand = lineText.indexOf("]");
		// subtract one because of starting [
		auto commandSize = endOfCommand - 1;
		auto commandText = lineText.mid(1, commandSize);
		QStringList commandTokens = commandText.split("|");

		// remove first token which is "focus"
		commandTokens.pop_front();

		if (!commandTokens.isEmpty())
		{
			auto token = commandTokens.takeFirst();
			if (token.startsWith("hi"))
				focusInformation.type_ = FocusInformation::FocusType::Highlight;
			else if (token.startsWith("ce"))
				focusInformation.type_ = FocusInformation::FocusType::Center;
		}

		if (!commandTokens.isEmpty())
		{
			auto stepToken = commandTokens.takeFirst();
			focusInformation.step_ = stepToken.toInt();
		}
		return true;
	}

	return false;
}


ReviewComment::ReviewComment(Model::Node* parent) :
	ReviewComment{new Comments::CommentNode{"comment here"},
							  QDateTime::currentMSecsSinceEpoch(), parent}
{}

ReviewComment::ReviewComment(Model::Node* node, Model::PersistentStore& store, bool partial)
	:Super{node, store, partial, ReviewComment::getMetaData()}
{}

ReviewComment* ReviewComment::clone() const { return new ReviewComment{*this}; }


}
