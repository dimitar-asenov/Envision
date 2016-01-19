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

#include "CToggleInfoEntry.h"

#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/nodes/InfoNode.h"

namespace Interaction {

CToggleInfoEntry::CToggleInfoEntry() : Command{"toggleInfo"}{}

bool CToggleInfoEntry::canInterpret(Visualization::Item* source, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	auto ancestor = source->findAncestorWithNode();
	return commandTokens.size() > 1 && commandTokens.first() == name()
			&& ancestor && DCast<Visualization::InfoNode>(ancestor->node());

}

CommandResult* CToggleInfoEntry::execute(Visualization::Item* source, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	auto info = DCast<Visualization::InfoNode>(source->findAncestorWithNode()->node());
	info->setEnabled(commandTokens[1], !(info->isEnabled(commandTokens[1])));
	info->automaticUpdate();
	source->findAncestorWithNode()->setUpdateNeeded(Visualization::Item::StandardUpdate);
	return new CommandResult{};
}

QList<CommandSuggestion*> CToggleInfoEntry::suggest(Visualization::Item*, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	QList<CommandSuggestion*> suggestions;
	if (textSoFar.startsWith(name() + " ") || name().startsWith(textSoFar))
	{
		auto parts = textSoFar.split(" ");
		auto nameSoFar = parts.size() > 1 ? parts[1] : "";
		for (auto layerName : Visualization::InfoNode::registeredInfoGetters())
			if (layerName.startsWith(nameSoFar))
				suggestions.append(new CommandSuggestion(name() + " " + layerName,
														"Toggle info layer " + layerName));
	}
	return suggestions;
}
}
