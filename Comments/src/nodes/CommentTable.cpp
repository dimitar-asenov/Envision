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

#include "CommentTable.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"

DEFINE_TYPED_LIST(Comments::CommentTable)

namespace Comments {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(CommentTable)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(CommentTable)

REGISTER_ATTRIBUTE(CommentTable, name, Text, false, false, true)
REGISTER_ATTRIBUTE(CommentTable, rowCount, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentTable, columnCount, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentTable, nodes, TypedListOfCommentFreeNode, false, false, true)

CommentTable::CommentTable(Node *parent, QString name, int rowCount, int columnCount)
	: Super(parent, CommentTable::getMetaData())
{
	setName(name);
	setRowCount(0);
	setColumnCount(0);
	resize(rowCount, columnCount);
}

void CommentTable::setNodeAt(int m, int n, Model::Node *aNode)
{
	nodes()->at(n*rowCount() + m)->setNode(aNode);
}

CommentFreeNode* CommentTable::nodeAt(int m, int n)
{
	return nodes()->at(n*rowCount() + m);
}

void CommentTable::resize(int m, int n)
{
	auto aList = new Model::TypedList<CommentFreeNode>;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			if (j < rowCount() && i < columnCount())
			{
				CommentFreeNode* aFreeNode = nodeAt(j, i);
				nodes()->replaceChild(aFreeNode, new CommentFreeNode{nullptr, ""});
				aFreeNode->setName(name()+"_"+QString::number(j)+"_"+QString::number(i));
				aList->append(aFreeNode);
			}
			else
				aList->append(new CommentFreeNode{nullptr, name()+"_"+QString::number(j)+"_"+QString::number(i)});
		}
	}

	replaceChild(nodes(), aList);
	setRowCount(m);
	setColumnCount(n);
}

}
