/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "VBoolean.h"

namespace Visualization {

DEFINE_ITEM_COMMON(VBoolean, "item")

VBoolean::VBoolean(Item* parent, NodeType *node, const StyleType *style) : Super{parent, node, style}
{
	if (node->get()) TextRenderer::setText( "true" );
	else TextRenderer::setText( "false" );
}

bool VBoolean::setText(const QString& newText)
{
	if (newText.contains(' ') || newText.compare("true", Qt::CaseInsensitive) == 0
		 || newText.compare("false", Qt::CaseInsensitive) == 0)
	{
		node()->beginModification("Set boolean");
		if ( newText.contains(' ') ) node()->set( !node()->get() );
		else if ( newText.compare("true", Qt::CaseInsensitive) == 0 ) node()->set(true);
		else node()->set(false);
		node()->endModification();
		if (node()->get()) TextRenderer::setText( "true" );
		else TextRenderer::setText( "false" );
		return true;
	}
	else return false;
}

QString VBoolean::currentText()
{
	return node()->get() ? "true" : "false";
}

}
