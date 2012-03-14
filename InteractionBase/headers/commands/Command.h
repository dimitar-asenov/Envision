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
 * Command.h
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef COMMAND_H_
#define COMMAND_H_

#include "../interactionbase_api.h"

#include "CommandResult.h"
#include "CommandSuggestion.h"
#include "CommandHelp.h"

#include "VisualizationBase/headers/items/Item.h"

#include <QtCore/QStringList>
#include <QtCore/QList>

namespace Interaction {

class INTERACTIONBASE_API Command
{
	public:
		virtual ~Command();

		virtual bool canInterpret(Visualization::Item* source, Visualization::Item* target, const QStringList& commandTokens);
		virtual CommandResult* execute(Visualization::Item* source, Visualization::Item* target, const QStringList& commandTokens) = 0;

		virtual QList<CommandSuggestion*> suggest(Visualization::Item* source, Visualization::Item* target, const QString& textSoFar);
		virtual QStringList commandForms(Visualization::Item* source, Visualization::Item* target, const QString& textSoFar) = 0;
		virtual QList<CommandHelp*> extendedHelp(Visualization::Item* source, Visualization::Item* target, const QString& commandForm = QString());
};

}

#endif /* COMMAND_H_ */
