/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
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

#include "CRemoveNodeFromView.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/items/VViewItemNode.h"

namespace Interaction {

CRemoveNodeFromView::CRemoveNodeFromView()
	:CommandWithDefaultArguments{"removeNode", QStringList{}}
{
}

bool CRemoveNodeFromView::canInterpret(Visualization::Item *source, Visualization::Item *target,
	const QStringList &commandTokens, const std::unique_ptr<Visualization::Cursor> &cursor)
{
	bool canInterpret = CommandWithDefaultArguments::canInterpret(source, target, commandTokens, cursor);
	//The first parent with a node should be in the current view
	auto ancestor = potentialTopLevelParent(source);
	if (!ancestor) return false;
	else
		return canInterpret &&
				ancestor->scene()->currentViewItem()->positionOfItem(ancestor) != QPoint{-1, -1};
}

CommandResult* CRemoveNodeFromView::executeWithArguments(Visualization::Item *source, Visualization::Item *,
		const QStringList&, const std::unique_ptr<Visualization::Cursor>&)
{
	//Go to the first parent of the source which has a node, which is a top-level item (as canInterpret checks)
	auto ancestor = potentialTopLevelParent(source);
	ancestor->scene()->currentViewItem()->removeNode(ancestor->node());
	return new CommandResult{};
}

QString CRemoveNodeFromView::description(Visualization::Item *, Visualization::Item *,
		const QStringList &, const std::unique_ptr<Visualization::Cursor> &)
{
	return "Remove the current top-level node from the current view";
}

Visualization::Item* CRemoveNodeFromView::potentialTopLevelParent(Visualization::Item *child)
{
	//The parent item to be used for an item can be deduced as follows:
	//Either the item is a spacing node -> then the first ancestor with a node will be a VViewItemNode
	//Else the item is a real AST node -> then the AST node's visualization will be found,
	//									  and the potential top level item in the view is its parent
	auto ancestor = child->findAncestorWithNode();
	if (DCast<Visualization::VViewItemNode>(ancestor)) return ancestor;
	else if (ancestor) return ancestor->parent();
	else return nullptr;
}

}
