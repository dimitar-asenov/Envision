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
#pragma once

#include "../codereview_api.h"

#include "ModelBase/src/nodes/nodeMacros.h"

#include "VersionControlUI/src/nodes/DiffComparisonPair.h"

#include "Comments/src/nodes/CommentNode.h"

#include "ModelBase/src/nodes/Text.h"

namespace CodeReview
{
class ReviewComment;
}

extern template class CODEREVIEW_API Model::TypedList<CodeReview::ReviewComment>;

namespace CodeReview {

class CODEREVIEW_API ReviewComment : public Super<Model::CompositeNode>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(ReviewComment)

	ATTRIBUTE(Comments::CommentNode, commentNode, setComment)
	ATTRIBUTE_VALUE(Model::Text, username, setUsername, QString)
	PRIVATE_ATTRIBUTE_VALUE(Model::Text, dateString, setDateString, QString)

	public:
		ReviewComment(Comments::CommentNode* commentNode, qint64 date, Model::Node* parent=nullptr);
		qint64 date();
		void setDate(qint64 date);

		// TODO move to more fitting place
		static QString systemUsername();
};

inline qint64 ReviewComment::date() { return dateString().toLongLong(); }
inline void ReviewComment::setDate(qint64 date) { setDateString(QString::number(date)); }

}
