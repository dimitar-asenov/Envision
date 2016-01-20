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

#include "CAddNodeToView.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/ViewItemManager.h"

namespace Interaction {

CAddNodeToView::CAddNodeToView()
	:CommandWithDefaultArguments{"addNode", {"current", "0", "0"}}
{
}

bool CAddNodeToView::canInterpret(Visualization::Item* source, Visualization::Item* target,
	const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor)
{
	bool canInterpret = CommandWithDefaultArguments::canInterpret(source, target, commandTokens, cursor);
	//The first parent with a node should be a declaration (these can be added to the view here)
	auto ancestor = source->findAncestorWithNode();
	if (!ancestor) return false;
	else
		return canInterpret && ancestor->node()->definesSymbol()
				&& (ancestor->node()->symbolType() == Model::Node::METHOD
					|| ancestor->node()->symbolType() == Model::Node::CONTAINER);
}

CommandResult* CAddNodeToView::executeWithArguments(Visualization::Item* source, Visualization::Item*,
		const QStringList& arguments, const std::unique_ptr<Visualization::Cursor>&)
{
	auto ancestor = source->findAncestorWithNode();
	auto name = arguments.at(0);
	auto colOk = true;
	auto rowOk = true;
	auto column = arguments.at(1).toInt(&colOk);
	auto row = arguments.at(2).toInt(&rowOk);
	if (name == "current")
		name = ancestor->scene()->currentViewItem()->name();
	auto view = ancestor->scene()->viewItems()->viewItem(name);
	if (view && rowOk && colOk)
	{
		view->insertNode(ancestor->node(), column, row);
		return new CommandResult{};
	}
	else if (!view)
		return new CommandResult{new CommandError{"The view with name " + name + " does not exist"}};
	else if (!colOk)
		return new CommandResult{new CommandError{arguments.at(1) + " is not an integer"}};
	else
		return new CommandResult{new CommandError{arguments.at(2) + " is not an integer"}};

}

QString CAddNodeToView::description(Visualization::Item *, Visualization::Item *,
		const QStringList &arguments, const std::unique_ptr<Visualization::Cursor> &)
{
	auto name = arguments.at(0);
	if (name == "current")
		return "Add the current item to the current view at column " +
				arguments.at(1) + " and row " + arguments.at(2);
	else
		return "Add the current item to the view " + name + " at column " +
				arguments.at(1) + " and row " + arguments.at(2);

}
}
