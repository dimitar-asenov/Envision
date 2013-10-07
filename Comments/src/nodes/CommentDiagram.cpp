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

#include "CommentDiagram.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(Comments::CommentDiagram)

namespace Comments {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(CommentDiagram)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(CommentDiagram)

REGISTER_ATTRIBUTE(CommentDiagram, name, Text, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagram, width, Float, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagram, height, Float, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagram, shapes, TypedListOfCommentDiagramShape, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagram, connectors, TypedListOfCommentDiagramConnector, false, false, true)

CommentDiagram::CommentDiagram(Node *parent, QString name)
: Super(parent, CommentDiagram::getMetaData())
{
	setName(name);
	setWidth(0);
	setHeight(0);
}

void CommentDiagram::setSize(QSizeF size)
{
	setWidth(size.width());
	setHeight(size.height());
}

void CommentDiagram::addConnector(int shape1, int point1, int shape2, int point2)
{
	model()->beginModification(this, "Adding connector");
	connectors()->append(new CommentDiagramConnector(shape1, point1, shape2, point2));
	model()->endModification();
}

void CommentDiagram::removeConnector(CommentDiagramConnector *connector)
{
	model()->beginModification(this, "removing connector from diagram");
	connectors()->remove(connector);
	model()->endModification();
}

void CommentDiagram::removeShape(CommentDiagramShape *shape)
{
	// what's the index of this shape?
	int shapeIndex = shapes()->indexOf(shape);
	Q_ASSERT(shapeIndex != -1);

	model()->beginModification(this, "removing shape from diagram");

	// the size may change inside the loop, cache its original value
	int connectorsSize = connectors()->size();
	// find all connectors that reference this shape
	for(int i = 0; i < connectorsSize; ++i)
	{
		// iterate in reverse to avoid ordering issues
		int index = connectorsSize - i - 1;
		auto c = connectors()->at(index);
		if(c->shape1() == shapeIndex || c->shape2() == shapeIndex)
			connectors()->remove(index);
		// else decrease all shape indexes bigger than the removed one by one
		else
		{
			if(c->shape1() > shapeIndex)
				c->setShape1(c->shape1() - 1);
			if(c->shape2() > shapeIndex)
				c->setShape2(c->shape2() - 1);
		}
	}

	shapes()->remove(shapeIndex);
	model()->endModification();
}

} /* namespace Comments */
