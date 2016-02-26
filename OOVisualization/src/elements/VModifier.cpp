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

#include "VModifier.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VModifier, "item")

VModifier::VModifier(Item* parent, NodeType* vis, const StyleType* style) : Super{parent, vis, style}
{}

bool VModifier::setText(const QString&)
{
	return false;
}

QString VModifier::currentText()
{
	QString text;
	if (node()->isSet(Modifier::Static)) text += " static";
	if (node()->isSet(Modifier::Public)) text += " public";
	if (node()->isSet(Modifier::Protected)) text += " protected";
	if (node()->isSet(Modifier::Private)) text += " private";
	if (node()->isSet(Modifier::Final)) text += " final";
	if (node()->isSet(Modifier::Abstract)) text += " abstract";
	if (node()->isSet(Modifier::Virtual)) text += " virtual";
	if (node()->isSet(Modifier::Override)) text += " override";
	if (node()->isSet(Modifier::Inline)) text += " inline";

	if (!text.isEmpty() && text.at(0) == ' ') text.remove(0, 1);
	return text;
}

}
