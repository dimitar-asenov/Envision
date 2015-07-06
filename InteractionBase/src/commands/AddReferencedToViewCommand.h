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

#pragma once

#include "../InteractionBase/src/interactionbase_api.h"
#include "CommandWithDefaultArguments.h"
#include "VisualizationBase/src/items/VViewItemNode.h"
#include "VisualizationBase/src/items/ViewItem.h"

namespace Visualization {
	class Item;
}
namespace Interaction {

template <class ReferenceTarget, class ReferenceResult>
class INTERACTIONBASE_API AddReferencedToViewCommand : public CommandWithDefaultArguments
{
	public:
		AddReferencedToViewCommand(QString name, const QStringList& defaultArguments,
								   int rightOffset, QString arrowLayer, int purpose = -1);

		virtual bool canInterpret(Visualization::Item* source, Visualization::Item* target,
				const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor);

	protected:
		virtual CommandResult* executeWithArguments(Visualization::Item* source, Visualization::Item* target,
				const QStringList& arguments, const std::unique_ptr<Visualization::Cursor>& cursor);

		virtual QSet<ReferenceResult*> references(ReferenceTarget* target) = 0;

	private:
		int rightOffset_{};
		QString arrowLayer_{};
		int purpose_{};
};

template <class ReferenceTarget, class ReferenceResult>
inline AddReferencedToViewCommand<ReferenceTarget, ReferenceResult>::
	AddReferencedToViewCommand(QString name, const QStringList &defaultArguments,
												int rightOffset, QString arrowLayer, int purpose)
	: CommandWithDefaultArguments(name, defaultArguments),
		rightOffset_(rightOffset), arrowLayer_(arrowLayer), purpose_(purpose) {}

template <class ReferenceTarget, class ReferenceResult>
inline bool AddReferencedToViewCommand<ReferenceTarget, ReferenceResult>::
	canInterpret(Visualization::Item* source, Visualization::Item* target,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor)
{
	bool canInterpret = CommandWithDefaultArguments::canInterpret(source, target, commandTokens, cursor);
	auto ancestorWithNode = source->findAncestorWithNode();
	auto topLevelAncestor = source->findAncestorOfType<Visualization::VViewItemNode>();
	if (!ancestorWithNode || !topLevelAncestor) return false;
	else
		return canInterpret && DCast<ReferenceTarget>(ancestorWithNode->node());

}

template <class ReferenceTarget, class ReferenceResult>
CommandResult* AddReferencedToViewCommand<ReferenceTarget, ReferenceResult>::
	executeWithArguments(Visualization::Item* source, Visualization::Item*,
		const QStringList&, const std::unique_ptr<Visualization::Cursor>&)
{
	auto ancestorWithNode = source->findAncestorWithNode();
	auto topLevelAncestor = source->findAncestorOfType<Visualization::VViewItemNode>();

	auto view = ancestorWithNode->scene()->currentViewItem();

	auto refs = references(DCast<ReferenceTarget>(ancestorWithNode->node()));
	if (refs.size() > 0)
	{
		auto pos = view->positionOfItem(topLevelAncestor);
		view->insertColumn(pos.x() + rightOffset_);
		auto row = 0;
		//Make the items appear at the same height as the item they reference
		view->addSpacing(pos.x() + rightOffset_, row++, ancestorWithNode->node(), topLevelAncestor->node());
		for (auto ref : refs)
		{
			auto actualRef = view->insertNode(ref, pos.x() + rightOffset_, row++, purpose_);
			view->addArrow(ancestorWithNode->node(), actualRef, arrowLayer_, topLevelAncestor->node());
		}
	}
	return new Interaction::CommandResult();
}

}
