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

ReviewComment::ReviewComment(Comments::CommentNode* commentNode, qint64 date, Model::Node* parent) :
	Super{parent, ReviewComment::getMetaData()}
{
	date_ = date;
	username_ = systemUsername();
	setComment(commentNode);
}

QString ReviewComment::systemUsername()
{
	QString username = qgetenv("USER");
	if (username.isEmpty())
		username = qgetenv("USERNAME");
	return username;
}


ReviewComment::ReviewComment(Model::Node* parent) :
	ReviewComment{new Comments::CommentNode{"comment here"},
							  QDateTime::currentMSecsSinceEpoch(), parent}
{}

ReviewComment::ReviewComment(Model::Node *, Model::PersistentStore &, bool)
	:Super{}
{
	Q_ASSERT(false);
}

ReviewComment* ReviewComment::clone() const { return new ReviewComment{*this}; }


}
