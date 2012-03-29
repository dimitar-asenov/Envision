/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * CSceneHandlerItemExit.cpp
 *
 *  Created on: Jan 19, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/CSceneHandlerItemExit.h"

#include "VisualizationBase/src/VisualizationManager.h"

namespace Interaction {

bool CSceneHandlerItemExit::canInterpret(Visualization::Item*, Visualization::Item*, const QStringList& commandTokens)
{
	return commandTokens.size() == 1 && commandTokens.first() == "exit";
}

CommandResult* CSceneHandlerItemExit::execute(Visualization::Item*, Visualization::Item*, const QStringList&)
{
	Visualization::VisualizationManager::instance().getMainWindow()->close();

	return new CommandResult();
}

QList<CommandSuggestion*> CSceneHandlerItemExit::suggest(Visualization::Item*, Visualization::Item*, const QString& textSoFar)
{
	QList<CommandSuggestion*> s;
	if (QString("exit").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
			s.append(new CommandSuggestion("exit", "Closes Envision"));
	return s;
}

QStringList CSceneHandlerItemExit::commandForms(Visualization::Item*, Visualization::Item*, const QString& textSoFar)
{
	QStringList forms;
	if (textSoFar.isEmpty() || QString("exit").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
		forms.append("exit");
	return forms;
}

}
