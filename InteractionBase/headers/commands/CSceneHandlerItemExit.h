/***********************************************************************************************************************
 * CSceneHandlerItemExit.h
 *
 *  Created on: Jan 19, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CSCENEHANDLERITEMEXIT_H_
#define CSCENEHANDLERITEMEXIT_H_

#include "../interactionbase_api.h"

#include "../commands/Command.h"

namespace Interaction {

class INTERACTIONBASE_API CSceneHandlerItemExit : public Command
{
	public:
		virtual bool canInterpret(Visualization::Item* source, Visualization::Item* target, const QStringList& commandTokens);
		virtual CommandResult* execute(Visualization::Item* source, Visualization::Item* target, const QStringList& commandTokens);

		virtual QList<CommandSuggestion*> suggest(Visualization::Item* source, Visualization::Item* target, const QString& textSoFar);
		virtual QStringList commandForms(Visualization::Item* source, Visualization::Item* target, const QString& textSoFar);
};

}

#endif /* CSCENEHANDLERITEMEXIT_H_ */
