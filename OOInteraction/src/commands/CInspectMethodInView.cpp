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
#include "CInspectMethodInView.h"

#include "OOModel/src/declarations/Method.h"
#include "VisualizationBase/src/ViewItemManager.h"
#include "VisualizationBase/src/items/ViewItem.h"

namespace OOInteraction {

CInspectMethodInView::CInspectMethodInView()
	:CommandWithDefaultArguments{"inspectMethod", {}}
{
}

bool CInspectMethodInView::canInterpret(Visualization::Item *source, Visualization::Item *target,
	const QStringList &commandTokens, const std::unique_ptr<Visualization::Cursor> &cursor)
{
	auto canInterpret = CommandWithDefaultArguments::canInterpret(source, target, commandTokens, cursor);

	auto ancestorWithNode = source->findAncestorWithNode();

	if (!ancestorWithNode) return false;
	else return canInterpret && (DCast<OOModel::Method>(ancestorWithNode->node())
								 || ancestorWithNode->node()->firstAncestorOfType<OOModel::Method>());
}

Interaction::CommandResult* CInspectMethodInView::executeWithArguments(Visualization::Item *source,
	Visualization::Item *, const QStringList &, const std::unique_ptr<Visualization::Cursor> &)
{
	auto node = source->findAncestorWithNode()->node();
	auto method = (DCast<OOModel::Method>(node)) ? DCast<OOModel::Method>(node) :
												   node->firstAncestorOfType<OOModel::Method>();

	auto callers = method->callers();
	auto callees = method->callees();

	auto view = source->scene()->viewItems()->newViewItem("Inspect_" + method->name());
	if (!view) return new Interaction::CommandResult{new Interaction::CommandError{"Already inspecting method"}};

	auto methodInView = view->insertNode(method, {1, 0});
	for (auto caller : callers)
	{
		auto callerInView = view->insertNode(caller, {0, 0});
		view->addArrow(caller, method, "callees", callerInView, methodInView);
	}
	for (auto callee : callees)
	{
		auto calleeInView = view->insertNode(callee, {2, 0}, Visualization::ViewItem::publicInterfacePurpose());
		view->addArrow(method, callee, "callees", methodInView, calleeInView);
	}
	source->scene()->viewItems()->switchToView(view);
	return new Interaction::CommandResult{};
}

QString CInspectMethodInView::description(Visualization::Item *, Visualization::Item *,
	const QStringList &, const std::unique_ptr<Visualization::Cursor> &)
{
	return "Inspect the current method in a new view";
}

}
