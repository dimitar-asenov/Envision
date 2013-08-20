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

#include "CommentDiagramConnector.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(Comments::CommentDiagramConnector)

namespace Comments {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(CommentDiagramConnector)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(CommentDiagramConnector)

REGISTER_ATTRIBUTE(CommentDiagramConnector, label, Text, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramConnector, x, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramConnector, y, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramConnector, width, Integer, false, false, true)
REGISTER_ATTRIBUTE(CommentDiagramConnector, height, Integer, false, false, true)

// references for primitive types?
CommentDiagramConnector::CommentDiagramConnector(const QString& label, const int& x, const int& y, const int& width, const int& height)
: Super{nullptr, CommentDiagramConnector::getMetaData()}
{
	setLabel(label);
	setX(x);
	setY(y);
	setWidth(width);
	setHeight(height);
}

QSize CommentDiagramConnector::size()
{
	return QSize(width(), height());
}

QSize CommentDiagramConnector::pos()
{
	return QSize(x(), y());
}

} /* namespace Comments */
