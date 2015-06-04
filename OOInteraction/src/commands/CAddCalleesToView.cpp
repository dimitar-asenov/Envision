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

#include "CAddCalleesToView.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/expressions/MethodCallExpression.h"


namespace OOInteraction {

CAddCalleesToView::CAddCalleesToView()
	:CommandWithDefaultArguments("addCallees", {"current"})
{
}

bool CAddCalleesToView::canInterpret(Visualization::Item* source, Visualization::Item* target,
	const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor)
{
	bool canInterpret = CommandWithDefaultArguments::canInterpret(source, target, commandTokens, cursor);
	auto ancestor = source->findAncestorWithNode();
	if (!ancestor) return false;
	else
		return canInterpret && dynamic_cast<OOModel::Method*>(ancestor->node());
}

Interaction::CommandResult* CAddCalleesToView::executeWithArguments(Visualization::Item* source, Visualization::Item*,
		const QStringList& arguments, const std::unique_ptr<Visualization::Cursor>&)
{
	auto ancestor = source->findAncestorWithNode();
	auto name = arguments.at(0);

	if (name == "current")
		name = ancestor->scene()->currentViewItem()->name();
	auto view = ancestor->scene()->viewItem(name);

	if (view)
	{
		auto callees_ = callees(ancestor->node());
		auto pos = view->positionOfNode(ancestor->node());

		if (callees_.size() > 0)
		{
			//TODO@cyril What if it is in the view, but not as a top-level item?
			if (pos.x() == -1)
			{
				view->insertColumn(0);
				view->insertNode(ancestor->node(), 0, 0);
				pos = view->positionOfNode(ancestor->node());
			}
			//TODO@cyril Insert the nodes in a good order
			view->insertColumn(pos.x() + 1);
			auto row = callees_.size() - 1;
			for (auto callee : callees_)
				view->insertNode(callee, pos.x() + 1, row--);
		}
		return new Interaction::CommandResult();
	}
	else
		return new Interaction::CommandResult(new Interaction::CommandError("View " + name + " does not exist"));
}

QSet<OOModel::Method*> CAddCalleesToView::callees(Model::Node* parent)
{
	QSet<OOModel::Method*> result;
	for (auto child : parent->children())
	{
		if (auto call = dynamic_cast<OOModel::MethodCallExpression*>(child))
			if (call->methodDefinition())
				result.insert(call->methodDefinition());
		result.unite(callees(child));
	}
	return result;
}


QString CAddCalleesToView::description(Visualization::Item*, Visualization::Item*,
		const QStringList& arguments, const std::unique_ptr<Visualization::Cursor>&)
{
	auto name = arguments.at(0);
	if (name == "current")
		return "Add the callees of the current method to the current view";
	else
		return "Add the callees of the current method to the view " + name;

}

}
