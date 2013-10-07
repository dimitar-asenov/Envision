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

#pragma once

#include "../comments_api.h"

#include "CommentDiagramConnector.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/nodes/Integer.h"
#include "ModelBase/src/nodes/nodeMacros.h"
#include "ModelBase/src/nodes/TypedList.h"

DECLARE_TYPED_LIST(COMMENTS_API, Comments, CommentDiagramConnector)

namespace Comments {

class COMMENTS_API CommentDiagramConnector : public Super<Model::CompositeNode> {
	COMPOSITENODE_DECLARE_STANDARD_METHODS(CommentDiagramConnector)

	ATTRIBUTE_VALUE_CUSTOM_RETURN(::Model::Text, label, setLabel, QString, const QString&)
	ATTRIBUTE_VALUE(::Model::Integer, shape1, setShape1, int)
	ATTRIBUTE_VALUE(::Model::Integer, point1, setPoint1, int)
	ATTRIBUTE_VALUE(::Model::Integer, shape2, setShape2, int)
	ATTRIBUTE_VALUE(::Model::Integer, point2, setPoint2, int)

	public:
		CommentDiagramConnector(const int& shape1, const int& point1, const int& shape2, const int& point2);

	friend QDebug operator<<(QDebug dbg, const CommentDiagramConnector *c);
};

} /* namespace Comments */
