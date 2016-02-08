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
#include "CSaveView.h"

#include "VisualizationBase/src/ViewItemManager.h"

using namespace Visualization;

namespace Interaction {

CSaveView::CSaveView() : Command{"saveView"}{}

bool CSaveView::canInterpret(Item*, Item*, const QStringList& commandTokens,
		const std::unique_ptr<Visualization::Cursor>&)
{
	return (commandTokens.size() <= 2) && QString{"saveView"}.startsWith(commandTokens.first());
}

CommandResult* CSaveView::execute(Item* source, Item*, const QStringList&,
		const std::unique_ptr<Visualization::Cursor>&)
{
	//Try to use an existing manager if possible
	if (source->findAncestorWithNode() && source->findAncestorWithNode()->node()->manager())
	{
		source->scene()->viewItems()->saveView(source->scene()->currentViewItem(),
					source->findAncestorWithNode()->node()->manager());
		return new CommandResult{};
	}
	else return new CommandResult{new CommandError{"Could not save view"}};
}

QList<CommandSuggestion*> CSaveView::suggest(Item*, Item*, const QString& textSoFar,
		const std::unique_ptr<Visualization::Cursor>&)
{
	if (textSoFar.startsWith(name()) || name().startsWith(textSoFar))
		return {new CommandSuggestion{name(), "Save the current view"}};
	else return {};
}

}
