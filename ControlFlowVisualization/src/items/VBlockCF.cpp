/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * VBlockCF.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VBlockCF.h"

using namespace OOVisualization;
using namespace Visualization;
using namespace OOModel;
using namespace Model;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(VBlockCF, "item")

VBlockCF::VBlockCF(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<ControlFlowItem, Block>(parent, node, style),
	statements(nullptr),
	vis_(nullptr)
{
}

VBlockCF::~VBlockCF()
{
	SAFE_DELETE_ITEM(statements);
	SAFE_DELETE_ITEM(vis_);
}

bool VBlockCF::sizeDependsOnParent() const
{
	if (showAsControlFlow()) return statements->sizeDependsOnParent();
	else return vis_->sizeDependsOnParent();
}

bool VBlockCF::isEmpty() const
{
	if (showAsControlFlow()) return statements->isEmpty();
	else return vis_->isEmpty();
}

void VBlockCF::determineChildren()
{
	if (showAsControlFlow())
	{
		SAFE_DELETE_ITEM(vis_);

		synchronizeItem(statements, node()->items(), &style()->statements());

		statements->setStyle(&style()->statements());
		statements->setPreferredBreakExit(preferredBreakExit_);
		statements->setPreferredContinueExit(preferredContinueExit_);
	}
	else
	{
		SAFE_DELETE_ITEM(statements);

		synchronizeItem<VBlock>(vis_, node(), nullptr);
	}
}

void VBlockCF::updateGeometry(int availableWidth, int availableHeight)
{
	clearConnectors();
	breaks_.clear();
	continues_.clear();

	if (!showAsControlFlow())
	{
		Item::updateGeometry(vis_, availableWidth, availableHeight);
		return;
	}

	Item::updateGeometry(statements, availableWidth, availableHeight);

	entrance_ = statements->entrance();
	exit_ = statements->exit();
	breaks_ = statements->breaks();
	continues_ = statements->continues();
}

}
