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

#include "TestBoxNode.h"

#include "ModelBase/src/nodes/TypedList.hpp"
template class Model::TypedList<Visualization::TestBoxNode>;

namespace Visualization {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(TestBoxNode)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(TestBoxNode)

REGISTER_ATTRIBUTE(TestBoxNode, red, Integer, false, false, true)
REGISTER_ATTRIBUTE(TestBoxNode, green, Integer, false, false, true)
REGISTER_ATTRIBUTE(TestBoxNode, blue, Integer, false, false, true)
REGISTER_ATTRIBUTE(TestBoxNode, sizeDependsOnParent, Boolean, false, false, true)
REGISTER_ATTRIBUTE(TestBoxNode, isHtml, Boolean, false, false, true)
REGISTER_ATTRIBUTE(TestBoxNode, label, Text, false, false, true)

TestBoxNode::TestBoxNode(const QString& label, QColor color, bool sizeDependsOnParent, bool isHtml)
: Super{nullptr, TestBoxNode::getMetaData()}
{
	setRed(color.red());
	setGreen(color.green());
	setBlue(color.blue());
	setLabel(label);
	setSizeDependsOnParent(sizeDependsOnParent);
	setIsHtml(isHtml);
}

TestBoxNode::TestBoxNode(const QString& label, bool sizeDependsOnParent, bool isHtml)
: Super{nullptr, TestBoxNode::getMetaData()}
{
	QColor color = Qt::yellow;
	setRed(color.red());
	setGreen(color.green());
	setBlue(color.blue());
	setLabel(label);
	setSizeDependsOnParent(sizeDependsOnParent);
	setIsHtml(isHtml);
}

}
