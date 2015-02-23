/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "CProbe.h"

#include "OOVisualization/src/elements/VStatementItemList.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"

#include "../debugger/JavaDebugger.h"

namespace OODebug {

bool CProbe::canInterpret(Visualization::Item*, Visualization::Item*, const QStringList& commandTokens,
								  const std::unique_ptr<Visualization::Cursor>&)
{
	return commandTokens.size() >= 1 && commandTokens.first() == "probe";
}

Interaction::CommandResult* CProbe::execute(Visualization::Item*, Visualization::Item* target,
	const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor)
{
	auto itemList = static_cast<OOVisualization::VStatementItemList*>(target);
	Q_ASSERT(itemList);
	// find the statement item corresponding to the cursor:
	auto cursorP = cursor.get();
	int itemIndex = -1;
	if (auto layoutCursor = dynamic_cast<Visualization::LayoutCursor*>(cursorP))
		itemIndex = layoutCursor->index(); // this corresponds to the statement below
	else
		itemIndex = itemList->indexOfChildOrSubChild(cursor->owner()) + 1; // we want the next item
	Q_ASSERT(itemIndex > -1);
	QStringList arguments = commandTokens;
	arguments.removeFirst(); // Remove the probe command
	JavaDebugger::instance().probe(itemList, arguments, itemIndex);
	return new Interaction::CommandResult();
}

QStringList CProbe::commandForms(Visualization::Item*, Visualization::Item*, const QString& textSoFar,
											const std::unique_ptr<Visualization::Cursor>&)
{
	QStringList forms;
	if (textSoFar.isEmpty() || QString("probe").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
		forms.append("probe");
	return forms;
}

} /* namespace OODebug */
