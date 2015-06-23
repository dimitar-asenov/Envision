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
	:CommandWithDefaultArguments("addCallees", {""})
{
}

bool CAddCalleesToView::canInterpret(Visualization::Item* source, Visualization::Item* target,
	const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor)
{
	bool canInterpret = CommandWithDefaultArguments::canInterpret(source, target, commandTokens, cursor);
	auto ancestor = source->findAncestorWithNode();
	if (!ancestor) return false;
	else
		return canInterpret && DCast<OOModel::Method>(ancestor->node());
}

Interaction::CommandResult* CAddCalleesToView::executeWithArguments(Visualization::Item* source, Visualization::Item*,
		const QStringList& arguments, const std::unique_ptr<Visualization::Cursor>&)
{
	auto ancestor = source->findAncestorWithNode();
	auto name = arguments.at(0);

	auto view = ancestor->scene()->currentViewItem();

	if (view)
	{
		auto callees_ = (DCast<OOModel::Method>(ancestor->node()))->callees();
		auto pos = view->positionOfItem(ancestor->parent());

		if (callees_.size() > 0)
		{
			Model::Node* actualNode{};
			//TODO@cyril What if it is in the view, but not as a top-level item?
			if (pos.x() == -1)
			{
				view->insertColumn(0);
				actualNode = view->insertNode(ancestor->node(), 0, 0);
				pos = view->positionOfNode(actualNode);
			}
			else actualNode = ancestor->parent()->node();
			view->insertColumn(pos.x() + 1);
			auto row = 0;
			//Make the first callee appear at the same height as the method
			view->addSpacing(pos.x() + 1, row++, actualNode);
			for (auto callee : callees_)
			{
				auto actualCallee = view->insertNode(callee, pos.x() + 1, row++);
				view->addArrow(actualNode, actualCallee, "callees");
			}
		}
		return new Interaction::CommandResult();
	}
	else
		return new Interaction::CommandResult(new Interaction::CommandError("View " + name + " does not exist"));
}

QString CAddCalleesToView::description(Visualization::Item*, Visualization::Item*,
		const QStringList&, const std::unique_ptr<Visualization::Cursor>&)
{
	return "Add the callees of the current method to the current view";

}

}
