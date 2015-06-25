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

#include "CAddBaseClassesToView.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/expressions/MethodCallExpression.h"
#include "VisualizationBase/src/items/VViewItemNode.h"


namespace OOInteraction {

CAddBaseClassesToView::CAddBaseClassesToView()
	:CommandWithDefaultArguments("addSuperclasses", {})
{
}

bool CAddBaseClassesToView::canInterpret(Visualization::Item* source, Visualization::Item* target,
	const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor)
{
	bool canInterpret = CommandWithDefaultArguments::canInterpret(source, target, commandTokens, cursor);
	auto ancestorWithNode = source->findAncestorWithNode();
	auto topLevelAncestor = source->findAncestorOfType<Visualization::VViewItemNode>();
	if (!ancestorWithNode || !topLevelAncestor) return false;
	else
		return canInterpret && DCast<OOModel::Class>(ancestorWithNode->node());
}

Interaction::CommandResult* CAddBaseClassesToView::executeWithArguments(Visualization::Item* source,
		Visualization::Item*, const QStringList&, const std::unique_ptr<Visualization::Cursor>&)
{
	auto ancestorWithNode = source->findAncestorWithNode();
	auto topLevelAncestor = source->findAncestorOfType<Visualization::VViewItemNode>();

	auto view = ancestorWithNode->scene()->currentViewItem();

	auto baseCl = baseClasses(DCast<OOModel::Class>(ancestorWithNode->node()));
	if (baseCl.size() > 0)
	{

		auto pos = view->positionOfItem(topLevelAncestor);
		view->insertColumn(pos.x());
		auto row = 0;
		//Make the first superclass appear at the same height as the class
		view->addSpacing(pos.x(), row++, ancestorWithNode->node(), topLevelAncestor->node());
		for (auto baseClass : baseCl)
		{
			auto actualBaseClass = view->insertNode(baseClass, pos.x(), row++);
			view->addArrow(ancestorWithNode->node(), actualBaseClass, "inheritance", topLevelAncestor->node());
		}
	}
	return new Interaction::CommandResult();
}

QString CAddBaseClassesToView::description(Visualization::Item*, Visualization::Item*,
		const QStringList&, const std::unique_ptr<Visualization::Cursor>&)
{
	return "Add the base classes of the current class to the current view";

}

QList<OOModel::Class*> CAddBaseClassesToView::baseClasses(OOModel::Class *parent)
{
	QList<OOModel::Class*> result;
	for (auto baseClass : *(parent->baseClasses()))
		if (auto asClass = OOModel::Class::expressionToClass(baseClass))
			result.append(asClass);
	return result;
}

}
