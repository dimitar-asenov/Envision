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

#include "CToggleArrowLayer.h"

#include "VisualizationBase/src/items/ViewItem.h"

namespace Interaction {

CToggleArrowLayer::CToggleArrowLayer() : Command{"toggleLayer"}{}

bool CToggleArrowLayer::canInterpret(Visualization::Item*, Visualization::Item*,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	return commandTokens.size() > 1 && commandTokens.first() == name();
}

CommandResult* CToggleArrowLayer::execute(Visualization::Item*, Visualization::Item* target,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	auto fullName = target->scene()->currentViewItem()->fullLayerName(commandTokens[1]);
	if (auto overlay = target->scene()->overlayGroup(fullName))
		overlay->toggle();
	return new CommandResult{};
}

QList<CommandSuggestion*> CToggleArrowLayer::suggest(Visualization::Item* source, Visualization::Item*,
		const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>&)
{
	QList<CommandSuggestion*> suggestions;
	if (textSoFar.startsWith(name() + " ") || name().startsWith(textSoFar))
	{
		auto parts = textSoFar.split(" ");
		auto nameSoFar = parts.size() > 1 ? parts[1] : "";
		for (auto layerName : source->scene()->currentViewItem()->arrowLayers())
			if (layerName.startsWith(nameSoFar))
				suggestions.append(new CommandSuggestion(name() + " " + layerName,
										"Toggle the arrow layer " + layerName));
	}
	return suggestions;
}
}
