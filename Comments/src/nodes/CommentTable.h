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

#pragma once

#include "../comments_api.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"
#include "ModelBase/src/nodes/TypedList.h"
#include "ModelBase/src/nodes/List.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/nodes/Integer.h"
#include "CommentFreeNode.h"

namespace Comments {
	class CommentTable;
}
extern template class COMMENTS_API Model::TypedList<Comments::CommentTable>;

namespace Comments {
/**
 * The CommentTable class offers a comment element to display tables.
 * These tables are represented as a grid of nodes of type CommentFreeNode.
 */
class COMMENTS_API CommentTable : public Super<Model::CompositeNode>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(CommentTable)

	ATTRIBUTE_VALUE(Model::Text, name, setName, QString)
	ATTRIBUTE_VALUE(Model::Integer, rowCount, setRowCount, int)
	ATTRIBUTE_VALUE(Model::Integer, columnCount, setColumnCount, int)
	ATTRIBUTE(Model::TypedList<CommentFreeNode>, nodes, setNodes)

	public:
		CommentTable(Node *parent, QString name, int rowCount, int columnCount);
		void setNodeAt(int m, int n, Model::Node* aNode);
		CommentFreeNode* nodeAt(int m, int n);
		void resize(int m, int n);
};

}
