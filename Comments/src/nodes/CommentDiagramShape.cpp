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

#include "CommentDiagramShape.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(Comments::CommentDiagramShape)

namespace Comments {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(CommentDiagramShape)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(CommentDiagramShape)

REGISTER_ATTRIBUTE(CommentDiagramShape, label, Text, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, x, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, y, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, width, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, height, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramShape, shapeType, Integer, false, false, true)

// references for primitive types?
CommentDiagramShape::CommentDiagramShape(const QString& label, const int& x, const int& y, const int& width,
		const int& height, enum CommentDiagramShapeType shapeType)
: Super{nullptr, CommentDiagramShape::getMetaData()}
{
	setLabel(label);
	setX(x);
	setY(y);
	setWidth(width);
	setHeight(height);
	setShapeType(shapeType);
}

QSize CommentDiagramShape::size()
{
	return QSize(width(), height());
}

QSize CommentDiagramShape::pos()
{
	return QSize(x(), y());
}

} /* namespace Comments */
