/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "items/VList.h"
#include "items/Text.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VList, "item")

VList::VList(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode< LayoutProvider<>, Model::List>(parent, node, style)
{
}

void VList::determineChildren()
{
	// TODO: find a better way and place to determine the style of children
	layout()->setStyle(&style()->itemsStyle());
	determineRange();
	layout()->synchronizeWithNodes(node()->nodes().toList().mid(rangeBegin_, rangeEnd_ - rangeBegin_), renderer());
}

void VList::determineRange()
{
	setRange(0, node()->size());
}

void VList::setRange(int begin, int end)
{
	Q_ASSERT(0 <= begin);
	Q_ASSERT(begin <= end);
	Q_ASSERT(end <= node()->size());
	Q_ASSERT(begin < node()->size() || node()->size() == 0);

	rangeBegin_ = begin;
	rangeEnd_ = end;
}

}
