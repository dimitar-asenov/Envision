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
#include "NodeReviews.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"
#include "ModelBase/src/nodes/TypedList.hpp"

template class Model::TypedList<CodeReview::NodeReviews>;

namespace CodeReview
{
DEFINE_COMPOSITE_EMPTY_CONSTRUCTORS(NodeReviews)
DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS(NodeReviews)

DEFINE_ATTRIBUTE(NodeReviews, nodeId, Text, false, false, true)
DEFINE_ATTRIBUTE(NodeReviews, revisionName, Text, false, false, true)
DEFINE_ATTRIBUTE(NodeReviews, reviewComments, TypedListOfReviewComment, false, false, true)
DEFINE_ATTRIBUTE(NodeReviews, offsetX, Integer, false, false, true)
DEFINE_ATTRIBUTE(NodeReviews, offsetY, Integer, false, false, true)

NodeReviews::NodeReviews(QString associatedNodeId, QString revisionName, QPoint offset)
	: Super{nullptr, NodeReviews::getMetaData()}
{
	setNodeId(associatedNodeId);
	setRevisionName(revisionName);
	setOffsetX(offset.x());
	setOffsetY(offset.y());
}

}
